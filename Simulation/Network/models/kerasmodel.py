# based on kerasmodel.py from Tamás Visy's temalab_gta repo
import gc

from settings import choices_count, AGENT_IM_WIDTH, AGENT_IM_HEIGHT
from support.logger import logger

# TODO (2) move and update warnings - best into readme or somewhere where they are actually read

logger.debug('Use a NumPy version with allow_pickle enabled')

logger.warning('Tensorflow warnings and info suppressed')
import os

os.environ['TF_CPP_MIN_LOG_LEVEL'] = '2'

logger.debug('Tensorflow 2 requires 64 bit Python')

import tensorflow as tf
from tensorflow import keras
from tensorflow.keras import layers
from tensorflow.keras.optimizers import SGD
from tensorflow.lite.python.lite import TFLiteConverterV2
import numpy as np
import os

logger.debug('Keras imports finished')

INPUT_SHAPE = [AGENT_IM_WIDTH, AGENT_IM_HEIGHT, 1]  # [WIDTH, HEIGHT, 1]
LEARNING_RATE = 0.1
PATH_PREFIX = ''
METRIC = 'val_loss'
file_name = 'files/keras/keras.model'
checkpoint_file_names = 'files/keras/checkpoints/keras_{epoch}.h5'


class KerasModel:
    network = None

    # predict_as_tensor = None

    def save_as_tflite(self):
        converter = TFLiteConverterV2.from_keras_model(self.network)
        tflite_model = converter.convert()
        open('files/converted_model.tflite', 'wb').write(tflite_model)

    def create(self):
        # TODO (5) check validity of model architecture
        network = keras.Sequential(
            [
                # Input      ex.: INPUT_SHAPE = 80, 60, 1
                keras.Input(shape=INPUT_SHAPE),
                # Layer (1)
                layers.Conv2D(16, 7, strides=4, activation='relu', padding='same'),
                # layers.MaxPooling2D(3, strides=2), # TODO (4) valueerror
                layers.Lambda(tf.nn.local_response_normalization),
                # Layer (2)
                layers.Conv2D(32, 5, strides=4, activation='relu', padding='same'),
                # layers.MaxPooling2D(3, strides=2),
                layers.Lambda(tf.nn.local_response_normalization),  # TODO (3) change to BatchNormalization()
                # Layer (3)
                layers.Conv2D(32, 3, activation='relu', padding='same'),
                # # Layer (4)
                # layers.Conv2D(64, 3, activation='relu', padding='same'),
                # # Layer (5)
                # layers.Conv2D(256, 3, activation='relu', padding='same'),
                layers.MaxPooling2D(2, strides=2),
                layers.Lambda(tf.nn.local_response_normalization),
                layers.Flatten(),  # I added this
                # Layer (6) # I changed 4096->2048->256
                # layers.Dense(128, activation='tanh'),
                # layers.Dropout(0.5),
                # Layer (7) # I changed 4096->512->32
                layers.Dense(8, activation='tanh'),
                # layers.Dropout(0.5),
                # Layer (8)
                layers.Dense(choices_count, activation='softmax'),
            ]
        )

        loss = tf.keras.losses.BinaryCrossentropy()

        network.compile(loss=loss, optimizer=SGD(lr=LEARNING_RATE), metrics=['accuracy'])

        logger.debug('Skipping kerasmodel summary')
        # network.summary()

        self.network = network
        # self.predict_as_tensor = tf.function(self.model.call)

    def save(self):
        if self.network is not None:
            self.network.save_weights(file_name)
        else:
            logger.warning('Cannot save network as it is None')

    def load(self):
        if os.path.isfile(f'{file_name}.data-00000-of-00001'):  # search for actual file
            self.network.load_weights(file_name)
            logger.debug(f'Successfully loaded {file_name} model in KerasModel.load()')
        else:
            logger.error(f'{file_name} file not found in KerasModel.load()')

    def train(self, train, test, epochs=None):
        # awaiting as {ndarray: (X,)}
        # inside: x {ndarray: (2,)}
        # inside: {ndarray: (width, height)}* and float?         *or other way (height, width)
        logger.debug(f'Num GPUs Available: {len(tf.config.experimental.list_physical_devices("GPU"))}')

        model_checkpoint_callback = keras.callbacks.ModelCheckpoint(
            filepath=checkpoint_file_names,
            save_weights_only=True,
            monitor=METRIC,
            mode='min',
            save_best_only=False)  # revert to True if needed

        initial_epoch = 0
        found = False
        path = ''
        for i in list(range(1000))[::-1]:
            path = checkpoint_file_names.format(epoch=i)
            if not found and os.path.isfile(path):
                logger.debug(f'File exists, loading checkpoint {path}')
                self.network.load_weights(path)
                if epochs is None:
                    initial_epoch = i
                found = True
        if not found:
            logger.warning(f'No checkpoint found at {path}')

        logger.debug('Reshaping data...')

        train_x = []
        train_y = []
        for i in train:
            train_x.append(i[0])
            train_y.append(i[1])

        train_x = np.array(train_x).reshape([-1, INPUT_SHAPE[0], INPUT_SHAPE[1], INPUT_SHAPE[2]])
        train_y = np.array(train_y)

        logger.debug('Training data finished')

        test_x = np.array([i[0] for i in test]).reshape([-1, INPUT_SHAPE[0], INPUT_SHAPE[1], INPUT_SHAPE[2]])
        test_y = np.array([i[1] for i in test])

        logger.debug('Starting training on {} data with {} data as test set'.format(len(train_x), len(test_x)))

        if epochs is None:
            e = 10
        else:
            e = epochs
        # (4.) Training the CNN
        logger.debug('Not printing epoch training data in KerasModel')
        self.network.fit(train_x, train_y, validation_data=(test_x, test_y),
                         epochs=e, initial_epoch=initial_epoch,
                         callbacks=[model_checkpoint_callback], verbose=0)

        # Saving the weights
        self.network.save_weights(file_name)
        logger.debug('Finished training, cleaning up...')
        # Long training session can crash
        tf.keras.backend.clear_session()  # TODO (4) check whether it helps
        gc.collect()  # TODO (2) remove if doesn't help
        logger.debug('Done')

    def predict(self, image):
        x = np.array([image.reshape(INPUT_SHAPE[0], INPUT_SHAPE[1], INPUT_SHAPE[2])])
        prediction = self.network.predict_on_batch(x)[0]
        return prediction
