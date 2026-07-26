#!/bin/bash

# enable colorful prompt
export TERM=xterm-color

# source ROS and Isaac ROS workspace
source /opt/ros/${ROS_DISTRO}/setup.bash
source ${ISAAC_ROS_WS}/install/setup.bash
export RCUTILS_COLORIZED_OUTPUT=1
export RMW_IMPLEMENTATION=rmw_cyclonedds_cpp
export CYCLONEDDS_URI=/cyclonedds.xml

# add robot model env variable
export ROBOT_MODEL_NAME=rosbot_xl

sudo sysctl -w net.core.rmem_default=10485760

# colcon command completion for bash
source /usr/share/colcon_argcomplete/hook/colcon-argcomplete.bash

# append to ~/.bashrc
cp "/root/.bashrc" /home/${USERNAME}/.bashrc
cat <<EOF >> "/home/${USERNAME}/.bashrc"

export TERM=xterm-color
source /opt/ros/${ROS_DISTRO}/setup.bash
source ${ISAAC_ROS_WS}/install/setup.bash
export RCUTILS_COLORIZED_OUTPUT=1
export RMW_IMPLEMENTATION=rmw_cyclonedds_cpp
export CYCLONEDDS_URI=/cyclonedds.xml
export ROBOT_MODEL_NAME=rosbot_xl
source /usr/share/colcon_argcomplete/hook/colcon-argcomplete.bash
EOF