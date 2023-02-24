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

spec = model_spec.get('efficientdet_lite0')

train_data = object_detector.DataLoader.from_pascal_voc(
    images_dir="./hand-signs/train",
    annotations_dir="./hand-signs/train",
    label_map=["thumbs", "peace"]
)

validation_data = object_detector.DataLoader.from_pascal_voc(
    images_dir="./hand-signs/validate",
    annotations_dir="./hand-signs/validate",
    label_map=["thumbs", "peace"]
)

model = object_detector.create(train_data, model_spec=spec, batch_size=8, train_whole_model=True, validation_data=validation_data)

print(
    model.evaluate(validation_data)
)

model.export(export_dir='.', tflite_filename='demo-02.tflite')

print(
    model.evaluate_tflite('demo-02.tflite', validation_data)
)

print("Done!")
