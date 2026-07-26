/**
 * @file ros2_nitros_camera.h
 * @brief Header file for the ROS2 Nitros Camera Grabber Node.
 *
 * This file contains the declarations for the ROS2 Nitros Camera Grabber Node,
 * which is responsible for interfacing with camera hardware, capturing images,
 * and publishing them to ROS2 topics for further processing and analysis.
 *
 * @project ros2_nitros_camera
 * @date 14.01.2024
 * @author Jemish Ghoghari <mailto:jemishghoghari50@gmail.com>
 */

#ifndef ROS2_NITROS_CAMERA_H_
#define ROS2_NITROS_CAMERA_H_

#include <chrono>
#include <cstdio>
#include <memory>
#include <string>

#include <rclcpp/rclcpp.hpp>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <isaac_ros_managed_nitros/managed_nitros_publisher.hpp>
#include <isaac_ros_nitros_image_type/nitros_image.hpp>
#include <isaac_ros_nitros_image_type/nitros_image_builder.hpp>
#include <isaac_ros_nitros_camera_info_type/nitros_camera_info.hpp>
#include <isaac_ros_common/qos.hpp>

#include <camera_calibration_parsers/parse.hpp>
#include <sensor_msgs/msg/camera_info.hpp>

#include <cuda_runtime.h>

using namespace std::chrono_literals;
using namespace std::placeholders;

namespace ros2_nitros_camera
{
namespace {
    constexpr const char kDefaultQoS[] = "DEFAULT";
}

class NitrosCameraNode : public rclcpp::Node
{
private:
    /* data */
    int camera_id;
    int camera_fps;
    int image_width_;
    int image_height_;
    std::string frame_id_;
    bool from_video_file_;
    std::string video_file_path_;
    std::string camera_name_;
    std::string camera_calibration_file_;

    cv::Mat frame;
    cv::VideoCapture cap;
    std::shared_ptr<nvidia::isaac_ros::nitros::ManagedNitrosPublisher<nvidia::isaac_ros::nitros::NitrosImage>> camera_pub_;
    // rclcpp::Publisher<sensor_msgs::msg::CameraInfo>::SharedPtr camera_info_publisher_;
    std::shared_ptr<nvidia::isaac_ros::nitros::ManagedNitrosPublisher<nvidia::isaac_ros::nitros::NitrosCameraInfo>> camera_info_publisher_;

    rclcpp::TimerBase::SharedPtr timer_;
    std::chrono::steady_clock::time_point last_frame_;
    sensor_msgs::msg::CameraInfo camera_info_msg_;

    const rclcpp::QoS output_qos = ::isaac_ros::common::AddQosParameter(
    *this, kDefaultQoS, "output_qos").keep_last(10);

public:
    explicit NitrosCameraNode(const rclcpp::NodeOptions&);
    ~NitrosCameraNode();

    void buildImageAndPublish();
    void buildCameraInfo(); 
};

} // namespace ros2_nitros_camera

#endif // ROS2_NITROS_CAMERA_H_