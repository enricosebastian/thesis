#!/bin/bash

. "/home/pi/Desktop/tf/bin/activate" #activate the python virtual environment
cd /home/pi/Desktop/thesis/python

lxterminal -e python initial.py
lxterminal -e python reader.py

cd /home/pi/Desktop/thesis/detect
lxterminal -e python detect.py
