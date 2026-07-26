#!/bin/bash

# Partition label to check
PARTITION_LABEL="data-partition"

# Mount point to mount the partition if it is not mounted
TARGET_MOUNT_POINT="/media/dv"

# Get the device path of the partition labeled "data-partition"
DEVICE_PATH=$(blkid -l -t LABEL="$PARTITION_LABEL" -o device)

# Check if the partition is found
if [ -z "$DEVICE_PATH" ]; then
  echo "Partition with label '$PARTITION_LABEL' not found."
  exit 1
fi

# Get the mount point of the partition labeled "data-partition"
MOUNT_POINT=$(lsblk -o LABEL,MOUNTPOINT | grep "^$PARTITION_LABEL" | awk '{print $2}')

# Check if the partition is mounted
if [ -n "$MOUNT_POINT" ]; then
  echo "Partition '$PARTITION_LABEL' is already mounted at '$MOUNT_POINT'."
else
  echo "Partition '$PARTITION_LABEL' is not mounted. Mounting it at '$TARGET_MOUNT_POINT'."
  
  # Create the target mount point directory if it does not exist
  if [ ! -d "$TARGET_MOUNT_POINT" ]; then
    sudo mkdir -p "$TARGET_MOUNT_POINT"
  fi

  # Mount the partition
  sudo mount "$DEVICE_PATH" "$TARGET_MOUNT_POINT"

  # Check if the mount was successful
  if [ $? -eq 0 ]; then
    echo "Partition '$PARTITION_LABEL' successfully mounted at '$TARGET_MOUNT_POINT'."
  else
    echo "Failed to mount partition '$PARTITION_LABEL' at '$TARGET_MOUNT_POINT'."
    exit 1
  fi
fi
