#include <opendrive_global_planner/opendrive_generator_server.h>

namespace path_planner
{

OpenDriveGeneratorServer::OpenDriveGeneratorServer(const rclcpp::NodeOptions &node_options) : Node("opendrive_generator_server", node_options) {
    RCLCPP_INFO(this->get_logger(), "OpenDrive Generator Server Started.");

    // Parameter declaration
    this->declare_parameter("map_content", "/workspaces/isaac_ros-dev/src/ros2-robotics-pkgs/carla_opendrive_parser/maps/CARISSMA_rural.xml");
    this->declare_parameter("distance", 1.0);
    this->declare_parameter("threshold", 0.1); 

    // Parameter initialization
    map_content_ = this->get_parameter("map_content").as_string();
    _distance = this->get_parameter("distance").as_double();
    _threshold = this->get_parameter("threshold").as_double();

    try {
        _map = std::make_shared<carla::road::Map>(MakeMap(map_content_));
    } catch (const std::exception &e) {
        RCLCPP_ERROR(this->get_logger(), "Failed to load OpenDRIVE map: %s", e.what());
    }

    // Publisher and Subscriber initialization
    origin_pose_ = this->create_publisher<geometry_msgs::msg::PoseStamped>("path_origin", 50);
    destination_pose_ = this->create_publisher<geometry_msgs::msg::PoseStamped>("path_destination", 50);
    marker_publisher_ = this->create_publisher<visualization_msgs::msg::Marker>("waypoints_marker", 50);
    
    // Service and Timer initialization
    service_ = this->create_service<GeneratePath>("generate_path", std::bind(&OpenDriveGeneratorServer::callback, this, std::placeholders::_1, std::placeholders::_2));
    toggle_service_ = this->create_service<std_srvs::srv::SetBool>("toggle_visualization", std::bind(&OpenDriveGeneratorServer::toggle_callback, this, std::placeholders::_1, std::placeholders::_2));

    timer_ = this->create_wall_timer(0.1ms, std::bind(&OpenDriveGeneratorServer::marker_visualization, this));
}

void OpenDriveGeneratorServer::toggle_callback(
    const std::shared_ptr<std_srvs::srv::SetBool::Request> request, 
    std::shared_ptr<std_srvs::srv::SetBool::Response> response) 
{
    _is_visualization_enabled = request->data;
    if (_is_visualization_enabled) {
        response->message = "Visualization Enabled";
    } else {
        response->message = "Visualization Disabled";
    }
    response->success = true;
}

void OpenDriveGeneratorServer::callback(const std::shared_ptr<GeneratePath::Request> request, std::shared_ptr<GeneratePath::Response> response) {
    _waypoints.clear();
    
    RCLCPP_INFO(this->get_logger(), "Incoming request\norigin: %f, %f, %f\ndestination: %f, %f, %f", request->origin.x, request->origin.y, request->origin.z, request->destination.x, request->destination.y, request->destination.z);

    const carla::geom::Location origin(request->origin.x, request->origin.y, request->origin.z);
    const carla::geom::Location destination(request->destination.x, request->destination.y, request->destination.z);

    const auto closets_wp_origin = _map->GetClosestWaypointOnRoad(origin);
    const auto closets_wp_destination = _map->GetClosestWaypointOnRoad(destination);

    if (!closets_wp_origin.has_value() || !closets_wp_destination.has_value()) {
        RCLCPP_ERROR(this->get_logger(), "Failed to find closest waypoints.");
        return;
    }

    carla::road::element::Waypoint start = *closets_wp_origin;
    const carla::road::element::Waypoint goal = *closets_wp_destination; // const since we don't want to modify the goal

    std::vector<carla::road::element::Waypoint> path;

    while (true) {
        path.push_back(start);

        // Stop condition: if same road & close enough (Threshold: 0.1)
        if (start.road_id == goal.road_id && std::abs(start.s - goal.s) < _threshold) {
            break;
        }

        // Get next waypoints at every @ distance
        std::vector<carla::road::element::Waypoint> next_waypoints = _map->GetNext(start, _distance);
        
        // Check if no further waypoints found
        if (next_waypoints.empty()) {
            RCLCPP_ERROR(this->get_logger(), "No further waypoints found!");
            break;
        }

        // Pick first option for simplicity (avoiding loops)
        start = next_waypoints.front();
    }

    // Ensure goal waypoint is included
    path.push_back(goal);

    response->path.resize(path.size());
    for (size_t i = 0; i < path.size(); i++) {

        carla::geom::Transform transform_wp_ = _map->ComputeTransform(path[i]);
        auto loc_start = transform_wp_.location;

        geometry_msgs::msg::Vector3 loc_waypoint_;
        loc_waypoint_.x = loc_start.x;
        loc_waypoint_.y = loc_start.y;
        loc_waypoint_.z = loc_start.z;
        _waypoints.push_back(loc_waypoint_);

        custom_interfaces::msg::Vector2 loc;
        loc.x = loc_start.x;
        loc.y = loc_start.y;

        response->path[i] = loc;
    }
}

void OpenDriveGeneratorServer::marker_visualization() {
    if (!_is_visualization_enabled) {
        return;
    }

    if (_waypoints.empty()) {
        return;
    }

    visualization_msgs::msg::Marker waypoint_marker;
    waypoint_marker.header.frame_id = "map";
    waypoint_marker.header.stamp = this->now();
    waypoint_marker.ns = "waypoints_marker";
    waypoint_marker.action = visualization_msgs::msg::Marker::ADD;
    waypoint_marker.pose.orientation.w = 1.0;
    waypoint_marker.id = 0;
    waypoint_marker.type = visualization_msgs::msg::Marker::LINE_STRIP;
    waypoint_marker.color.r = 0.0;
    waypoint_marker.color.g = 1.0;
    waypoint_marker.color.b = 0.0;
    waypoint_marker.color.a = 0.6;
    waypoint_marker.scale.x = 1.0;
    waypoint_marker.lifetime = rclcpp::Duration::from_seconds(0.1);

    for (const auto &wp : _waypoints) {
        geometry_msgs::msg::Point point;
        point.x = wp.x;
        point.y = wp.y;
        point.z = wp.z;

        waypoint_marker.points.push_back(point);
    }

    geometry_msgs::msg::PoseStamped origin_pose_msg;
    origin_pose_msg.header.stamp = this->now();
    origin_pose_msg.header.frame_id = "map";
    origin_pose_msg.pose.position.x = _waypoints.front().x;
    origin_pose_msg.pose.position.y = _waypoints.front().y;
    origin_pose_msg.pose.position.z = _waypoints.front().z;
    origin_pose_msg.pose.orientation.w = 1.0;

    geometry_msgs::msg::PoseStamped destination_pose_msg;
    destination_pose_msg.header.stamp = this->now();
    destination_pose_msg.header.frame_id = "map";
    destination_pose_msg.pose.position.x = _waypoints.back().x;
    destination_pose_msg.pose.position.y = _waypoints.back().y;
    destination_pose_msg.pose.position.z = _waypoints.back().z;
    destination_pose_msg.pose.orientation.w = 1.0;

    origin_pose_->publish(origin_pose_msg);
    destination_pose_->publish(destination_pose_msg);
    marker_publisher_->publish(waypoint_marker);
} 

OpenDriveGeneratorServer::~OpenDriveGeneratorServer() {
    RCLCPP_INFO(this->get_logger(), "OpenDrive Generator Server Terminated.");
}
} // namespace path_planner

#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(path_planner::OpenDriveGeneratorServer)