#include "carla_opendrive_parser/carla_opendrive_parser.h"

namespace path_planner
{

OpenDriveMapParser::OpenDriveMapParser(const rclcpp::NodeOptions &node_options) : Node("carla_opendrive_parser", node_options)
{
    RCLCPP_INFO(this->get_logger(), "OpenDrive Map Parser Node Started.");

    this->declare_parameter("map_content", "/workspaces/isaac_ros-dev/src/ros2-robotics-pkgs/carla_opendrive_parser/maps/CARISSMA_rural.xml");
    this->declare_parameter("origin", rclcpp::PARAMETER_DOUBLE_ARRAY);
    this->declare_parameter("destination", rclcpp::PARAMETER_DOUBLE_ARRAY); 
    map_content = this->get_parameter("map_content").get_parameter_value().get<std::string>();

    this->set_parameter(rclcpp::Parameter("origin", std::vector<double>{-171.71f, -122.36f, -0.64f}));
    this->set_parameter(rclcpp::Parameter("destination", std::vector<double>{5.91f, -3.52f, -0.70f}));

    std::vector<double> origin_param;
    this->get_parameter("origin", origin_param);
    std::vector<double> destination_param;
    this->get_parameter("destination", destination_param);

    origin.x = origin_param[0];
    origin.y = origin_param[1];
    origin.z = origin_param[2];

    destination.x = destination_param[0];
    destination.y = destination_param[1];
    destination.z = destination_param[2];

    _map = std::make_shared<carla::road::Map>(MakeMap(map_content));

    marker_publisher_ = this->create_publisher<visualization_msgs::msg::Marker>("/waypoints_marker", 50);

    timer_ = this->create_wall_timer(
        0.5ms,
        std::bind(&OpenDriveMapParser::path_planner, this));
}

void OpenDriveMapParser::path_planner() {
    std::vector<carla::road::element::Waypoint> path;

    boost::optional<carla::road::element::Waypoint> road_projected_waypoint_start = _map->GetClosestWaypointOnRoad(origin, static_cast<int32_t>(carla::road::Lane::LaneType::Driving));
    boost::optional<carla::road::element::Waypoint> road_projected_waypoint_end = _map->GetClosestWaypointOnRoad(destination, static_cast<int32_t>(carla::road::Lane::LaneType::Driving));

    if (!road_projected_waypoint_start) {
        std::cerr << "Error: Could not find a valid waypoint for the origin location!" << std::endl;
    }

    if (!road_projected_waypoint_end) {
        std::cerr << "Error: Could not find a valid waypoint for the destination location!" << std::endl;
    }

    carla::road::element::Waypoint start_waypoint = *road_projected_waypoint_start;
    carla::road::element::Waypoint goal_waypoint = *road_projected_waypoint_end;

    // Start path generation
    auto current_waypoint = start_waypoint;
    while (std::abs(current_waypoint.s - goal_waypoint.s) > 0.1) {
        path.push_back(current_waypoint);
        
        // Get next waypoints along the lane
        auto next_waypoints = _map->GetNext(current_waypoint, 1.0);  // Step size of 2 meters

        if (!next_waypoints.empty()) {
            current_waypoint = next_waypoints.front();  // Choose the first available waypoint
        } else {
            std::cerr << "Error: No further waypoints found!" << std::endl;
            break;
        }
    }

    // Ensure the goal waypoint is included
    path.push_back(goal_waypoint);

    visualization_msgs::msg::Marker waypoints_marker = get_waypoints_marker(path, {0.0f, 1.0f, 0.0f}, 0.2, 0.5);

    marker_publisher_->publish(waypoints_marker);
}

visualization_msgs::msg::Marker OpenDriveMapParser::get_waypoints_marker(
    const std::vector<carla::road::element::Waypoint> &waypoints,
    const std::array<float, 3> &color = {0.0f, 1.0f, 0.0f},
    double lifetime = 0.2,
    double scale = 0.1
) {
    visualization_msgs::msg::Marker waypoint_marker;
    waypoint_marker.header.frame_id = "map";
    waypoint_marker.header.stamp = this->now();
    waypoint_marker.ns = "waypoints_marker";
    waypoint_marker.action = visualization_msgs::msg::Marker::ADD;
    waypoint_marker.pose.orientation.w = 1.0;
    waypoint_marker.id = 0;
    waypoint_marker.type = visualization_msgs::msg::Marker::LINE_STRIP;
    waypoint_marker.color.r = color[0];
    waypoint_marker.color.g = color[1];
    waypoint_marker.color.b = color[2];
    waypoint_marker.color.a = 0.6;
    waypoint_marker.scale.x = scale;
    waypoint_marker.lifetime = rclcpp::Duration::from_seconds(lifetime);

    for (const auto &wp : waypoints) {
        carla::geom::Transform transform_opt = _map->ComputeTransform(wp);
        auto loc = transform_opt.location;

        geometry_msgs::msg::Point point;
        point.x = loc.x;
        point.y = loc.y;
        point.z = loc.z;

        waypoint_marker.points.push_back(point);
    }

    return waypoint_marker;
}

OpenDriveMapParser::~OpenDriveMapParser()
{
    RCLCPP_INFO(this->get_logger(), "OpenDrive Map Parser Terminated.");
}

}

#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(path_planner::OpenDriveMapParser)