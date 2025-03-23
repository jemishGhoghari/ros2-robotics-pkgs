// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include "opendrive/OpenDriveParser.h"

#include "Logging.h"
#include "opendrive/parser/ControllerParser.h"
#include "opendrive/parser/GeoReferenceParser.h"
#include "opendrive/parser/GeometryParser.h"
#include "opendrive/parser/JunctionParser.h"
#include "opendrive/parser/LaneParser.h"
#include "opendrive/parser/ObjectParser.h"
#include "opendrive/parser/ProfilesParser.h"
#include "opendrive/parser/RoadParser.h"
#include "opendrive/parser/SignalParser.h"
#include "opendrive/parser/TrafficGroupParser.h"
#include "road/MapBuilder.h"

#include <pugixml/pugixml.hpp>

namespace carla {
namespace opendrive {

  boost::optional<road::Map> OpenDriveParser::Load(const std::string &opendrive) {
    pugi::xml_document xml;
    pugi::xml_parse_result parse_result = xml.load_file(opendrive.c_str());

    if (parse_result == false) {
      log_error("unable to parse the OpenDRIVE XML string");
      return {};
    }

    carla::road::MapBuilder map_builder;

    parser::GeoReferenceParser::Parse(xml, map_builder);
    parser::RoadParser::Parse(xml, map_builder);
    parser::JunctionParser::Parse(xml, map_builder);
    parser::GeometryParser::Parse(xml, map_builder);
    parser::LaneParser::Parse(xml, map_builder);
    parser::ProfilesParser::Parse(xml, map_builder);
    parser::TrafficGroupParser::Parse(xml, map_builder);
    parser::SignalParser::Parse(xml, map_builder);
    parser::ObjectParser::Parse(xml, map_builder);
    parser::ControllerParser::Parse(xml, map_builder);

    return map_builder.Build();
  }

} // namespace opendrive
} // namespace carla
