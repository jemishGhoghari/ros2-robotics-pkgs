#include <opendrive_global_planner/opendrive_global_planner.h>

namespace path_planner
{

OpenDriveGlobalPlanner::OpenDriveGlobalPlanner(const rclcpp::NodeOptions &node_options) : Node("opendrive_global_planner", node_options)
{
    RCLCPP_INFO(this->get_logger(), "OpenDrive Global Planner Node Started.");

    this->declare_parameter("map_path", "src/ros2-robotics-pkgs/opendrive_global_planner/maps/CARISSMA_rural.xodr");
    map_path_ = this->get_parameter("map_path").get_parameter_value().get<std::string>();

    open_drive_map_ = std::make_shared<odr::OpenDriveMap>(map_path_);

    path_publisher_ = this->create_publisher<nav_msgs::msg::Path>("/path", 10);

    timer_ = this->create_wall_timer(
        0.5ms,
        std::bind(&OpenDriveGlobalPlanner::path_publisher, this));
}

OpenDriveGlobalPlanner::~OpenDriveGlobalPlanner()
{
    RCLCPP_INFO(this->get_logger(), "OpenDrive Global Planner Terminated.");
}

void OpenDriveGlobalPlanner::path_publisher()
{
    nav_msgs::msg::Path path_msg;
    path_msg.header.stamp = this->now();
    path_msg.header.frame_id = "map";
    auto roads = open_drive_map_->get_roads();

    auto graph = open_drive_map_->get_routing_graph();
    auto path_short_path = graph.shortest_path(odr::LaneKey("5", 0.0, -1), odr::LaneKey("14", 0.0, -1));

    for (const auto& path : path_short_path) {

        auto road = open_drive_map_->get_road(path.road_id);
        auto ref_line = road.ref_line.approximate_linear(0.1, path.lanesection_s0, road.length);

        for (const auto& s : ref_line)
        {   
            auto pose_xyz = road.ref_line.get_xyz(s);
            geometry_msgs::msg::PoseStamped pose;
            pose.pose.position.x = pose_xyz[0];
            pose.pose.position.y = pose_xyz[1];
            pose.pose.position.z = pose_xyz[2];	
            path_msg.poses.push_back(pose);
        }
    }

    path_publisher_->publish(path_msg);
}

} // namespace path_planner

#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(path_planner::OpenDriveGlobalPlanner)