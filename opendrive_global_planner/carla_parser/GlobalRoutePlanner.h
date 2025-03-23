#ifndef GLOBALROUTEPLANNER_H
#define GLOBALROUTEPLANNER_H

#include "GraphBuilder.h"
#include <geom/Location.h>
#include <vector>
#include <utility>

/// Minimal GlobalRoutePlanner that uses the graph to plan a route
class GlobalRoutePlanner {
public:
  GlobalRoutePlanner(std::shared_ptr<carla::road::Map> map,
                     const GraphAdjacency &graph);

  /// A simplified route function
  std::vector<std::pair<carla::road::element::Waypoint, int>>
    trace_route(const carla::geom::Location &origin,
                const carla::geom::Location &destination);

private:
  std::shared_ptr<carla::road::Map> _map;
  const GraphAdjacency &_graph; // reference to the adjacency from GraphBuilder
};

#endif // GLOBALROUTEPLANNER_H