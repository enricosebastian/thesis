# based on: https://www.youtube.com/watch?v=sarZ_FZfDxs

import numpy as np
import os

from tflite_model_maker.config import ExportFormat
from tflite_model_maker import model_spec
from tflite_model_maker import object_detector

import tensorflow as tf
assert tf.__version__.startswith('2')

tf.get_logger().setLevel('ERROR')
from absl import logging
logging.set_verbosity(logging.ERROR)

objects = ['plastic_bottle', 'drone']
train_folder = 'images/train'
validate_folder = 'images/validate'

# Before running, make sure the terminal is in the right folder:
# cd Desktop/thesis/tflite/prototype/[folder-name]


# This is where the training begins...
train_data = object_detector.DataLoader.from_pascal_voc(
    train_folder,
    train_folder,
    objects
    )

val_data = object_detector.DataLoader.from_pascal_voc(
    validate_folder,
    validate_folder,
    objects
    )

print(tf.config.list_physical_devices('GPU'))
spec = model_spec.get('efficientdet_lite0')

model = object_detector.create(train_data, model_spec=spec, batch_size=4, train_whole_model=True, epochs=20, validation_data=val_data)

print(model.evaluate(val_data))

model.export(export_dir='./models/', tflite_filename='bottle-and-drone.tflite')


print(model.evaluate_tflite('./models/bottle-and-drone.tflite', val_data))

print("\n\n Model making done.")