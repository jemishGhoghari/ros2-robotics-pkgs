#ifndef CARLA_OPENDRIVE_PARSER_H_
#define CARLA_OPENDRIVE_PARSER_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "road/Map.h"
#include "opendrive/OpenDriveParser.h"
#include "road/element/Waypoint.h"
#include "geom/Location.h"
#include "geom/Transform.h"

#include <rclcpp/rclcpp.hpp>
#include <nav_msgs/msg/path.hpp>
#include <visualization_msgs/msg/marker.hpp>
#include <geometry_msgs/msg/point.hpp>

using namespace std::chrono_literals;
using namespace std::placeholders;

namespace path_planner
{

static auto MakeMap(const std::string &opendrive_content) {
    auto stream = std::istringstream(opendrive_content);
    auto map = carla::opendrive::OpenDriveParser::Load(stream.str());
    if (!map) {
        carla::throw_exception(std::runtime_error("Failed to load OpenDRIVE map"));
    }
    return std::move(*map);
}

class OpenDriveMapParser : public rclcpp::Node
{   
private:
    std::string map_content; // path to OpenDRIVE XML file
    std::shared_ptr<carla::road::Map> _map;
    carla::geom::Location origin;
    carla::geom::Location destination;

    // rclcpp::Publisher<nav_msgs::msg::Path>::SharedPtr path_publisher_;
    rclcpp::Publisher<visualization_msgs::msg::Marker>::SharedPtr marker_publisher_;
    rclcpp::TimerBase::SharedPtr timer_;

public:
    OpenDriveMapParser(const rclcpp::NodeOptions&);
    ~OpenDriveMapParser();

    void path_planner();
    visualization_msgs::msg::Marker get_waypoints_marker(
        const std::vector<carla::road::element::Waypoint> &waypoints,
        const std::array<float, 3> &color,
        double lifetime,
        double scale
    );
};

}

#endif CARLA_OPENDRIVE_PARSER_H_