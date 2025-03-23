#include "TopologyBuilder.h"
#include <iostream> // for debug prints

TopologyBuilder::TopologyBuilder(std::shared_ptr<carla::road::Map> map, double sampling_resolution)
  : _map(std::move(map)), _sampling_resolution(sampling_resolution) {}

boost::optional<std::vector<TopologySegment>> TopologyBuilder::BuildTopology() {
  if (!_map) {
    std::cerr << "[TopologyBuilder] Invalid map pointer.\n";
    return boost::none;
  }

  // 1) Retrieve raw topology from the OpenDRIVE map
  //    This returns a list of (entry, exit) waypoint pairs
  auto raw_topology = _map->GenerateTopology();
  if (raw_topology.empty()) {
    std::cerr << "[TopologyBuilder] Map returned an empty topology.\n";
    return boost::none;
  }

  std::vector<TopologySegment> segments;
  segments.reserve(raw_topology.size());

  // 2) Build each segment
  for (const auto &pair : raw_topology) {
    const auto &entry_wp = pair.first;
    const auto &exit_wp  = pair.second;

    auto entry_tf = _map->ComputeTransform(entry_wp);
    auto exit_tf  = _map->ComputeTransform(exit_wp);
    auto loc1 = entry_tf.location;
    auto loc2 = exit_tf.location;

    // Round coordinates (like Python np.round)
    float x1 = std::round(loc1.x);
    float y1 = std::round(loc1.y);
    float z1 = std::round(loc1.z);

    float x2 = std::round(loc2.x);
    float y2 = std::round(loc2.y);
    float z2 = std::round(loc2.z);

    TopologySegment seg;
    seg.entry     = entry_wp;
    seg.exit      = exit_wp;
    seg.entryxyz  = std::make_tuple(x1, y1, z1);
    seg.exitxyz   = std::make_tuple(x2, y2, z2);

    // Build intermediate path at resolution
    double distance = loc1.Distance(loc2);
    if (distance > _sampling_resolution) {
      auto current_wp = entry_wp;
      while (true) {
        auto next_list = _map->GetNext(current_wp, _sampling_resolution);
        if (next_list.empty()) {
          break;
        }
        auto next_wp = next_list.front();
        seg.path.push_back(next_wp);

        auto next_loc = _map->ComputeTransform(next_wp).location;
        if (next_loc.Distance(loc2) <= _sampling_resolution) {
          break;
        }
        current_wp = next_wp;
      }
    } else {
      // If distance <= resolution, just add one step
      auto next_list = _map->GetNext(entry_wp, _sampling_resolution);
      if (!next_list.empty()) {
        seg.path.push_back(next_list.front());
      }
    }

    segments.push_back(std::move(seg));
  }

  return segments;
}