#!/bin/bash

# run isaac_ros docker without building
$HOME/workspaces/robotics_experiments/src/isaac_ros_common/scripts/run_dev.sh \
--isaac_ros_dev_dir $HOME/workspaces/robotics_experiments \
--docker_arg "-v $HOME/.bash_history:/home/admin/.bash_history" \
--docker_arg "-v /dev:/dev" \
$@