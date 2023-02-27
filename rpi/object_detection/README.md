# TensorFlow Lite Python object detection example with Raspberry Pi
This is the portion of the thesis responsible for object detection. It uses a modified detect.py TensorFlow Lite file to send commands to the Arduino brain module.

## Tutorial
1. Create a virtualenv that has all the previous pip installs. Do this through `virtualenv tf --system-site-packages`
1. Make sure that you are in the current `thesis/rpi/object_detection` directory
1. Ensure that `setup.sh` exists within the folder. Run the command `sh setup.sh`
1. Wait for all the important libraries and models to be installed
1. Install additional libraries via `sudo apt-get install libatlas-base-dev`
1. Run the detect.py Python file. Type `python detect.py`