#!/bin/sh
make -C ~/arietta/linux-3.16.1 ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- M=`pwd` modules
sshpass -p acmesystems scp hcsr04.ko root@192.168.10.10:/root
