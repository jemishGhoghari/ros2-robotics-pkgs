#ifndef OPENDRIVE_GENERATOR_SERVER_H
#define OPENDRIVE_GENERATOR_SERVER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <cmath>
#include <memory>

#include "road/Map.h"
#include "opendrive/OpenDriveParser.h"
#include "road/element/Waypoint.h"
#include "geom/Location.h"
#include "geom/Transform.h"
#include "geom/Vector3D.h"
#include "geom/Math.h"

#include <rclcpp/rclcpp.hpp>
#include <custom_interfaces/msg/location.hpp>
#include <custom_interfaces/srv/generate_path.hpp>
#include <geometry_msgs/msg/vector3.hpp>
#include <visualization_msgs/msg/marker.hpp>
#include <std_srvs/srv/set_bool.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>

using namespace std::chrono_literals;
using namespace std::placeholders;

namespace path_planner {

using GeneratePath = custom_interfaces::srv::GeneratePath;
using Location = custom_interfaces::msg::Location;    

static auto MakeMap(const std::string &opendrive_content) {
    auto stream = std::istringstream(opendrive_content);
    auto map = carla::opendrive::OpenDriveParser::Load(stream.str());
    if (!map) {
        carla::throw_exception(std::runtime_error("Failed to load OpenDRIVE map"));
    }

    return std::move(*map);
}

class OpenDriveGeneratorServer : public rclcpp::Node
{
private:
    std::shared_ptr<carla::road::Map> _map;
    std::string map_content_;
    double _distance = 1.0;
    double _threshold = 0.1;
    bool _is_visualization_enabled = false;
    std::vector<geometry_msgs::msg::Vector3> _waypoints;

    void callback(const std::shared_ptr<GeneratePath::Request> request, std::shared_ptr<GeneratePath::Response> response);
    void toggle_callback(const std::shared_ptr<std_srvs::srv::SetBool::Request> request, std::shared_ptr<std_srvs::srv::SetBool::Response> response);

    void marker_visualization();

    visualization_msgs::msg::Marker get_waypoints_marker(
        const std::vector<carla::road::element::Waypoint> &waypoints,
        const std::array<float, 3> &color,
        double lifetime,
        double scale
    );

    rclcpp::Publisher<geometry_msgs::msg::PoseStamped>::SharedPtr origin_pose_;
    rclcpp::Publisher<geometry_msgs::msg::PoseStamped>::SharedPtr destination_pose_;
    rclcpp::Publisher<visualization_msgs::msg::Marker>::SharedPtr marker_publisher_;
    rclcpp::Service<std_srvs::srv::SetBool>::SharedPtr toggle_service_;
    rclcpp::Service<GeneratePath>::SharedPtr service_;
    rclcpp::TimerBase::SharedPtr timer_;
public:
    OpenDriveGeneratorServer(const rclcpp::NodeOptions &node_options);
    ~OpenDriveGeneratorServer();
};

}  // namespace path_planner
#endif  // OPENDRIVE_GENERATOR_SERVER_H
