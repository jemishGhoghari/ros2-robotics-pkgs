#ifndef CARLA_OPENDRIVE_PARSER_H_
#define CARLA_OPENDRIVE_PARSER_H_

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
#include <nav_msgs/msg/path.hpp>
#include <visualization_msgs/msg/marker.hpp>
#include <geometry_msgs/msg/point.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>

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

// Struct for priority queue in A*
struct AStarNode {
    carla::road::element::Waypoint waypoint;
    double cost;  // g + h cost
    double g_cost;
    
    bool operator>(const AStarNode &other) const {
        return cost > other.cost;
    }
};

// // Comparator for priority queue (min-heap)
// struct Compare {
//     bool operator()(const PathNode &a, const PathNode &b) {
//         return a.estimated_cost > b.estimated_cost;  // Lower cost has higher priority
//     }
// };

class OpenDriveMapParser : public rclcpp::Node
{   
private:
    std::shared_ptr<carla::road::Map> _map;
    carla::geom::Location origin;
    carla::geom::Location destination;
    double sampling_resolution = 1.0;

    // rclcpp::Publisher<nav_msgs::msg::Path>::SharedPtr path_publisher_;
    rclcpp::Publisher<visualization_msgs::msg::Marker>::SharedPtr marker_publisher_;
    rclcpp::Publisher<geometry_msgs::msg::PoseStamped>::SharedPtr origin_pose_;
    rclcpp::Publisher<geometry_msgs::msg::PoseStamped>::SharedPtr destination_pose_;
    rclcpp::TimerBase::SharedPtr timer_;

public:
    OpenDriveMapParser(const rclcpp::NodeOptions&);
    ~OpenDriveMapParser();

    void path_planner();
    std::vector<carla::road::element::Waypoint> AStarPathPlanning(const carla::road::element::Waypoint &start, const carla::road::element::Waypoint &goal);
    
    visualization_msgs::msg::Marker get_waypoints_marker(
        const std::vector<carla::road::element::Waypoint> &waypoints,
        const std::array<float, 3> &color,
        double lifetime,
        double scale
    );

    // Heuristic function: Euclidean distance
    double Heuristic(const carla::road::element::Waypoint &a, const carla::road::element::Waypoint &b) {
        carla::geom::Transform ta = _map->ComputeTransform(a);
        carla::geom::Transform tb = _map->ComputeTransform(b);
        return std::sqrt(std::pow(ta.location.x - tb.location.x, 2) + 
                        std::pow(ta.location.y - tb.location.y, 2));
    }
};

}

#endif CARLA_OPENDRIVE_PARSER_H_