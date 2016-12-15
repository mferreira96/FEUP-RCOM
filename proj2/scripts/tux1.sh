#!/bin/bash

ifconfig eth0 up
ifconfig eth0 172.16.51.1/24
route add -net 172.16.51.0/24 gw 172.16.50.254
route add default gw 172.16.50.254
printf "search lixa.fe.up.pt\nnameserver 172.16.1.1\n" > /etc/resolv.conf
