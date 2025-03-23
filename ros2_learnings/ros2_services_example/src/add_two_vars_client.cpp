#include <rclcpp/rclcpp.hpp>
#include <ros2_services_example/srv/add_two_vars.hpp>

using AddTwoVars = ros2_services_example::srv::AddTwoVars;

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);

    auto node = rclcpp::Node::make_shared("add_two_vars_client");
    auto client = node->create_client<AddTwoVars>("add_two_vars");

    while (!client->wait_for_service(std::chrono::seconds(1))) {
        if (!rclcpp::ok()) {
            RCLCPP_ERROR(node->get_logger(), "Interrupted while waiting for the service. Exiting.");
            return 1;
        }
        RCLCPP_INFO(node->get_logger(), "Service not available, waiting again...");
    }

    auto request = std::make_shared<AddTwoVars::Request>();
    request->var1 = 10;
    request->var2 = 20;

    auto future = client->async_send_request(request);
    if (rclcpp::spin_until_future_complete(node, future) == rclcpp::FutureReturnCode::SUCCESS) {
        auto response = future.get();
        RCLCPP_INFO(node->get_logger(), "Sum: %ld", response->sum);
    } else {
        RCLCPP_ERROR(node->get_logger(), "Failed to call service add_two_vars");
    }

    rclcpp::shutdown();

    return 0;
}