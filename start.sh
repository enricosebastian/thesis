#!/bin/bash

. "/home/pi/Desktop/thesis-object-detection-model/tf/bin/activate" #activate the python virtual environment
cd /home/pi/Desktop/pyth/transceiving

lxterminal -e python thesis-receiver.py
lxterminal -e python thesis-transmitter.py