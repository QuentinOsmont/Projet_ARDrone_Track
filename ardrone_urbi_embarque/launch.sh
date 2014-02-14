#!/bin/sh

echo 2 > /proc/cpu/alignment

LD_LIBRARY_PATH=./lib                                          \
./bin/urbi-launch lib/gostai/uobjects/ardrone/ARDrone.so       \
--start -- --host 0.0.0.0 --port 54000

