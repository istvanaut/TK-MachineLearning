import tensorflow as tf
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense, Dropout, Activation, Flatten, Conv2D, MaxPooling2D
from tensorflow.keras.utils import to_categorical
from tensorflow.keras.callbacks import TensorBoard
import pickle
import time
import sklearn.model_selection as sk

X = pickle.load(open("/content/X_10.pickle", "rb"))
y = pickle.load(open("/content/y_10.pickle", "rb"))

X = X / 255.0
y = to_categorical(y, 10)  # you need to set how many categories there are

X_train, X_test, y_train, y_test = sk.train_test_split(X, y, test_size=0.20, random_state=42)

# starting parameters
BATCH = 16

for i in range(3):
  for j in range(3):
    NAME = "pics64_window{}_pooling{}_{}".format(i + 1, j + 1, int(time.time()))
    tensorboard = TensorBoard(log_dir='logs/{}'.format(NAME))
    print(NAME)
    model = Sequential()
    model.add(Conv2D(64, (i+1, i+1), activation='relu', input_shape=X_train.shape[1:]))
    model.add(MaxPooling2D(j+1, j+1))
    #
    model.add(Flatten())
    #
    model.add(Dense(512, activation='relu'))
    #
    model.add(Dense(10, activation='softmax'))

    model.compile(loss='categorical_crossentropy',
                  optimizer="adam",
                  metrics=['accuracy'])

    model_fit = model.fit(X_train, y_train, batch_size=BATCH,
                          epochs=30,
                          validation_data=(X_test, y_test),
                          callbacks=[tensorboard])
