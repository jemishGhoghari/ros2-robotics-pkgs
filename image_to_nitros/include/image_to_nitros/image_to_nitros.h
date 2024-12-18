/**
 * @file image_to_nitros.h
 * @brief Header file for the ROS2 Image type to Nitros Image type conversion node.
 *
 * This file contains the declarations for the ROS2 Nitros Camera Grabber Node,
 * which is responsible for interfacing with camera hardware, capturing images,
 * and publishing them to ROS2 topics for further processing and analysis.
 *
 * @project image_to_nitros
 * @date 12.12.2024
 * @author Jemish Ghoghari <mailto:jemishghoghari50@gmail.com>
 */

#ifndef IMAGE_TO_NITROS_H_
#define IMAGE_TO_NITROS_H_

#include <chrono>
#include <cstdio>
#include <memory>
#include <string>

#include <rclcpp/rclcpp.hpp>
#include "message_filters/subscriber.h"
#include "message_filters/synchronizer.h"
#include "message_filters/sync_policies/exact_time.h"

#include <isaac_ros_managed_nitros/managed_nitros_publisher.hpp>
#include <isaac_ros_managed_nitros/managed_nitros_subscriber.hpp>
#include <isaac_ros_nitros_image_type/nitros_image.hpp>
#include <isaac_ros_nitros_image_type/nitros_image_builder.hpp>
#include <isaac_ros_nitros_camera_info_type/nitros_camera_info.hpp>
#include <isaac_ros_common/qos.hpp>

#include <sensor_msgs/msg/camera_info.hpp>
#include "cv_bridge/cv_bridge.h"

#include <cuda_runtime.h>

using namespace std::chrono_literals;
using namespace std::placeholders;

namespace image_to_nitros
{
namespace {
    constexpr const char kDefaultQoS[] = "DEFAULT";
}

class ImageToNitros : public rclcpp::Node
{
private:
    std::shared_ptr<nvidia::isaac_ros::nitros::ManagedNitrosPublisher<nvidia::isaac_ros::nitros::NitrosImage>> nitros_camera_pub_;
    std::shared_ptr<nvidia::isaac_ros::nitros::ManagedNitrosPublisher<nvidia::isaac_ros::nitros::NitrosCameraInfo>> nitros_camera_info_publisher_;
    rclcpp::Subscription<sensor_msgs::msg::CameraInfo>::SharedPtr camera_info_subscriber_;
    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr image_subscriber_;

    const rclcpp::QoS output_qos = ::isaac_ros::common::AddQosParameter(
    *this, kDefaultQoS, "output_qos").keep_last(10);

public:
    explicit ImageToNitros(const rclcpp::NodeOptions&);
    ~ImageToNitros();

    void ImageCallback(const sensor_msgs::msg::Image::SharedPtr camera_image);   
    void CameraInfoCallback(const sensor_msgs::msg::CameraInfo::SharedPtr camera_info);
};

} // namespace image_to_nitros

#endif // IMAGE_TO_NITROS_H_