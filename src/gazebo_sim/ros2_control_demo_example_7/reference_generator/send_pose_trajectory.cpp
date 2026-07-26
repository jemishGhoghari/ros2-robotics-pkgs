#include <kdl/chainfksolverpos_recursive.hpp>
#include <kdl/chainiksolverpos_lma.hpp>
#include <kdl/chainiksolvervel_pinv.hpp>
#include <kdl/jntarray.hpp>
#include <kdl/tree.hpp>
#include <kdl_parser/kdl_parser.hpp>
#include <rclcpp/rclcpp.hpp>
#include <trajectory_msgs/msg/joint_trajectory.hpp>
#include <trajectory_msgs/msg/joint_trajectory_point.hpp>

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<rclcpp::Node>("send_pose_trajectory");
    auto pub = node->create_publisher<trajectory_msgs::msg::JointTrajectory>(
        "/r6bot_controller/joint_trajectory", 10);

    // Get robot description from parameter server
    auto robot_param = rclcpp::Parameter();
    node->declare_parameter("robot_description", rclcpp::ParameterType::PARAMETER_STRING);
    node->get_parameter("robot_description", robot_param);
    auto robot_description = robot_param.as_string();

    // Create kinematic chain
    KDL::Tree robot_tree;
    KDL::Chain chain;
    kdl_parser::treeFromString(robot_description, robot_tree);
    robot_tree.getChain("base_link", "tool0", chain);

    // Initialize joint position arrays
    KDL::JntArray joint_positions(chain.getNrOfJoints());
    KDL::JntArray target_joint_positions(chain.getNrOfJoints());

    // Define target pose (modify these values based on the desired goal position)
    KDL::Frame target_pose(
        KDL::Rotation::RPY(1, 0, 1),  // No rotation (can be modified)
        KDL::Vector(0.7, 0.9, 0.1));  // Desired position (X, Y, Z)

    // Create IK solvers
    auto ik_pos_solver = std::make_shared<KDL::ChainIkSolverPos_LMA>(chain); // LMA IK Solver

    // Compute inverse kinematics to get target joint positions
    int ik_result = ik_pos_solver->CartToJnt(joint_positions, target_pose, target_joint_positions);
    if (ik_result < 0)
    {
        RCLCPP_ERROR(node->get_logger(), "Inverse Kinematics failed!");
        return -1;
    }

    // Create trajectory message
    trajectory_msgs::msg::JointTrajectory trajectory_msg;
    trajectory_msg.header.stamp = node->now();
    for (size_t i = 0; i < chain.getNrOfSegments(); i++)
    {
        auto joint = chain.getSegment(i).getJoint();
        if (joint.getType() != KDL::Joint::Fixed)
        {
            trajectory_msg.joint_names.push_back(joint.getName());
        }
    }

    trajectory_msgs::msg::JointTrajectoryPoint trajectory_point_msg;
    trajectory_point_msg.positions.resize(chain.getNrOfJoints());
    trajectory_point_msg.velocities.resize(chain.getNrOfJoints());

    double total_time = 3.0; // Time to reach the goal (seconds)
    int trajectory_len = 200; // Number of trajectory points
    int loop_rate = trajectory_len / total_time;
    double dt = 1.0 / loop_rate;

    // Generate smooth trajectory from initial to target joint positions
    for (int i = 0; i < trajectory_len; i++)
    {
        double alpha = static_cast<double>(i) / trajectory_len; // Interpolation factor
        for (size_t j = 0; j < chain.getNrOfJoints(); j++)
        {
            trajectory_point_msg.positions[j] = (1 - alpha) * joint_positions(j) + alpha * target_joint_positions(j);
            trajectory_point_msg.velocities[j] = (target_joint_positions(j) - joint_positions(j)) / total_time;
        }

        // Set timing information
        trajectory_point_msg.time_from_start.sec = i / loop_rate;
        trajectory_point_msg.time_from_start.nanosec = static_cast<int>(
            1E9 / loop_rate * static_cast<double>(i % loop_rate));

        trajectory_msg.points.push_back(trajectory_point_msg);
    }

    // Publish trajectory
    pub->publish(trajectory_msg);
    RCLCPP_INFO(node->get_logger(), "Trajectory sent to move arm to target pose!");

    while (rclcpp::ok()) {}

    return 0;
}
