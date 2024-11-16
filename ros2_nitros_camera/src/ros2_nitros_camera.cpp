#include <ros2_nitros_camera/ros2_nitros_camera.h>

namespace ros2_nitros_camera
{

NitrosCameraNode::NitrosCameraNode(const rclcpp::NodeOptions &node_options) : Node("ros2_nitros_camera", node_options)
{
    this->declare_parameter("camera_id", 2);
    this->declare_parameter("camera_fps", 30);
    this->declare_parameter("image_width", 640);
    this->declare_parameter("image_height", 480);
    this->declare_parameter("frame_id", "camera");

    camera_id = this->get_parameter("camera_id").get_parameter_value().get<int>();
    camera_fps = this->get_parameter("camera_fps").get_parameter_value().get<int>();
    image_width_ = this->get_parameter("image_width").get_parameter_value().get<int>();
    image_height_ = this->get_parameter("image_height").get_parameter_value().get<int>();
    frame_id_ = this->get_parameter("frame_id").get_parameter_value().get<std::string>();

    camera_pub_ = std::make_shared<nvidia::isaac_ros::nitros::ManagedNitrosPublisher<nvidia::isaac_ros::nitros::NitrosImage>>(
            this, "camera/image_raw",
            nvidia::isaac_ros::nitros::nitros_image_rgb8_t::supported_type_name,
            nvidia::isaac_ros::nitros::NitrosStatisticsConfig(),
            output_qos); // Right Camera

    cap.open("/dev/bus/usb/001/001", cv::CAP_V4L2);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, image_width_);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, image_height_);

    timer_ = this->create_wall_timer(
        1ms,
        std::bind(&NitrosCameraNode::buildImageAndPublish, this)
    );
}

void NitrosCameraNode::buildImageAndPublish() {
    cv::Mat frame;
    cap >> frame;

    auto now = std::chrono::steady_clock::now();

    if (!frame.empty() && 
        std::chrono::duration_cast<std::chrono::milliseconds>(now - last_frame_).count() > 1/camera_fps*1000) {
        last_frame_ = now;

        if(frame.rows != image_width_ || frame.cols != image_height_){
            cv::resize(frame, frame, cv::Size(image_width_, image_height_));
        }

        size_t cameraBufferSize{frame.step * frame.size().height};

        void * cameraBuffer;
        cudaMalloc(&cameraBuffer, cameraBufferSize);
        cudaMemcpy(cameraBuffer, frame.data, cameraBufferSize, cudaMemcpyDefault);

        std_msgs::msg::Header imageHeader;
        imageHeader.stamp = this->now();
        imageHeader.frame_id = frame_id_;

        nvidia::isaac_ros::nitros::NitrosImage cameraNitrosImage =
            nvidia::isaac_ros::nitros::NitrosImageBuilder()
            .WithHeader(imageHeader)
            .WithEncoding(img_encodings::RGB8)
            .WithDimensions(frame.size().height, frame.size().width)
            .WithGpuData(cameraBuffer)
            .Build();

        camera_pub_->publish(std::move(cameraNitrosImage));       
    }
    // camera_pub_->publish(*nitros_image);
}

NitrosCameraNode::~NitrosCameraNode()
{
    RCLCPP_INFO(this->get_logger(), "ROS2 Nitros Camera Grabber Node Terminated.");
}

} // namespace ros2_nitros_camera

#include "rclcpp_components/register_node_macro.hpp"

RCLCPP_COMPONENTS_REGISTER_NODE(ros2_nitros_camera::NitrosCameraNode)

// int main(int argc, char * argv[]) {
//     setvbuf(stdout, NULL, _IONBF, BUFSIZ);
    
//     rclcpp::init(argc, argv);
//     auto node = std::make_shared<ros2_nitros_camera::NitrosCameraNode>();
//     auto executor = std::make_shared<rclcpp::executors::SingleThreadedExecutor>();
//     executor->add_node(node);
//     executor->spin();
    
//     rclcpp::shutdown();
//     return 0;
// }