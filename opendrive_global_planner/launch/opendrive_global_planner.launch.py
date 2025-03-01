# Description: Launch file for two cameras and rviz2
from launch import LaunchDescription
from launch_ros.actions import *
from launch_ros.descriptions import ComposableNode
from launch_ros.actions import ComposableNodeContainer

def generate_launch_description():
    """Generate launch description with multiple components."""

    opendrive_globa_planner_node = ComposableNode(
        package='opendrive_global_planner',
        plugin='path_planner::OpenDriveGlobalPlanner',
        namespace='',
        name='opendrive_globa_planner_node'
    )

    container = ComposableNodeContainer(
        name='path_planner_container',
        namespace='',
        package='rclcpp_components',
        executable='component_container_mt',
        composable_node_descriptions=[opendrive_globa_planner_node],
        arguments=['--ros-args', '--log-level', 'INFO'],
        output='screen'
    )

    return LaunchDescription([
        container
    ])