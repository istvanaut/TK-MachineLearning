import time
from threading import Thread

from support.logger import logger
from window import Window, QUIT


class DashboardThread(Thread):
    data = None

    def __init__(self, data):
        super().__init__()
        self.data = data
        self.window = None

    def run(self):
        logger.info(f'{self.__class__.__name__} started')
        self.window = Window(self.data)
        self.window.work()

    def set_stop(self):
        self.window.add_event(QUIT)
