cmake_minimum_required(VERSION 3.8)
project(opendrive_global_planner)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(custom_interfaces REQUIRED)
find_package(std_srvs REQUIRED)
find_package(Boost REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(
  include
  carla_parser
)

set(SOURCES
  src/carla_parser/geom/GeoLocation.cpp
  src/carla_parser/geom/Math.cpp
  src/carla_parser/geom/Mesh.cpp
  src/carla_parser/geom/Simplification.cpp
  src/carla_parser/opendrive/OpenDriveParser.cpp
  src/carla_parser/opendrive/parser/ControllerParser.cpp
  src/carla_parser/opendrive/parser/GeometryParser.cpp
  src/carla_parser/opendrive/parser/GeoReferenceParser.cpp
  src/carla_parser/opendrive/parser/JunctionParser.cpp
  src/carla_parser/opendrive/parser/LaneParser.cpp
  src/carla_parser/opendrive/parser/ObjectParser.cpp
  src/carla_parser/opendrive/parser/ProfilesParser.cpp
  src/carla_parser/opendrive/parser/RoadParser.cpp
  src/carla_parser/opendrive/parser/SignalParser.cpp
  src/carla_parser/opendrive/parser/TrafficGroupParser.cpp
  src/carla_parser/road/Lane.cpp
  src/carla_parser/road/LaneSection.cpp
  src/carla_parser/road/Map.cpp
  src/carla_parser/road/MapBuilder.cpp
  src/carla_parser/road/MapData.cpp
  src/carla_parser/road/MeshFactory.cpp
  src/carla_parser/road/Road.cpp
  src/carla_parser/road/SignalType.cpp
  src/carla_parser/road/element/Geometry.cpp
  src/carla_parser/road/element/LaneCrossingCalculator.cpp
  src/carla_parser/road/element/LaneMarking.cpp
  src/carla_parser/road/element/Waypoint.cpp
  src/carla_parser/Exception.cpp
  src/carla_parser/StringUtil.cpp
  src/carla_parser/pugixml/pugixml.cpp
  src/carla_parser/odrSpiral/odrSpiral.cpp
)

add_library(OpenDRIVE ${SOURCES})
target_include_directories(
  OpenDRIVE PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${pugixml_SOURCE_DIR}/src>
)

set_target_properties(OpenDRIVE PROPERTIES POSITION_INDEPENDENT_CODE ON)

install(
  TARGETS OpenDRIVE
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
)

# OpenDRIVE Path Generator Server
add_library(${PROJECT_NAME} SHARED
  src/opendrive_generator_server.cpp
)

ament_target_dependencies(${PROJECT_NAME}
  "rclcpp"
  "rclcpp_components"
  "nav_msgs"
  "geometry_msgs"
  "visualization_msgs"
  "custom_interfaces"
  "std_srvs"
)

target_link_libraries(${PROJECT_NAME} OpenDRIVE)

rclcpp_components_register_nodes(${PROJECT_NAME} "path_planner::OpenDriveGeneratorServer")
set(node_plugins "${node_plugins}path_planner::OpenDriveGeneratorServer;$<TARGET_FILE:path_planner>\n")
set_target_properties(${PROJECT_NAME} PROPERTIES
  BUILD_WITH_INSTALL_RPATH TRUE
  BUILD_RPATH_USE_ORIGIN TRUE
  INSTALL_RPATH_USE_LINK_PATH TRUE)

target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

install(TARGETS
  ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(DIRECTORY
  launch
  maps
  DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
