import random

from agents.agent import Agent
from settings import choices
from kerasmodel import KerasModel
from support.logger import logger
import numpy as np


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
    logger.info(f'Trainables: {len(trainables)} -> {len(leftchoices)+len(rightchoices)}')
    return leftchoices+rightchoices


class KerasAgent(Agent):

    def __init__(self):
        super().__init__()

        self.choices = choices

        self.model = KerasModel()

    def load(self):
        self.model.create()
        self.model.load()

    def predict(self, state, pure=True, auto=False):
        if state is None:
            return None, None

        im, _, _ = state.get_formatted()
        prediction = self.model.predict(im)
        action = np.argmax(prediction)
        # print(action)

        # # auto driving
        # if pure is not True and auto:
        #     action = 1
        #
        # # random exploration
        # if pure is not True and random.random() < 0.4:
        #     if random.random() < 0.5:
        #         action = 0
        #     else:
        #         action = 1

        choice = choices[action][:]
        # # auto driving
        # if pure is not True and auto:
        #     choice[1] *= -1*state.side
        # # noise
        # if pure is not True:
        #     choice[1] += -0.05 + random.random() * 0.1
        return action, choice

    def optimize(self, new_state, prev_state=None, action=None):
        logger.critical('Optimize unsupported for this agent.')

    def train_on_memory(self, memory):
        states = [prev_state for (prev_state, action, new_state) in memory]
        self.train_model(states)

    def train_model(self, data=None):
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
        self.model.train(train=train, test=val, epochs=4)

    def save_as_tflite(self):
        self.model.save_as_tflite()
