import random
import csv
from collections import namedtuple

Transition = namedtuple('Transition',
                        ('state_image', 'state_features', 'action', 'next_image', 'next_features', 'reward'))


# Storage for transitions that can be used to reach supervised style of learning
class ReplayMemory(object):

    def __init__(self, capacity):
        self.capacity = capacity
        self.memory = []
        self.position = 0
        self.time = []

    def push(self, *args, **kwargs):
        """Saves a transition."""
        if len(self.memory) < self.capacity:
            self.memory.append(None)
            self.time.append(None)
        self.memory[self.position] = Transition(*args)
        self.time[self.position] = kwargs.get('time')
        self.position = (self.position + 1) % self.capacity

    def sample(self, batch_size):
        return random.sample(self.memory, batch_size)

    def save(self):
        f = dict(zip(self.time, self.memory))
        w = csv.writer(open("output.csv", "w"))

        top_row = ['Time', 'state_image', 'state_features', 'action', 'next_image', 'next_features', 'reward']
        w.writerow(top_row)
        for key, tuple in dict.items():
            val = [_ for _ in tuple]
            w.writerow([key]+val)

    def __len__(self):
        return len(self.memory)
