# Copyright 2017 The TensorFlow Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ==============================================================================
"""Kafka Dataset."""
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from tensorflow.contrib.util import loader
from tensorflow.python.data.ops.readers import Dataset
from tensorflow.python.framework import common_shapes
from tensorflow.python.framework import dtypes
from tensorflow.python.framework import ops
from tensorflow.python.framework import tensor_shape
from tensorflow.python.platform import resource_loader
from tensorflow.python.data.ops import iterator_ops

import tensorflow as tf

class MongoDBDataset(Dataset):
  """A Kafka Dataset that consumes the message.
  """

  def __init__(self, database, collection):
    """Create a KafkaReader.

    Args:
      topics: A `tf.string` tensor containing one or more subscriptions,
              in the format of [topic:partition:offset:length],
              by default length is -1 for unlimited.
      servers: A list of bootstrap servers.
      group: The consumer group id.
      eof: If True, the kafka reader will stop on EOF.
      timeout: The timeout value for the Kafka Consumer to wait
               (in millisecond).
    """
    module = tf.load_op_library('./../../cmake-build-debug/libTFMongoDB.so')

    super(MongoDBDataset, self).__init__()
    self._database = ops.convert_to_tensor(
        database, dtype=dtypes.string, name="database")
    self._collection = ops.convert_to_tensor(
      collection, dtype=dtypes.string, name="collection")
    self.rr_ = module.mongo_dataset(self._database, self._collection)


  def _as_variant_tensor(self):
      return self.rr_

  @property
  def output_classes(self):
    return ops.Tensor

  @property
  def output_shapes(self):
    return tensor_shape.scalar()

  @property
  def output_types(self):
    return dtypes.string


repeat_dataset = MongoDBDataset("names", "firstNames")
repeat_dataset2 = repeat_dataset.repeat()
batch_dataset = repeat_dataset2.batch(2)

iterator = iterator_ops.Iterator.from_structure(batch_dataset.output_types)
init_op = iterator.make_initializer(repeat_dataset2)
init_batch_op = iterator.make_initializer(batch_dataset)
get_next = iterator.get_next()


with tf.Session() as sess:
    # Basic test: read from topic 0.
    sess.run(init_batch_op, feed_dict={})

    for i in range(20):
        print(sess.run(get_next))

    print("Shutting down...")
