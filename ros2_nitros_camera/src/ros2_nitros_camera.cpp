#include <ros2_nitros_camera/ros2_nitros_camera.h>

namespace ros2_nitros_camera
{

NitrosCameraNode::NitrosCameraNode(const rclcpp::NodeOptions &node_options) : Node("ros2_nitros_camera", node_options)
{
    this->declare_parameter("camera_id", 0);
    this->declare_parameter("camera_fps", 30);
    this->declare_parameter("image_width", 640);
    this->declare_parameter("image_height", 640);
    this->declare_parameter("frame_id", "camera");
    this->declare_parameter("from_video_file", true);
    this->declare_parameter("camera_calibration_file", "");

    camera_id = this->get_parameter("camera_id").get_parameter_value().get<int>();
    camera_fps = this->get_parameter("camera_fps").get_parameter_value().get<int>();
    image_width_ = this->get_parameter("image_width").get_parameter_value().get<int>();
    image_height_ = this->get_parameter("image_height").get_parameter_value().get<int>();
    frame_id_ = this->get_parameter("frame_id").get_parameter_value().get<std::string>();
    from_video_file_ = this->get_parameter("from_video_file").get_parameter_value().get<bool>();
    camera_calibration_file_ = this->get_parameter("camera_calibration_file").get_parameter_value().get<std::string>();

    camera_pub_ = std::make_shared<nvidia::isaac_ros::nitros::ManagedNitrosPublisher<nvidia::isaac_ros::nitros::NitrosImage>>(
            this, "/image",
            nvidia::isaac_ros::nitros::nitros_image_rgb8_t::supported_type_name,
            nvidia::isaac_ros::nitros::NitrosStatisticsConfig(),
            output_qos
        ); // Right Camera

    camera_info_publisher_ = std::make_shared<nvidia::isaac_ros::nitros::ManagedNitrosPublisher<nvidia::isaac_ros::nitros::NitrosCameraInfo>>(
            this, "camera_info",
            nvidia::isaac_ros::nitros::nitros_camera_info_t::supported_type_name,
            nvidia::isaac_ros::nitros::NitrosStatisticsConfig(),
            output_qos
        );

    if (!camera_calibration_parsers::readCalibration(camera_calibration_file_, camera_name_, camera_info_msg_)) {
        RCLCPP_WARN(get_logger(), "camera_info was not loaded. image_proc will not perform rectification automatically.");
    }

    if (from_video_file_) {
        this->declare_parameter("video_file_path", "src/ros2-robotics-pkgs/ros2_nitros_camera/sample_video/Test_Video.mp4");
        video_file_path_ = this->get_parameter("video_file_path").get_parameter_value().get<std::string>();
        cap.open(video_file_path_);
        RCLCPP_INFO(this->get_logger(), "Reading video file from: %s", video_file_path_.c_str());
    } else {
        cap.open(camera_id, cv::CAP_V4L2);
    }

    cap.set(cv::CAP_PROP_FRAME_WIDTH, image_width_);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, image_height_);
    cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));

    timer_ = this->create_wall_timer(
        std::chrono::milliseconds(static_cast<int>(1000.0 / camera_fps)),
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
        
        nvidia::isaac_ros::nitros::NitrosCameraInfo cameraNitrosInfo_;
        rclcpp::TypeAdapter<nvidia::isaac_ros::nitros::NitrosCameraInfo, sensor_msgs::msg::CameraInfo>::convert_to_custom(camera_info_msg_, cameraNitrosInfo_);

        camera_pub_->publish(std::move(cameraNitrosImage));
        camera_info_publisher_->publish(std::move(cameraNitrosInfo_));      
    } else {
        // cap.open(video_file_path_);
        RCLCPP_WARN(this->get_logger(), "Empty frame or frame not ready.");
    }
    // camera_pub_->publish(*nitros_image);
}

void NitrosCameraNode::buildCameraInfo() {
    nvidia::isaac_ros::nitros::NitrosCameraInfo cameraNitrosInfo_;
    rclcpp::TypeAdapter<nvidia::isaac_ros::nitros::NitrosCameraInfo, sensor_msgs::msg::CameraInfo>::convert_to_custom(camera_info_msg_, cameraNitrosInfo_);

    camera_info_publisher_->publish(std::move(cameraNitrosInfo_));
}

NitrosCameraNode::~NitrosCameraNode()
{
    cap.release();
    RCLCPP_INFO(this->get_logger(), "ROS2 Nitros Camera Grabber Node Terminated.");
}

} // namespace ros2_nitros_camera

#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(ros2_nitros_camera::NitrosCameraNode)