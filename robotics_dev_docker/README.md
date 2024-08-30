# srd_docker

## Installation

### Build isaac_ros_common docker image
1. Install docker
```
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

2. Install git-lfs
```
sudo apt-get install git-lfs
git lfs install --skip-repo
```

3. On x86_64 platforms:
    - Install the nvidia-container-toolkit using the [instructions](https://docs.nvidia.com/datacenter/cloud-native/container-toolkit/latest/install-guide.html#installing-with-apt).
    - Configure nvidia-container-toolkit for Docker using the [instructions](https://docs.nvidia.com/datacenter/cloud-native/container-toolkit/latest/install-guide.html#configuring-docker).

4. Setup isaac_ros-dev workspace
```
mkdir -p ~/workspaces/isaac_ros-dev/src
cd ~/workspaces/isaac_ros-dev/src
git clone https://github.com/NVIDIA-ISAAC-ROS/isaac_ros_common.git
git clone git@github.com:SchanzerRacing/srd_acu_docker.git -b develop
cp srd_acu_docker/scripts/.isaac_ros_common-config isaac_ros_common/scripts
isaac_ros_common/scripts/run_dev.sh $HOME/workspaces/isaac_ros-dev/
```

### Setup automatic host configuration on startup
1. Setup can service
```
cd ~/workspaces/isaac_ros-dev/src/srd_acu_docker/startup
sudo cp srd_can_startup.sh /usr/local/bin/
sudo cp srd-can-startup.service /etc/systemd/system/
sudo chmod 755 /usr/local/bin/srd_can_startup.sh
sudo systemctl enable srd-can-startup.service
```

2. Setup docker autostart service
```
mkdir -p ~/.config/systemd/user/
cd ~/workspaces/isaac_ros-dev/src/srd_acu_docker/startup
cp srd_dev_startup.sh ~
cp srd_main_startup.sh ~
cp srd-main-startup.service ~/.config/systemd/user/
systemctl --user enable srd-main-startup.service
```

## Usage

`sudo /usr/local/bin/srd_can_startup.sh`(re-)start cans
`~/srd_dev_startup.sh` just runs the isaac-ros docker with shell
`~/srd_main_startup.sh` runs the isaac-ros docker detached and launches main