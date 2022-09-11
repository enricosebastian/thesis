#!/bin/bash

. "/home/pi/Desktop/tf/bin/activate" #activate the python virtual environment
cd /home/pi/Desktop/thesis/python

lxterminal -e python thesis-receiver.py
lxterminal -e python thesis-transmitter.py