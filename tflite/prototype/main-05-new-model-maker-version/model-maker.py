# based on prerequisites here: https://www.tensorflow.org/lite/models/modify/model_maker/object_detection
# and https://www.tensorflow.org/lite/api_docs/python/tflite_model_maker/object_detector/DataLoader#from_csv

# cd Desktop/thesis/tflite/prototype/main-05-new-model-maker-version
import os

from tflite_model_maker import image_classifier
from tflite_model_maker.image_classifier import DataLoader
from tflite_model_maker.config import QuantizationConfig
from tflite_model_maker.config import ExportFormat
from tflite_model_maker import model_spec
from tflite_model_maker import object_detector
 
import tensorflow as tf
assert tf.__version__.startswith('2')

tf.get_logger().setLevel('ERROR')
from absl import logging
logging.set_verbosity(logging.ERROR)

# CHANGE VALUES HERE ONLY
batch_size = 4 # default is 8
epochs = 100 # default is 50

model_name = 'efficientdet_lite0'
train_dir = './images/all/train'
validate_dir = './images/all/validate'
test_dir = './images/all/test'
label_map = ['plastic_bottle', 'drone', 'obtrusion']

export_dir = './models/'
model_filename = 'all.tflite'
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

test_data = object_detector.DataLoader.from_pascal_voc(
    images_dir=test_dir,
    annotations_dir=test_dir,
    label_map=label_map
)

model = object_detector.create(train_data, model_spec=spec, epochs=epochs, batch_size=batch_size, train_whole_model=True, validation_data=validation_data)

print("=============Validation results==============\n")
print(
    model.evaluate(validation_data)
)

print("=============Test results==============\n")
print(
    model.evaluate(test_data)
)

model.export(export_dir=export_dir, tflite_filename=model_filename)

print("=============Validation results for exported model==============\n")
print(
    model.evaluate_tflite(export_dir+model_filename, validation_data)
)

print("=============Test results for exported model==============\n")
print(
    model.evaluate_tflite(export_dir+model_filename, test_data)
)

print("\n\nDone with model of batch size " + str(batch_size) + " and epoch " + str(epochs))
