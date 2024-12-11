# Description: Launch file for two cameras and rviz2
from launch import LaunchDescription
from launch_ros.actions import *
from launch_ros.descriptions import ComposableNode
from launch_ros.actions import ComposableNodeContainer

def generate_launch_description():
    """Generate launch description with multiple components."""

    nitros_camera_node = ComposableNode(
        package='ros2_nitros_camera',
        plugin='ros2_nitros_camera::NitrosCameraNode',
        namespace='',
        name='ros2_nitros_camera_node',
        parameters=[
            {"camera_id" : 0},
            {"camera_fps" : 60},
            {"image_width" : 640},
            {"image_height" : 640},	
            {"frame_id" : "camera"},
            {"from_video_file" : False},
            {"video_file_path" : "src/ros2-robotics-pkgs/ros2_nitros_camera/sample_video/Test_Video.mp4"},
            {"camera_calibration_file" : "src/ros2-robotics-pkgs/ros2_nitros_camera/calibration_config/camera_calibration.yaml"}
        ]
    )

    container = ComposableNodeContainer(
        name='tensor_rt_container',
        namespace='',
        package='rclcpp_components',
        executable='component_container_mt',
        composable_node_descriptions=[nitros_camera_node],
        arguments=['--ros-args', '--log-level', 'INFO'],
        output='screen'
    )

    return LaunchDescription([
        container
    ])
