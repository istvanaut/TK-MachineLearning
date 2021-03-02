# from Tamás Visy's temalab_gta repo

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
        self.lock.acquire()
        if self.lock.locked():
            self.data[key] = data
            self.lock.release()
        else:
            raise Exception('Data locking')

    def get(self, key=None, delete=False):
        self.lock.acquire()
        if self.lock.locked():
            data = None
            if key is None:
                data = self.data
                if delete:
                    self.data = dict()
            if key in self.data.keys():
                data = self.data[key]
                if delete:
                    self.data[key] = None
            self.lock.release()
            return data
        else:
            raise Exception('Data locking')

    def push(self, data, key):
        self.lock.acquire()
        if self.lock.locked():
            d = None
            if key in self.data.keys():
                d = self.data[key]
            if d is None:
                d = []

            d.append(data)
            self.data[key] = d
            self.lock.release()
        else:
            raise Exception('Data locking')

    def pop(self, key):
        self.lock.acquire()
        if self.lock.locked():
            d = None
            if key in self.data.keys():
                d = self.data[key]
            if d is None or len(d) is 0:
                self.lock.release()
                return None
            else:
                element = d[0]
                d = d[1::]
                self.data[key] = d
                self.lock.release()
                return element
        else:
            raise Exception('Data locking')

    def copy(self):
        return Data(self.data)
