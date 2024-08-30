#!/bin/bash

modprobe vcan
modprobe can-gw

# Returns true if iface exists, otherwise false.
function ifexists {
    typeset output
    output=$(ip link show "$1") && [[ -n $output ]]
}

echo flush cangw rules
cangw -F

for i in $(seq 0 1); do
    if ifexists vcan$i; then
        echo delete vcan$i
        ip link delete vcan$i
    fi

    echo add vcan$i
    ip link add dev vcan$i type vcan

    echo set vcan$i up
    ip link set vcan$i up txqueuelen 1000

    echo forward can$i to vcan$i
    cangw -A -s vcan$i -d can$i -e -t
    cangw -A -s can$i -d vcan$i -e -t
done

exit 0