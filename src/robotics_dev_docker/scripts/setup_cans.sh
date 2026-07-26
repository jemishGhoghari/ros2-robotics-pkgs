#!/bin/bash

echo config devmem
busybox devmem 0x0c303018 w 0x0000C458 #can0_din
busybox devmem 0x0c303010 w 0x0000C400 #can0_dout
busybox devmem 0x0c303008 w 0x0000C458 #can1_din
busybox devmem 0x0c303000 w 0x0000C400 #can1_dout

modprobe can
modprobe can_raw
modprobe mttcan

# Returns true if iface exists and is up, otherwise false.
function ifup {
    typeset output
    output=$(ip link show "$1" up) && [[ -n $output ]]
}

for i in $(seq 0 1); do
    if ifup can$i; then
        echo set can$i down
        ip link set can$i down
    else
        echo can$i is not up
    fi
    echo set can$i up
    ip link set can$i up type can bitrate 1000000 restart-ms 100 berr-reporting on
    ip link set can$i txqueuelen 1000
done

exit 0