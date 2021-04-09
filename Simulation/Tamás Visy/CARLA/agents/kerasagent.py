import random

from agents.agent import Agent, choices
from agents.state import feature_dimension
from kerasmodel import KerasModel
from support.logger import logger
import numpy as np

feature_dimension = feature_dimension()


def states_to_trainable(data):
    trainable = []
    for state in data:
        inp = state.image
        choice = [0, 0]  # -1*state.side
        if state.side < 0:
            choice[1] = 1
        if state.side > 0:
            choice[0] = 1
        trainable.append([inp, choice])
    return trainable


class KerasAgent(Agent):

    def __init__(self):
        super().__init__()

        self.choices = choices

        self.model = KerasModel()

    def load(self):
        self.model.create()
        self.model.load()

    def predict(self, state):
        if state is None:
            return None, None
        im, _, _ = state.get_formatted()
        prediction = self.model.predict(im)
        action = np.argmax(prediction)
        print(prediction)
        choice = choices[action]
        return action, choice

    def optimize(self, new_state, prev_state=None, action=None):
        logger.critical('Use train instead of kerasagent optimize')

    def train(self, data=None):
        if data is None:
            val = None
            train = None
            # val = get_validation_data()
            # train = get_train_data()
            pass
        else:
            trainable = states_to_trainable(data)
            random.shuffle(trainable)
            val_len = len(trainable)//10
            val = trainable[:val_len]
            train = trainable[val_len:]
        self.model.train(train=train, test=val, epochs=3)
