#include <image_to_nitros/image_to_nitros.h>

using namespace std::chrono_literals;
using namespace std::placeholders;

namespace image_to_nitros
{

ImageToNitros::ImageToNitros(const rclcpp::NodeOptions& options) : Node("image_to_nitros_node", options)
{
    camera_info_subscriber_ = this->create_subscription<sensor_msgs::msg::CameraInfo>(
        "camera_info", 10,
        std::bind(&ImageToNitros::CameraInfoCallback, this, _1));
    
    image_subscriber_ = this->create_subscription<sensor_msgs::msg::Image>(
        "image", 10,
        std::bind(&ImageToNitros::ImageCallback, this, _1));

    nitros_camera_pub_ = std::make_shared<nvidia::isaac_ros::nitros::ManagedNitrosPublisher<nvidia::isaac_ros::nitros::NitrosImage>>(
        this, "nitros_gpu_image",
        nvidia::isaac_ros::nitros::nitros_image_rgba8_t::supported_type_name);
    
    nitros_camera_info_publisher_ = std::make_shared<nvidia::isaac_ros::nitros::ManagedNitrosPublisher<nvidia::isaac_ros::nitros::NitrosCameraInfo>>(
        this, "nitros_camera_info",
        nvidia::isaac_ros::nitros::nitros_camera_info_t::supported_type_name);
}

ImageToNitros::~ImageToNitros() = default;

void ImageToNitros::ImageCallback(const sensor_msgs::msg::Image::SharedPtr camera_image) {
    sensor_msgs::msg::Image::SharedPtr camera_image_const = cv_bridge::toCvCopy(camera_image, "rgb8")->toImageMsg();
    // Get size of image
    size_t buffer_size{camera_image_const->step * camera_image_const->height};

    // Allocate CUDA buffer to store image
    void * buffer;
    cudaMalloc(&buffer, buffer_size);

    // Copy data bytes to CUDA buffer
    cudaMemcpy(buffer, camera_image_const->data.data(), buffer_size, cudaMemcpyDefault);

    // Adding header data
    std_msgs::msg::Header header;
    header.stamp.sec = 123456;
    header.stamp.nanosec = 789101112;
    header.frame_id = "ego_vehicle/rgb_front";

    // Create NitrosImage wrapping CUDA buffer
    nvidia::isaac_ros::nitros::NitrosImage nitros_image =
        nvidia::isaac_ros::nitros::NitrosImageBuilder()
        .WithHeader(header)
        .WithEncoding(img_encodings::RGB8)
        .WithDimensions(camera_image_const->height, camera_image_const->width)
        .WithGpuData(buffer)
        .Build();
    
    nitros_camera_pub_->publish(nitros_image);
}

void ImageToNitros::CameraInfoCallback(const sensor_msgs::msg::CameraInfo::SharedPtr camera_info) {
    nvidia::isaac_ros::nitros::NitrosCameraInfo cameraNitrosInfo_;
    rclcpp::TypeAdapter<nvidia::isaac_ros::nitros::NitrosCameraInfo, sensor_msgs::msg::CameraInfo>::convert_to_custom(*camera_info, cameraNitrosInfo_);

    nitros_camera_info_publisher_->publish(cameraNitrosInfo_);
}

} // namespace image_to_nitros

#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(image_to_nitros::ImageToNitros)