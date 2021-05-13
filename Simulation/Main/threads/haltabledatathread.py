# based on basethread.py from Tamás Visy's temalab_gta repo
import time
from threading import Thread

from data.datakey import DataKey
from support.logger import logger


class HaltableDataThread(Thread):

    def __init__(self, data):
        super().__init__()
        self.data = data
        self.stop = False

    def run(self):
        self.beginning()
        while not self.stop:
            while not self.data.get(DataKey.THREAD_HALT) and not self.stop:
                self.loop()
            logger.debug(f'{self.__class__.__name__} sleeping')
            time.sleep(1.0)
        self.finish()

    def set_stop(self):
        self.stop = True

    def beginning(self):
        logger.debug(f'{self.__class__.__name__} started')

    def loop(self):
        pass

    def finish(self):
        logger.info(f'{self.__class__.__name__} finishing')
        pass
