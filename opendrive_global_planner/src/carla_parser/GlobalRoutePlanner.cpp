#include "GlobalRoutePlanner.h"
#include <iostream>

GlobalRoutePlanner::GlobalRoutePlanner(boost::optional<carla::road::Map> map,
                                       const GraphAdjacency &graph)
  : _map(map), _graph(graph) {}

std::vector<std::pair<carla::road::element::Waypoint, int>>
GlobalRoutePlanner::trace_route(const carla::geom::Location &origin,
                                const carla::geom::Location &destination) {
  std::vector<std::pair<carla::road::element::Waypoint, int>> route;

  if (!_map) {
    std::cerr << "[GlobalRoutePlanner] No map loaded.\n";
    return route;
  }

  // 1) localize origin/destination in the map
  auto origin_wp_opt = _map->GetClosestWaypointOnRoad(origin);
  auto dest_wp_opt   = _map->GetClosestWaypointOnRoad(destination);

  if (!origin_wp_opt || !dest_wp_opt) {
    std::cerr << "[GlobalRoutePlanner] Invalid origin or destination.\n";
    return route;
  }

  // In a real scenario, you'd do BFS/A* using _graph to find a path from node to node.
  // We'll do a trivial route with just origin/destination for demonstration:
  route.push_back({*origin_wp_opt, 0});
  route.push_back({*dest_wp_opt,   0});

  return route;
}