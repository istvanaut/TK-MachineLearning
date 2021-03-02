# based on basethread.py from Tamás Visy's temalab_gta repo

from threading import Thread


class BaseThread(Thread):
    data = None

    def __init__(self, data):
        super().__init__()
        self.data = data

    def run(self):
        self.beginning()
        while True:
            self.loop()

    def beginning(self):
        print(self.__class__.__name__, 'started')

    def loop(self):
        pass
