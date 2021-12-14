from threading import Thread

from gui.window import Window, QUIT
from support.logger import logger


class DashboardThread(Thread):

    def __init__(self):
        super().__init__()
        self.window = None

    def run(self):
        logger.debug(f'{self.__class__.__name__} started')
        self.window = Window()
        logger.debug(f'{self.__class__.__name__} ready')
        self.window.work()
        logger.debug(f'{self.__class__.__name__} finished')

    def handle(self, state, out=None):
        if self.window is not None:
            self.window.handle(state, out)
        else:
            logger.debug('Window is None, can not handle incoming')

    def set_stop(self):
        if self.window is not None:
            self.window.add_event(QUIT)
        else:
            logger.debug('Window is None, can not stop')

    def clear(self):
        if self.window is not None:
            self.window.clear()
        else:
            logger.debug('Window is None, can not handle clear')
