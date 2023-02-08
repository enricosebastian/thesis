#!/bin/bash

. "/home/pi/Desktop/tf/bin/activate" #activate the python virtual environment
cd /home/pi/Desktop/thesis/python

xterm -e python /home/pi/Desktop/thesis/python/demo.py &
xterm -e python /home/pi/Desktop/thesis/python/sending_json_demo.py
