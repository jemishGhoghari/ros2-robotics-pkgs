#!/bin/bash

modprobe can
modprobe can_raw
modprobe mttcan
modprobe vcan
modprobe can-gw

# Returns true if iface exists, otherwise false.
function ifexists {
    typeset output
    output=$(ip link show "$1") && [[ -n $output ]]
}

# Returns true if iface exists and is up, otherwise false.
function ifup {
    typeset output
    output=$(ip link show "$1" up) && [[ -n $output ]]
}

if ifup can0; then
    echo set can0 down
    ip link set can0 down
else
    echo can0 is not up
fi

echo set can0 up
ip link set can0 up type can bitrate 1000000 restart-ms 100 berr-reporting on
ip link set can0 txqueuelen 1000

echo flush cangw rules
cangw -F

if ifexists vcan0; then
    echo delete vcan0
    ip link delete vcan0
fi

echo add vcan0
ip link add dev vcan0 type vcan

echo set vcan0 up
ip link set vcan0 up txqueuelen 1000

echo forward can0 to vcan0
cangw -A -s vcan0 -d can0 -e -t
cangw -A -s can0 -d vcan0 -e -t

exit 0