# based on basethread.py from Tamás Visy's temalab_gta repo

from threading import Thread

from support.logger import logger


class BaseThread(Thread):

    def __init__(self, data):
        super().__init__()
        self.data = data
        self.stop = False

    def run(self):
        self.beginning()
        while not self.stop:
            self.loop()
        self.finish()

    def set_stop(self):
        self.stop = True

    def beginning(self):
        logger.info(f'{self.__class__.__name__} started')

    def loop(self):
        pass

    def finish(self):
        logger.info(f'{self.__class__.__name__} finishing')
        pass
