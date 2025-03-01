#ifndef ROS2_NITROS_CAMERA_H_
#define ROS2_NITROS_CAMERA_H_

#include <rclcpp/rclcpp.hpp>
#include <nav_msgs/msg/path.hpp>
#include <OpenDriveMap.h>

using namespace std::chrono_literals;
using namespace std::placeholders;

namespace path_planner
{

class OpenDriveGlobalPlanner : public rclcpp::Node
{
private:
    /* data */
    std::string map_path_;
    std::shared_ptr<odr::OpenDriveMap> open_drive_map_;

    rclcpp::Publisher<nav_msgs::msg::Path>::SharedPtr path_publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
public:
    OpenDriveGlobalPlanner(const rclcpp::NodeOptions&);
    ~OpenDriveGlobalPlanner();

    void path_publisher();
};

} // namespace path_planner

#endif  // ROS2_NITROS_CAMERA_H_