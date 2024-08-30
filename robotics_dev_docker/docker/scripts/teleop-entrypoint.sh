#!/bin/bash

echo "source /opt/ros/${ROS_DISTRO}/setup.bash" >> ~/.bashrc
echo "source /workspaces/isaac_ros-dev/install/setup.bash" >> ~/.bashrc
source /opt/ros/${ROS_DISTRO}/setup.bash
source /workspaces/isaac_ros-dev/install/setup.bash

sudo service udev restart

ros2 launch srd_teleop teleop.launch.py