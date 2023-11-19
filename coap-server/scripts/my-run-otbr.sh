#!/bin/bash

# Exit the script if any command fails
set -e

echo -e "Trying to create 172.18.0.0/16 network\n"
sudo docker network create --subnet=172.18.0.0/16 test || true

echo -e "Loading ip6table_filter\n"
sudo modprobe ip6table_filter

# Should be modified to remove only otbr docker
echo -e "Cleaning otbr docker\n"
sudo docker rm $(sudo docker ps -a | grep "openthread/otbr" | tr -s " " | cut -f1 -d " ") || true

echo -e "Running otbr docker\n"
sudo docker run --name otbr --sysctl "net.ipv6.conf.all.disable_ipv6=0 net.ipv4.conf.all.forwarding=1 net.ipv6.conf.all.forwarding=1" -p 8080:80 --dns=127.0.0.1 -v /dev/ttyACM0:/dev/ttyACM0 --net test --ip 172.18.0.6 --privileged openthread/otbr --radio-url spinel+hdlc+uart:///dev/ttyACM0