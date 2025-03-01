cmake_minimum_required(VERSION 3.8)
project(opendrive_global_planner)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

include(FetchContent)

FetchContent_Declare(
    pugixml
    GIT_REPOSITORY https://github.com/zeux/pugixml.git
    GIT_TAG v1.14
)
FetchContent_MakeAvailable(pugixml)

set(SOURCES
    libOpenDRIVE/src/Geometries/Arc.cpp
    libOpenDRIVE/src/Geometries/CubicSpline.cpp
    libOpenDRIVE/src/Geometries/Line.cpp
    libOpenDRIVE/src/Geometries/ParamPoly3.cpp
    libOpenDRIVE/src/Geometries/RoadGeometry.cpp
    libOpenDRIVE/src/Geometries/Spiral.cpp
    libOpenDRIVE/src/Geometries/Spiral/odrSpiral.cpp
    libOpenDRIVE/src/Junction.cpp
    libOpenDRIVE/src/Lane.cpp
    libOpenDRIVE/src/LaneSection.cpp
    libOpenDRIVE/src/Log.cpp
    libOpenDRIVE/src/Mesh.cpp
    libOpenDRIVE/src/OpenDriveMap.cpp
    libOpenDRIVE/src/RefLine.cpp
    libOpenDRIVE/src/Road.cpp
    libOpenDRIVE/src/RoadMark.cpp
    libOpenDRIVE/src/RoadNetworkMesh.cpp
    libOpenDRIVE/src/RoadObject.cpp
    libOpenDRIVE/src/RoadSignal.cpp
    libOpenDRIVE/src/RoutingGraph.cpp
    ${pugixml_SOURCE_DIR}/src/pugixml.cpp
)

include_directories(
  include
  libOpenDRIVE/include
)

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(Boost REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

add_library(OpenDrive ${SOURCES})
target_include_directories(
    OpenDrive PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${pugixml_SOURCE_DIR}/src>
)

set_target_properties(OpenDrive PROPERTIES POSITION_INDEPENDENT_CODE ON)

install(
  TARGETS OpenDrive
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
)

add_library(${PROJECT_NAME} SHARED
  src/opendrive_global_planner.cpp
)

ament_target_dependencies(${PROJECT_NAME}
  "rclcpp"
  "rclcpp_components"
  "nav_msgs"
  "geometry_msgs"
)

target_link_libraries(${PROJECT_NAME} OpenDrive pugixml)

rclcpp_components_register_nodes(${PROJECT_NAME} "path_planner::OpenDriveGlobalPlanner")
set(node_plugins "${node_plugins}path_planner::OpenDriveGlobalPlanner;$<TARGET_FILE:path_planner>\n")
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
