from threading import Thread

from support.logger import logger
from window import Window, QUIT


class DashboardThread(Thread):

    def __init__(self):
        super().__init__()
        self.window = None

    def run(self):
        logger.info(f'{self.__class__.__name__} started')
        self.window = Window()
        logger.info(f'{self.__class__.__name__} ready')
        self.window.work()
        logger.info(f'{self.__class__.__name__} finished')

    def handle(self, data, line, starting_dir, state, out=None, pure=False):
        if self.window is not None:
            self.window.handle(data, line, starting_dir, state, out, pure)
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
