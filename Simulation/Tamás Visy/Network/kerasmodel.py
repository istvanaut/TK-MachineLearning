# based on kerasmodel.py from Tamás Visy's temalab_gta repo
import gc

# try:
#     RUNTIME
# except NameError:  # if runtime is not defined, we have to import variables from models.model
#     from models.model import Model
#     from support.image_manipulator import normalize, resize, grayscale
#     from support.settings_and_constants import KERAS, INPUT_SHAPE, LEARNING_RATE, MODEL_FILE_NAME, METRIC, \
#         TENSORBOARD_DIR, KERAS_CHECKPOINT_FILE_NAMES, WIDTH, HEIGHT

# TODO (8) refactor this file
# TODO (4) where to put agent_im_stuff
from ReinforcementModel import AGENT_IM_WIDTH, AGENT_IM_HEIGHT

# if KERAS:
from agents.agent import choices_count

print('Use a NumPy version with allow_pickle enabled')
print('Tensorflow warnings and info suppressed')
import os

os.environ['TF_CPP_MIN_LOG_LEVEL'] = '2'
print('Tensorflow 2 requires 64 bit Python')
import tensorflow as tf
from tensorflow import keras
from tensorflow.keras import layers
from tensorflow.keras.optimizers import SGD
import numpy as np
import os

print('Keras imports finished')

INPUT_SHAPE = [AGENT_IM_WIDTH, AGENT_IM_HEIGHT, 1]  # [WIDTH, HEIGHT, 1]
LEARNING_RATE = 0.01
PATH_PREFIX = ''
METRIC = 'val_loss'
# TODO (6) move files
# MODEL_FILE_NAME = PATH_PREFIX + 'other/model/{}/py-gta5-sdc-pilotnet-{}.model'
file_name = 'files/keras.model'
# KERAS_CHECKPOINT_FILE_NAMES = PATH_PREFIX + 'other/checkpoint/model-pilotnet-{name}_best.h5'
checkpoint_file_names = 'files/checkpoints/keras_{epoch}.h5'


class KerasModel:  # class KerasModel(Model):
    network = None

    # predict_as_tensor = None

    def create(self):

        # TODO (5) check validity of model architecture
        network = keras.Sequential(
            [
                # Input      ex.: INPUT_SHAPE = 80, 60, 1
                keras.Input(shape=INPUT_SHAPE),
                # Layer (1)
                layers.Conv2D(96, 11, strides=4, activation='relu', padding='same'),
                # layers.MaxPooling2D(3, strides=2), # TODO (4) valueerror
                layers.Lambda(tf.nn.local_response_normalization),
                # Layer (2)
                layers.Conv2D(256, 5, strides=4, activation='relu', padding='same'),
                # layers.MaxPooling2D(3, strides=2),
                layers.Lambda(tf.nn.local_response_normalization),  # TODO (3) change to BatchNormalization()
                # Layer (3)
                layers.Conv2D(384, 3, activation='relu', padding='same'),
                # Layer (4)
                layers.Conv2D(384, 3, activation='relu', padding='same'),
                # Layer (5)
                layers.Conv2D(256, 3, activation='relu', padding='same'),
                layers.MaxPooling2D(2, strides=2),
                layers.Lambda(tf.nn.local_response_normalization),
                layers.Flatten(),  # I added this
                # Layer (6) # I changed 4096->2048
                layers.Dense(2048, activation='tanh'),
                layers.Dropout(0.5),
                # Layer (7) # I changed 4096->512
                layers.Dense(512, activation='tanh'),
                layers.Dropout(0.5),
                # Layer (8)
                layers.Dense(choices_count, activation='softmax'),
            ]
        )

        # # CNN Architecture based on Nvidia's 'pilotnet'
        # # Changes: filters, dropout, etc.
        # network = keras.Sequential([
        #     # Input
        #     keras.Input(shape=INPUT_SHAPE),
        #     # Layer (1)
        #     layers.Conv2D(8, 5, strides=2, activation='tanh', padding='same'),
        #     layers.BatchNormalization(),
        #     # Layer (2)
        #     layers.Conv2D(16, 5, strides=2, activation='tanh', padding='same'),
        #     layers.BatchNormalization(),
        #     # Layer (3)
        #     layers.Conv2D(24, 5, strides=2, activation='tanh', padding='same'),
        #     layers.BatchNormalization(),
        #     # Layer (4)
        #     layers.Conv2D(32, 3, strides=1, activation='tanh', padding='same'),
        #     layers.BatchNormalization(),
        #     # Layer (5)
        #     layers.Conv2D(40, 3, strides=1, activation='tanh', padding='same'),
        #     layers.BatchNormalization(),
        #     # Flatten
        #     layers.Flatten(),
        #     # Layer (6)
        #     layers.Dense(2048, activation='tanh'),
        #     layers.Dropout(0.3),
        #     # Layer (7)
        #     layers.Dense(512, activation='tanh'),
        #     layers.Dropout(0.2),
        #     # Layer (8)
        #     layers.Dense(128, activation='tanh'),
        #     layers.Dropout(0.1),
        #     # Layer (9)
        #     layers.Dense(16, activation='tanh'),
        #     # Layer (9)
        #     layers.Dense(1, activation='tanh'),
        # ])

        loss = tf.keras.losses.MeanSquaredError(reduction='auto', name='mean_squared_error')

        # TODO (4) understand metrics, loss_weights, optimizer (adam and SGD)
        network.compile(loss=loss, optimizer=SGD(lr=LEARNING_RATE), metrics=['accuracy'])
        network.summary()

        self.network = network
        # self.predict_as_tensor = tf.function(self.model.call)

    def load(self):
        if os.path.isfile(file_name):  # search for actual file
            self.network.load_weights(file_name)
            print(f'Successfully loaded {file_name}model in KerasModel.load()')
        else:
            print(f'ERROR! {file_name} file not found in KerasModel.load()')

    def train(self, train, test, epochs=None):
        # awaiting as {ndarray: (X,)}
        # inside: x {ndarray: (2,)}
        # inside: {ndarray: (width, height)}* and float?         *or other way (height, width)
        print('Num GPUs Available:', len(tf.config.experimental.list_physical_devices('GPU')))

        model_checkpoint_callback = keras.callbacks.ModelCheckpoint(
            filepath=checkpoint_file_names,
            save_weights_only=True,
            monitor=METRIC,
            mode='min',
            save_best_only=False)  # revert to True if needed

        # tensorboard_callback = keras.callbacks.TensorBoard(log_dir=TENSORBOARD_DIR)

        initial_epoch = 0
        found = False
        for i in list(range(1000))[::-1]:
            path = checkpoint_file_names.format(epoch=i)
            # KERAS_CHECKPOINT_FILE_NAMES.format(name=self.name).format(epoch=i)  # This must be sequential
            if not found and os.path.isfile(path):
                print('File exists, loading checkpoint', path)
                self.network.load_weights(path)
                if epochs is None:
                    initial_epoch = i
                found = True
        if not found:
            print('No checkpoint found at', path)

        # (2.) Load train data, split training and testing sets
        #       - training data is what we'll fit the neural network with
        #       - test data is to validate the results &
        #            test the accuracy of the network
        print('Reshaping data...')

        train_x = []
        train_y = []
        for i in train:
            train_x.append(i[0])  # train_x.append(normalize(i[0]))
            train_y.append(i[1])

        train_x = np.array(train_x).reshape([-1, INPUT_SHAPE[0], INPUT_SHAPE[1], INPUT_SHAPE[2]])
        train_y = np.array(train_y)

        print('\tTraining data finished')

        # test_x = np.array([normalize(i[0]) for i in test])
        # .reshape([-1, INPUT_SHAPE[0], INPUT_SHAPE[1], INPUT_SHAPE[2]])
        test_x = np.array([i[0] for i in test]).reshape([-1, INPUT_SHAPE[0], INPUT_SHAPE[1], INPUT_SHAPE[2]])
        test_y = np.array([i[1] for i in test])

        print('Starting training on {} data with {} data as test set'.format(len(train_x), len(test_x)))

        if epochs is None:
            e = 10
        else:
            e = epochs
        # (4.) Training the CNN
        self.network.fit(train_x, train_y, validation_data=(test_x, test_y),
                         epochs=e, initial_epoch=initial_epoch,
                         callbacks=[model_checkpoint_callback])  # Removed tensorboard_callback from callbacks

        # Saving the weights
        self.network.save_weights(file_name)
        print('Finished training, cleaning up...')
        tf.keras.backend.clear_session()  # TODO (4) check whether it helps
        gc.collect()  # TODO (2) remove if doesn't help
        print('Done')

    def predict(self, image):
        # image = grayscale(image)
        # image = resize(image, WIDTH, HEIGHT)
        #
        # image = normalize(image)
        x = np.array([image.reshape(INPUT_SHAPE[0], INPUT_SHAPE[1], INPUT_SHAPE[2])])
        # prediction = self.network.predict_on_batch(x)[0][0]
        prediction = self.network.predict_on_batch(x)[0]
        return prediction
