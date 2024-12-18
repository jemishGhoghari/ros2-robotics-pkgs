cmake_minimum_required(VERSION 3.8)
project(image_to_nitros)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(isaac_ros_managed_nitros REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(isaac_ros_nitros_image_type REQUIRED)
find_package(isaac_ros_nitros_camera_info_type REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(message_filters REQUIRED)
find_package(cv_bridge REQUIRED)


# GPU Image Builder node
add_library(${PROJECT_NAME} SHARED
  src/image_to_nitros.cpp
)

ament_target_dependencies(${PROJECT_NAME}
  "rclcpp"
  "rclcpp_components"
  "isaac_ros_managed_nitros"
  "isaac_ros_nitros_image_type"
  "isaac_ros_nitros_camera_info_type"
  "sensor_msgs"
  "message_filters"
  "cv_bridge"
)

rclcpp_components_register_nodes(${PROJECT_NAME} "image_to_nitros::ImageToNitros")
set(node_plugins "${node_plugins}image_to_nitros::ImageToNitros;$<TARGET_FILE:image_to_nitros>\n")
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
