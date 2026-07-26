#!/bin/bash

source /opt/ros/${ROS_DISTRO}/setup.bash
source /workspaces/isaac_ros-dev/install/setup.bash
export RCUTILS_COLORIZED_OUTPUT=1

echo "source /opt/ros/${ROS_DISTRO}/setup.bash" >> ~/.bashrc
echo "source /workspaces/isaac_ros-dev/install/setup.bash" >> ~/.bashrc
echo "export RCUTILS_COLORIZED_OUTPUT=1" >> ~/.bashrc

sudo service udev restart

ros2 launch srd_main main.launch.py
