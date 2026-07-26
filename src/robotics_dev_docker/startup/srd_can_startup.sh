#!/bin/bash

modprobe can
modprobe can_raw
modprobe mttcan
modprobe vcan
modprobe can-gw

# setup cans
echo config devmem
busybox devmem 0x0c303018 w 0x458 #can0_din
busybox devmem 0x0c303010 w 0x400 #can0_dout
busybox devmem 0x0c303008 w 0x458 #can1_din
busybox devmem 0x0c303000 w 0x400 #can1_dout

cangw -F

echo config and up cans
for i in $(seq 0 1); do
    ip link set can$i down
    ip link set can$i up type can bitrate 1000000 prop-seg 8 phase-seg1 5 phase-seg2 5 sjw 4
    ip link set can$i txqueuelen 1000
done

echo config vcans
for i in $(seq 0 1); do
    ip link add dev vcan$i type vcan
    ip link set vcan$i down
    ip link set vcan$i up txqueuelen 1000
done

echo config can forwarding
for i in $(seq 0 1); do
    cangw -A -s vcan$i -d can$i -e -t -l 1
    cangw -A -s can$i -d vcan$i -e -t -l 1
done

exit 0
