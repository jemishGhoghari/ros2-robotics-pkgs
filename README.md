# Robotics 4.0 - Try to build Autonomous Robots for Industrial Support

---

## Table of Contents

- [Overview](#overview)
- [Project Objectives](#project-objectives)
- [Robot Platform](#robot-platform)
- [Subproject: Object-Guided Navigation](#subproject-object-guided-navigation)
- [Package Documentation](#package-documentation)
- [Workspace Setup](#workspace-setup)
  - [Prerequisites](#prerequisites)
  - [Installation](#installation)
  - [Jetson Autostart](#jetson-autostart-not-required-for-development)
- [Development](#development)

---

## Overview

Building robots in Sim environment for supporting variuos indutries like manufaturing, grocery, agriculture and so on...

**Core focus areas:**

- AI based environmental perception
- Safe Robot Navigation
- ROS 2 integration and node development
- Robot experiments and evaluation

---

## Project Objectives

By the end of this project, We will learn

- Understand the ROS 2 robotics pipeline end-to-end
- Train and evaluate AI perception models
- Development of Safe Navigation
- Robot control
- Evaluate robotic system performance in live scenarios

---

## Robot Platform

|                    |                              |
| ------------------ | ---------------------------- |
| **Platform** | ROSbot XL Gazebo Sim         |
| **Compute**  | Development PCs or Laptops   |
| **Sensor**   | Camera, LiDAR, IMU snd so on |

---

## Package Documentation

| Area              | README        |
| ----------------- | ------------- |
| Gazebo Sim        | No readme yet |
| Docker            | REAMD.md      |
| Utility pkgs      | No ReamMe yet |
| Custom Interfaces | No ReadMe yet |

---

## Workspace Setup

### Prerequisites

The Isaac ROS dev environment can be run either on a desktop PC or on the Jetson device itself.

1. Install Docker using the [official instructions](https://docs.docker.com/engine/install/ubuntu/#install-using-the-repository), or run the commands below. Skip this step if Docker is already installed.

   ```bash
   # Add Docker's official GPG key:
   sudo apt-get update
   sudo apt-get install ca-certificates curl gnupg
   sudo install -m 0755 -d /etc/apt/keyrings
   curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo gpg --dearmor -o /etc/apt/keyrings/docker.gpg
   sudo chmod a+r /etc/apt/keyrings/docker.gpg

   # Add the repository to Apt sources:
   echo \
     "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.gpg] https://download.docker.com/linux/ubuntu \
     $(. /etc/os-release && echo "$VERSION_CODENAME") stable" | \
     sudo tee /etc/apt/sources.list.d/docker.list > /dev/null

   sudo apt-get update

   # Install the Docker packages.
   sudo apt-get install docker-ce docker-ce-cli containerd.io docker-buildx-plugin docker-compose-plugin
   ```
2. Install `nvidia-container-toolkit` using the [official instructions](https://docs.nvidia.com/datacenter/cloud-native/container-toolkit/latest/install-guide.html#installing-with-apt), or run the commands below:

   ```bash
   sudo apt-get update && apt-get install -y --no-install-recommends \
      curl \
      gnupg2
   ```

   ```bash
   curl -fsSL https://nvidia.github.io/libnvidia-container/gpgkey | sudo gpg --dearmor -o /usr/share/keyrings/nvidia-container-toolkit-keyring.gpg \
     && curl -s -L https://nvidia.github.io/libnvidia-container/stable/deb/nvidia-container-toolkit.list | \
       sed 's#deb https://#deb [signed-by=/usr/share/keyrings/nvidia-container-toolkit-keyring.gpg] https://#g' | \
       sudo tee /etc/apt/sources.list.d/nvidia-container-toolkit.list
   ```

   ```bash
   sudo sed -i -e '/experimental/ s/^#//g' /etc/apt/sources.list.d/nvidia-container-toolkit.list
   ```

   ```bash
   sudo apt-get update
   ```

### Installation

#### Isaac ROS Development Environment

1. Set up Docker to run without `sudo` using the [official instructions](https://docs.docker.com/engine/install/linux-postinstall/).
2. Configure the container runtime with `nvidia-ctk`:

   ```bash
   sudo nvidia-ctk runtime configure --runtime=docker && systemctl --user restart docker
   ```

   > [!NOTE]
   > To use `nvidia-container-toolkit` on Jetson with Isaac ROS 3.2, you need to generate a [CDI specification](https://github.com/cncf-tags/container-device-interface) file. This configures the toolkit to use the correct GPU devices and [PVA accelerators](https://docs.nvidia.com/vpi/architecture.html).
   >
   > ```bash
   > sudo nvidia-ctk cdi generate --mode=csv --output=/etc/cdi/nvidia.yaml
   > ```
   >
3. Install `git-lfs`:

   ```bash
   sudo apt-get install git-lfs
   git lfs install --skip-repo
   ```
4. Clone the repository:

   ```bash
   mkdir ~/workspaces && cd ~/workspaces
   git clone https://github.com/jemishGhoghari/ros2-robotics-pkgs.git robotics_experiments
   ```
5. Update the submodules:

   ```bash
   cd ~/workspaces/robotics_experiments
   git submodule update --init --recursive
   ```
6. Link the config for the additional custom [Isaac ROS Development Environment](https://nvidia-isaac-ros.github.io/concepts/docker_devenv/index.html) image layer:

   ```bash
   ln -s ~/workspaces/robotics_experiments/docker/.isaac_ros_common-config ~/.isaac_ros_common-config
   ```
7. Set up the dev convenience script:

   ```bash
   sudo ln -s ~/workspaces/robotics_experiments/docker/startup/rosbotxl_dev_startup.sh ~/robotics_dev_startup.sh
   ```
8. Build and run the Docker image using the dev script. See the [Isaac ROS Common docs](https://nvidia-isaac-ros.github.io/repositories_and_packages/isaac_ros_common/index.html) for details.

   ```bash
   ~/robotics_dev_startup.sh
   ```

#### Jetson Autostart (not required for development)

1. Set up the autostart service:

   ```bash
   mkdir -p ~/.config/systemd/user/
   sudo ln -s ~/workspaces/robotics_experiments/docker/startup/robotics_main_startup.sh ~/rosbotxl_main_startup.sh
   sudo ln -s ~/workspaces/robotics_experiments/docker/startup/robotics-main-startup.service ~/.config/systemd/user/robotics-main-startup.service
   systemctl --user enable robotics-main-startup.service
   ```

   > [!IMPORTANT]
   > The autostart service only runs while the user is logged in. To run it on boot without an active login, either install a [dummy display dongle](https://www.google.com/search?q=dummy+displayport) or enable lingering (so `systemd-logind` spawns a user manager at boot) with:
   >
   > ```bash
   > loginctl enable-linger
   > ```
   >

---

## Development

1. Stop the running container, if autostart is enabled:

   ```bash
   docker stop isaac_ros_dev-${PLATFORM}-container
   ```
2. Launch the dev container:

   ```bash
   ~/robotics_dev_startup.sh -b
   ```
   > [!NOTE]
   > The `-b` flag skips rebuilding the Docker image and uses the existing one.
   >
3. Build the workspace inside the container:

   ```bash
   colcon build --symlink-install
   ```
4. launch gazebo simulation exmaple:

   ```bash
   ros2 launch ros2_control_demo_example_7 r6bot_controller.launch.py
   ```
5. send trajectory command to robot:

   ```bash
   ros2 run ros2_control_demo_example_7 send_trajectory.launch.py
   ```