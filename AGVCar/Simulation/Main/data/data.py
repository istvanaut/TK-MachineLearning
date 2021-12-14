# based on data.py from Tamás Visy's temalab_gta repo
import copy
from threading import Lock


class Data:
    data = None
    lock = None

    def __init__(self, data=None):
        if data is None:
            data = dict()
        self.data = data
        self.lock = Lock()

    def put(self, key, data):
        with self.lock:
            self.data[key] = copy.deepcopy(data)

    def get(self, key=None, delete=False):
        with self.lock:
            data = None
            if key is None:
                data = self.data
                if delete:
                    self.data = dict()
            if key in self.data.keys():
                data = self.data[key]
                if delete:
                    self.data[key] = None
            return copy.deepcopy(data)

    def push(self, data, key):
        with self.lock:
            d = None
            if key in self.data.keys():
                d = self.data[key]
            if d is None:
                d = []

            d.append(copy.deepcopy(data))
            self.data[key] = d

    def pop(self, key):
        with self.lock:
            d = None
            if key in self.data.keys():
                d = self.data[key]
            if d is None or len(d) is 0:
                return None
            else:
                element = d[0]
                d = d[1::]
                self.data[key] = d
                return copy.deepcopy(element)

    def copy(self):
        return Data(self.data)

    def clear(self):
        with self.lock:
            self.data = dict()
