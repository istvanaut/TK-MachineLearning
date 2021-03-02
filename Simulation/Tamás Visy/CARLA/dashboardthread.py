import random
from threading import Thread
import numpy as np

import window
from data import Data
from key import Key


class DashboardThread(Thread):
    data = None

    def __init__(self, data):
        super().__init__()
        self.data = data

    def run(self):
        window.create(self.data)


if __name__ == '__main__':
    d = Data()
    i = np.concatenate((random.random() * np.ones([200, 200, 3]), random.random() * np.ones([200, 200, 3])))
    d.put(Key.SENSOR_CAMERA, i)
    del i
    dt = DashboardThread(d)
    dt.start()
