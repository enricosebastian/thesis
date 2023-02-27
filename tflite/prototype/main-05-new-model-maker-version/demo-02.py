# based on prerequisites here: https://www.tensorflow.org/lite/models/modify/model_maker/object_detection
# and https://www.tensorflow.org/lite/api_docs/python/tflite_model_maker/object_detector/DataLoader#from_csv
import numpy as np
import os

from tflite_model_maker.config import QuantizationConfig
from tflite_model_maker.config import ExportFormat
from tflite_model_maker import model_spec
from tflite_model_maker import object_detector

import tensorflow as tf
assert tf.__version__.startswith('2')

tf.get_logger().setLevel('ERROR')
from absl import logging
logging.set_verbosity(logging.ERROR)

# CONSTANTS HERE
model_name = 'efficientdet_lite0'
train_dir = './images/new-hand-images/train'
validate_dir = './images/new-hand-images/validate'
label_map = ['thumbs_up', 'ok', 'thumbs_down']

export_dir = './models/'
model_filename = 'new-hand-images.tflite'
#######################

spec = model_spec.get(model_name)

train_data = object_detector.DataLoader.from_pascal_voc(
    images_dir=train_dir,
    annotations_dir=train_dir,
    label_map=label_map
)

validation_data = object_detector.DataLoader.from_pascal_voc(
    images_dir=validate_dir,
    annotations_dir=validate_dir,
    label_map=label_map
)

model = object_detector.create(train_data, model_spec=spec, batch_size=8, train_whole_model=True, validation_data=validation_data)

print(
    model.evaluate(validation_data)
)

model.export(export_dir=export_dir, tflite_filename=model_filename)

print(
    model.evaluate_tflite(export_dir+model_filename, validation_data)
)

print("\n\nDone!")
