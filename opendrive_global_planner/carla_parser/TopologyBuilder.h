#ifndef TOPOLOGYBUILDER_H
#define TOPOLOGYBUILDER_H

#include <road/Map.h>
#include <road/element/Waypoint.h>
#include <boost/optional.hpp>
#include <vector>
#include <tuple>
#include <memory>
#include <cmath> // for std::round

/// Holds data about a single segment from entry to exit
struct TopologySegment {
  carla::road::element::Waypoint entry;
  carla::road::element::Waypoint exit;

  // Rounded coordinates of entry/exit
  std::tuple<float, float, float> entryxyz;
  std::tuple<float, float, float> exitxyz;

  // Intermediate waypoints between entry & exit
  std::vector<carla::road::element::Waypoint> path;
};

/// Class that builds a list of TopologySegments from an OpenDRIVE Map
class TopologyBuilder {
public:
  /// Construct with a shared pointer to the OpenDRIVE Map and a sampling resolution
  TopologyBuilder(std::shared_ptr<carla::road::Map> map, double sampling_resolution);

  /// Build the detailed topology
  /// Returns a boost::optional containing a vector of segments, or boost::none on failure
  boost::optional<std::vector<TopologySegment>> BuildTopology();

private:
  std::shared_ptr<carla::road::Map> _map;
  double _sampling_resolution;
};

#endif // TOPOLOGYBUILDER_H