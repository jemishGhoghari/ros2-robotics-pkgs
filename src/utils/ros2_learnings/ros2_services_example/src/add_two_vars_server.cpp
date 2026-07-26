#include "rclcpp/rclcpp.hpp"
#include "ros2_services_example/srv/add_two_vars.hpp"

using AddTwoVars = ros2_services_example::srv::AddTwoVars;
using std::placeholders::_1;
using std::placeholders::_2;

class AddTwoVarsServer : public rclcpp::Node
{
public:
  AddTwoVarsServer()
  : Node("add_two_vars_server")
  {
    service_ = this->create_service<AddTwoVars>("add_two_vars", std::bind(&AddTwoVarsServer::callback, this, _1, _2));
  }

private:
    void callback(const std::shared_ptr<AddTwoVars::Request> request, std::shared_ptr<AddTwoVars::Response> response)
    {
        response->sum = request->var1 + request->var2;
        RCLCPP_INFO(this->get_logger(), "Incoming request\nvar1: %ld\nvar2: %ld", request->var1, request->var2);
        RCLCPP_INFO(this->get_logger(), "Sending back response: [%ld]", response->sum);
    }
    
    rclcpp::Service<AddTwoVars>::SharedPtr service_;
};

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<AddTwoVarsServer>());
  rclcpp::shutdown();
  return 0;
}