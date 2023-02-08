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

print(tf.config.list_physical_devices('GPU'))

spec = model_spec.get('efficientdet_lite0')