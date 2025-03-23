from launch import LaunchDescription
from launch_ros.actions import *
from launch_ros.descriptions import ComposableNode
from launch_ros.actions import ComposableNodeContainer
from launch.substitutions import PathJoinSubstitution
from launch_ros.substitutions import FindPackageShare

def generate_launch_description():
    """Generate launch description with multiple components."""
    opendrive_map_content = PathJoinSubstitution([FindPackageShare('opendrive_global_planner'), 'maps', 'CARISSMA_rural.xml'])

    opendrive_global_planner_node = ComposableNode(
        package='opendrive_global_planner',
        plugin='path_planner::OpenDriveGeneratorServer',
        namespace='',
        name='opendrive_path_generator_server',
        parameters=[
            {"map_content" : opendrive_map_content},
            {"distance" : 1.0},
            {"threshold" : 1.0},
        ]
    )

    container = ComposableNodeContainer(
        name='path_planner_container',
        namespace='',
        package='rclcpp_components',
        executable='component_container_mt',
        composable_node_descriptions=[opendrive_global_planner_node],
        arguments=['--ros-args', '--log-level', 'INFO'],
        output='screen'
    )

    return LaunchDescription([
        container
    ])