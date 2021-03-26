import time

import icarla
from support.logger import logger

HOST = 'localhost'
PORT = 2000


class Connection:
    def __init__(self):
        self.client = None
        self.world = None

    def connect(self):
        while self.client is None or self.world is None:
            self.__connect()
            if self.client is None or self.world is None:
                time.sleep(2.0)

    def __connect(self):
        try:
            logger.info(f'Connecting to {HOST}:{PORT}...')
            self.client = icarla.client(HOST, PORT)
            # Even on a local machine, CARLA server takes about 2-3 seconds to respond - especially loading maps is long
            self.client.set_timeout(10.0)
            self.world = self.client.get_world()
        except RuntimeError as r:
            logger.error(f'Could not connect to server: {r}')
            return None, None
