#!/bin/bash

docker run -d \
    --name isaac_ros_dev-aarch64-container \
    --rm \
    --privileged \
    --network host \
    --ipc host \
    --runtime nvidia \
    --pid host \
    -v /tmp/.X11-unix:/tmp/.X11-unix \
    -v $HOME/.Xauthority:/home/admin/.Xauthority:rw \
    -v $HOME/.bash_history:/home/admin/.bash_history \
    -e DISPLAY \
    -e USER \
    -e NVIDIA_VISIBLE_DEVICES=nvidia.com/gpu=all,nvidia.com/pva=all \
    -e NVIDIA_DRIVER_CAPABILITIES=all \
    -e ROS_DOMAIN_ID \
    -e ISAAC_ROS_WS=/workspaces/isaac_ros-dev \
    -v /tmp/:/tmp/ \
    -v /dev/input:/dev/input \
    -v /etc/localtime:/etc/localtime:ro \
    -v /usr/bin/tegrastats:/usr/bin/tegrastats \
    -v /usr/lib/aarch64-linux-gnu/tegra:/usr/lib/aarch64-linux-gnu/tegra \
    -v /usr/src/jetson_multimedia_api:/usr/src/jetson_multimedia_api \
    -v /usr/share/vpi3:/usr/share/vpi3 \
    -v $ISAAC_ROS_DEV_DIR:/workspaces/isaac_ros-dev \
    --workdir /workspaces/isaac_ros-dev \
    --entrypoint /usr/local/bin/scripts/workspace-entrypoint.sh \
    isaac_ros_dev-aarch64 \
    ros2 launch rosbot_bringup bringup.launch.py