#include "GraphBuilder.h"

GraphBuilder::GraphBuilder(const std::vector<TopologySegment> &segments)
  : _segments(segments) {}

void GraphBuilder::BuildGraph() {
  _graph.clear();
  _id_map.clear();
  _road_id_to_edge.clear();

  int node_counter = 0;

  if (_segments.empty()) {
    std::cerr << "[GraphBuilder] No segments to build graph.\n";
    return;
  }

  // For each segment, assign node IDs for entry/exit and add adjacency
  for (const auto &seg : _segments) {
    auto entry_xyz = seg.entryxyz; // (x1,y1,z1)
    auto exit_xyz  = seg.exitxyz;  // (x2,y2,z2)

    // Possibly check intersection = ...
    bool intersection = false; // e.g. if we have a junction check

    int road_id    = seg.entry.road_id;
    int section_id = seg.entry.section_id;
    int lane_id    = seg.entry.lane_id;

    // 1) Assign node for entry_xyz
    if (_id_map.find(entry_xyz) == _id_map.end()) {
      _id_map[entry_xyz] = node_counter++;
    }
    int n1 = _id_map[entry_xyz];

    // 2) Assign node for exit_xyz
    if (_id_map.find(exit_xyz) == _id_map.end()) {
      _id_map[exit_xyz] = node_counter++;
    }
    int n2 = _id_map[exit_xyz];

    // 3) Update _road_id_to_edge
    _road_id_to_edge[road_id][section_id][lane_id] = std::make_pair(n1, n2);

    // 4) Build EdgeAttributes
    EdgeAttributes edge;
    edge.length         = seg.path.size() + 1.0; // similar to Python
    edge.path           = seg.path;
    edge.entry_waypoint = seg.entry;
    edge.exit_waypoint  = seg.exit;
    edge.intersection   = intersection;
    edge.type           = 0; // e.g. LANEFOLLOW

    // 5) Insert into adjacency
    _graph[n1][n2] = edge;
  }

  std::cout << "[GraphBuilder] Graph built with " << _graph.size() << " nodes.\n";
}