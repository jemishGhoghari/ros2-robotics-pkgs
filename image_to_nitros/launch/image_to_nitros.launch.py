# Description: Launch file for two cameras and rviz2
from launch import LaunchDescription
from launch_ros.actions import *
from launch_ros.descriptions import ComposableNode
from launch_ros.actions import ComposableNodeContainer

def generate_launch_description():
    """Generate launch description with multiple components."""

    image_to_nitros_node = ComposableNode(
        package='image_to_nitros',
        plugin='image_to_nitros::ImageToNitros',
        namespace='',
        name='image_to_nitros_node',
        remappings=[
            ('/image', '/carla/ego_vehicle/rgb_front/image'),
            ('/camera_info', '/carla/ego_vehicle/rgb_front/camera_info'),
            ('/nitros_gpu_image', '/image'),
            ('/nitros_camera_info', '/camera_info'),
        ]
    )

    container = ComposableNodeContainer(
        name='container',
        namespace='',
        package='rclcpp_components',
        executable='component_container_mt',
        composable_node_descriptions=[image_to_nitros_node],
        arguments=['--ros-args', '--log-level', 'INFO'],
        output='screen'
    )

    return LaunchDescription([
        container
    ])
