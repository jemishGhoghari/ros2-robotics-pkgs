#ifndef GRAPHBUILDER_H
#define GRAPHBUILDER_H

#include "TopologyBuilder.h" // For TopologySegment
#include <unordered_map>
#include <tuple>
#include <vector>
#include <iostream>

/// Edge attributes, similar to Python's 'edge' dictionary
struct EdgeAttributes {
  double length = 0.0;
  std::vector<carla::road::element::Waypoint> path;
  carla::road::element::Waypoint entry_waypoint;
  carla::road::element::Waypoint exit_waypoint;
  bool intersection = false; // optional
  int type = 0;             // e.g., LANEFOLLOW
};

/// Simple adjacency type: node_id -> node_id -> EdgeAttributes
using GraphAdjacency = std::unordered_map<int, std::unordered_map<int, EdgeAttributes>>;

/// GraphBuilder builds a graph from a list of TopologySegments
class GraphBuilder {
public:
  GraphBuilder(const std::vector<TopologySegment> &segments);

  /// Build the adjacency list
  void BuildGraph();

  /// Access the final adjacency
  const GraphAdjacency &GetGraph() const { return _graph; }

  /// Access the node index map: (x,y,z) -> node_id
  const std::unordered_map<std::tuple<float,float,float>, int> &GetNodeMap() const { return _id_map; }

  /// Access the road->section->lane -> (n1,n2) map
  const std::unordered_map<int,
        std::unordered_map<int,
        std::unordered_map<int, std::pair<int,int>>>> &GetRoadIdToEdge() const {
    return _road_id_to_edge;
  }

private:
  const std::vector<TopologySegment> &_segments;
  GraphAdjacency _graph;

  // (x,y,z) -> node_id
  std::unordered_map<std::tuple<float,float,float>, int> _id_map;

  // road_id->section_id->lane_id -> (n1,n2)
  std::unordered_map<int,
    std::unordered_map<int,
      std::unordered_map<int, std::pair<int,int>>>> _road_id_to_edge;
};

#endif // GRAPHBUILDER_H