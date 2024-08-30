#!/bin/bash

nmcli c modify eth1 802-3-ethernet.mtu 16334
ethtool -G eth1 rx 8184 tx 8184
ethtool -C eth1 rx-usecs 62 tx-usecs 62

nmcli c modify eth2 802-3-ethernet.mtu 16334
ethtool -G eth2 rx 8184 tx 8184
ethtool -C eth2 rx-usecs 62 tx-usecs 62

nmcli c modify eth3 802-3-ethernet.mtu 16334
ethtool -G eth3 rx 8184 tx 8184
ethtool -C eth3 rx-usecs 62 tx-usecs 62

nmcli c modify eth4 802-3-ethernet.mtu 16334
ethtool -G eth4 rx 8184 tx 8184
ethtool -C eth4 rx-usecs 62 tx-usecs 62

exit 0