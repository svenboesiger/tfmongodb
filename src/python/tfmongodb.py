# Copyright 2017 The TensorFlow Authors. All Rights Reserved.
# Copyright 2018 Sven Boesiger. All Rights Reserved.
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
"""MongoDB Dataset."""
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import tensorflow as tf
import os

from tensorflow.data import Dataset
from tensorflow.python.framework import dtypes
from tensorflow.python.framework import ops
from tensorflow.python.framework import tensor_shape

class MongoDBDataset(Dataset):
    """A MongoDB Dataset.
    """
    __MODULE_NAME="XXX"
    def __init__(self, database, collection, uri=""):
        """Create a MongoDB Dataset.

        Args:
          database:     A `tf.string` tensor that contains the name of the
                        database.
          collection:   A `tf.string` tensor that contains the name of the
                        collection.
          uri       :   A `tf.string` tensor that specifies the MongoDB URI host
        """
        module = tf.load_op_library(os.path.join(os.path.dirname(__file__), self.__MODULE_NAME))

        super(MongoDBDataset, self).__init__()
        self._database = ops.convert_to_tensor(
            database, dtype=dtypes.string, name="database")
        self._collection = ops.convert_to_tensor(
            collection, dtype=dtypes.string, name="collection")
        self._uri = ops.convert_to_tensor(
            uri, dtype=dtypes.string, name="uri")
        self.rr_ = module.mongo_dataset(self._database, self._collection, self._uri)

    def _as_variant_tensor(self):
        return self.rr_

    @property
    def output_classes(self):
        return tf.Tensor

    @property
    def output_shapes(self):
        return  tf.TensorShape([])

    @property
    def output_types(self):
        return dtypes.string

    def _inputs(self):
        """Returns a list of the input datasets of the dataset."""
        return []