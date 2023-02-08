# Setup for Tensorflow Lite to Raspberry Pi

## Installing through pip
Make sure you're in the ~/thesis/detect/ folder where sh setup.sh can be seen via the `ls` command

Run the Bash script through:

```
sh setup.sh
```

After the successful installation, make sure you are running tflite_support version `0.4.0`. If not, change to that via:

```
pip list

# Not running tflite-support 0.4.0?
pip uninsstall tflite-support
pip install tflite-support==0.4.0
```

## Running the program
Under the assumed ideal conditions, this command should work:

```
python detect.py --model efficientdet_lite0.tflite --cameraId 0
```

Usual errors involve the wrong `cameraId` number, or a different `model` name 