#!/bin/bash

. "/home/pi/Desktop/tf/bin/activate" #activate the python virtual environment
cd /home/pi/Desktop/thesis/python

lxterminal -e python /home/pi/Desktop/thesis/python/initial.py
lxterminal -e python /home/pi/Desktop/thesis/python/reader.py
lxterminal -e python /home/pi/Desktop/thesis/python/detect.py
