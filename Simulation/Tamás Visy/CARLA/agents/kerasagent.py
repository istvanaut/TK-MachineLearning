import random

from agents.agent import Agent, choices
from agents.state import feature_dimension
from kerasmodel import KerasModel
from support.logger import logger
import numpy as np

feature_dimension = feature_dimension()


def states_to_trainables(data):
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


def balance(trainables):
    leftchoices = []
    rightchoices = []
    for trainable in trainables:
        if np.argmax(trainable[1]) == 0:
            leftchoices.append(trainable)
        elif np.argmax(trainable[1]) == 1:
            rightchoices.append(trainable)
    leftchoices = leftchoices[:min(len(leftchoices), len(rightchoices))]
    rightchoices = rightchoices[:min(len(leftchoices), len(rightchoices))]
    print(len(trainables), '->', len(leftchoices)+len(rightchoices))
    return leftchoices+rightchoices


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
        auto = random.random() < 1/3
        # random exploration
        if random.random() < 0.6:
            if random.random() < 0.5:
                action = 0
            else:
                action = 1
        # autodriving
        if auto:
            action = 1
        choice = choices[action][:]
        # autodriving
        if auto:
            choice[1] *= -1*state.side
            # noise
            choice[1] += -0.1 + random.random() * 0.2
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
            trainables = states_to_trainables(data)
            random.shuffle(trainables)
            trainables = balance(trainables)
            random.shuffle(trainables)
            val_len = len(trainables)//10
            val = trainables[:val_len]
            train = trainables[val_len:]
        self.model.train(train=train, test=val, epochs=3)
