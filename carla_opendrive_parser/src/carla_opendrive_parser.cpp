#include "carla_opendrive_parser/carla_opendrive_parser.h"

namespace path_planner
{

OpenDriveMapParser::OpenDriveMapParser(const rclcpp::NodeOptions &node_options) : Node("carla_opendrive_parser", node_options)
{
    RCLCPP_INFO(this->get_logger(), "OpenDrive Map Parser Node Started.");

    this->declare_parameter("map_content", "/workspaces/isaac_ros-dev/src/ros2-robotics-pkgs/carla_opendrive_parser/maps/CARISSMA_rural.xml");
    this->declare_parameter<std::vector<double>>("origin", {-171.71f, 122.36f, -0.64f});
    this->declare_parameter<std::vector<double>>("destination", {20.84f, 66.99f, -0.41f});
    
    std::string map_content = this->get_parameter("map_content").as_string();
    std::vector<double> origin_param = this->get_parameter("origin").as_double_array();
    std::vector<double> destination_param = this->get_parameter("destination").as_double_array();

    origin.x = origin_param[0];
    origin.y = origin_param[1];
    origin.z = origin_param[2];

    destination.x = destination_param[0];
    destination.y = destination_param[1];
    destination.z = destination_param[2];

    _map = std::make_shared<carla::road::Map>(MakeMap(map_content));

    marker_publisher_ = this->create_publisher<visualization_msgs::msg::Marker>("/waypoints_marker", 100);
    origin_pose_ = this->create_publisher<geometry_msgs::msg::PoseStamped>("origin", 100);
    destination_pose_ = this->create_publisher<geometry_msgs::msg::PoseStamped>("destination", 100);

    timer_ = this->create_wall_timer(
        0.1ms,
        std::bind(&OpenDriveMapParser::path_planner, this));
}

std::vector<carla::road::element::Waypoint> OpenDriveMapParser::AStarPathPlanning(const carla::road::element::Waypoint &start, const carla::road::element::Waypoint &goal) {
    std::vector<carla::road::element::Waypoint> path;
    std::priority_queue<AStarNode, std::vector<AStarNode>, std::greater<AStarNode>> open_set;
    std::unordered_map<std::string, double> g_score;
    std::unordered_map<std::string, carla::road::element::Waypoint> came_from;

    auto waypoint_hash = [](const carla::road::element::Waypoint &wp) {
        return std::to_string(wp.road_id) + "_" + std::to_string(wp.section_id) +
               "_" + std::to_string(wp.lane_id) + "_" + std::to_string(static_cast<int>(wp.s * 10));
    };

    open_set.push({start, Heuristic(start, goal), 0.0});
    g_score[waypoint_hash(start)] = 0.0;

    while (!open_set.empty()) {
        AStarNode current = open_set.top();
        open_set.pop();

        if (current.waypoint.road_id == goal.road_id && std::abs(current.waypoint.s - goal.s) < 1.0) {
            // Reconstruct path
            carla::road::element::Waypoint trace = current.waypoint;
            while (came_from.find(waypoint_hash(trace)) != came_from.end()) {
                path.push_back(trace);
                trace = came_from[waypoint_hash(trace)];
            }
            path.push_back(start);
            std::reverse(path.begin(), path.end());
            return path;
        }

        std::vector<carla::road::element::Waypoint> neighbors = _map->GetNext(current.waypoint, 2.0);

        for (const auto &neighbor : neighbors) {
            double tentative_g_score = current.g_cost + Heuristic(current.waypoint, neighbor);
            std::string neighbor_key = waypoint_hash(neighbor);

            if (g_score.find(neighbor_key) == g_score.end() || tentative_g_score < g_score[neighbor_key]) {
                g_score[neighbor_key] = tentative_g_score;
                came_from[neighbor_key] = current.waypoint;

                double f_score = tentative_g_score + Heuristic(neighbor, goal);
                open_set.push({neighbor, f_score, tentative_g_score});
            }
        }
    }

    std::cerr << "Error: No valid path found using A*!" << std::endl;
    return path;
}

void OpenDriveMapParser::path_planner() {
    std::vector<carla::road::element::Waypoint> path;

    // Find start and goal waypoints
    auto start_opt = _map->GetClosestWaypointOnRoad(origin, static_cast<int32_t>(carla::road::Lane::LaneType::Driving));
    auto goal_opt  = _map->GetClosestWaypointOnRoad(destination, static_cast<int32_t>(carla::road::Lane::LaneType::Driving));

    if (!start_opt || !goal_opt) {
        std::cerr << "Error: Could not find valid waypoints for origin or destination." << std::endl;
    }

    // path = AStarPathPlanning(*start_opt, *goal_opt);

    carla::road::element::Waypoint current = *start_opt;
    carla::road::element::Waypoint goal = *goal_opt;

    const auto goal_transform = _map->ComputeTransform(current);

    while (true) {
        path.push_back(current);

        // const auto current_transform = _map->ComputeTransform(current);

        // const double euclidean_distance = carla::geom::Math::Distance2D(current_transform.location, goal_transform.location);

        // Stop condition: if same road & close enough
        if (current.road_id == goal.road_id && std::abs(current.s - goal.s) < 1.0) {
            break;
        }

        std::vector<carla::road::element::Waypoint> next_waypoints = _map->GetNext(current, 0.1);

        if (next_waypoints.empty()) {
            std::cerr << "Error: No further waypoints found!" << std::endl;
            break;
        }

        // Pick first option for simplicity (avoiding loops)
        current = next_waypoints.front();
    }

    // Ensure goal waypoint is included
    path.push_back(goal);
    // Ensure the goal waypoint is included
    // RCLCPP_INFO(this->get_logger(), "Path found with %ld waypoints.", path.size());

    visualization_msgs::msg::Marker waypoints_marker = get_waypoints_marker(path, {0.0f, 1.0f, 0.0f}, 0.2, 0.5);

    // Puslish Origin Pose for Visualization
    geometry_msgs::msg::PoseStamped origin_pose_msg;
    origin_pose_msg.header.stamp = this->now();
    origin_pose_msg.header.frame_id = "map";
    carla::geom::Transform transform_opt_origin = _map->ComputeTransform(path.front());
    auto loc_start = transform_opt_origin.location;
    origin_pose_msg.pose.position.x = loc_start.x;
    origin_pose_msg.pose.position.y = loc_start.y;
    origin_pose_msg.pose.position.z = -loc_start.z;

    RCLCPP_INFO(this->get_logger(), "Start Location: %f, %f, %f", loc_start.x, loc_start.y, loc_start.z);
    RCLCPP_INFO(this->get_logger(), "Goal Location: %f, %f, %f", destination.x, destination.y, destination.z);

    origin_pose_msg.pose.orientation.x = 0.0;
    origin_pose_msg.pose.orientation.y = 0.0;
    origin_pose_msg.pose.orientation.z = 0.0;
    origin_pose_msg.pose.orientation.w = 1.0;

    // Publish Destination Pose for Visualization
    geometry_msgs::msg::PoseStamped destination_pose_msg;
    destination_pose_msg.header.stamp = this->now();
    destination_pose_msg.header.frame_id = "map";
    carla::geom::Transform transform_opt_destination = _map->ComputeTransform(path.back());
    auto loc_end = transform_opt_destination.location;
    destination_pose_msg.pose.position.x = loc_end.x;
    destination_pose_msg.pose.position.y = loc_end.y;
    destination_pose_msg.pose.position.z = -loc_end.z;

    destination_pose_msg.pose.orientation.x = 0.0;
    destination_pose_msg.pose.orientation.y = 0.0;
    destination_pose_msg.pose.orientation.z = 0.0;
    destination_pose_msg.pose.orientation.w = 1.0;

    origin_pose_->publish(origin_pose_msg);
    destination_pose_->publish(destination_pose_msg);
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