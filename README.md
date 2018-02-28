# TFMongoDB
## Overview

TFMongoDB is a C++ implemented dataset op for google’s tensorflow that allows you to connect to your MongoDatabase natively. Hence you can access your mongodb stored documents more efficiently.

Currently only MacOS X is supported while Tensorflow >= 1.5 is required.

## Install

In order to use the dataset you need to install it with pip:

    pip install tfmongodb


## Usage

TFMongoDB can be accessed through the MongoDBDataset:

    dataset = MongoDBDataset(<database_name>, <collection_name>)

example:

    from tfmongodb import MongoDBDataset
    from tensorflow.python.framework import ops
    from tensorflow.python.data.ops import iterator_ops
    import tensorflow as tf

    CSV_TYPES = [[""], [""], [0]]

    def _parse_line(line):
        fields = tf.decode_csv(line, record_defaults=CSV_TYPES)
        return fields

    dataset = MongoDBDataset("eccounting", "creditors")
    dataset = dataset.map(_parse_line)
    repeat_dataset2 = dataset.repeat()
    batch_dataset = repeat_dataset2.batch(20)

    iterator = iterator_ops.Iterator.from_structure(batch_dataset.output_types)
    #init_op = iterator.make_initializer(dataset)
    init_batch_op = iterator.make_initializer(batch_dataset)
    get_next = iterator.get_next()


    with tf.Session() as sess:
        sess.run(init_batch_op, feed_dict={})

        for i in range(5):
            print(sess.run(get_next))

