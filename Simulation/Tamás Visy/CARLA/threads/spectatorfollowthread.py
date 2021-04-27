import numpy as np
import time

import icarla
from support.logger import logger

from support.datakey import DataKey
from threads.haltabledatathread import HaltableDataThread


class SpectatorFollowThread(HaltableDataThread):

    def __init__(self, data):
        super().__init__(data)
        self.spectator = None
        self.path = None

    def set_spectator_and_path(self, spectator, path):
        self.spectator = spectator
        self.path = path

    def loop(self):
        if self.spectator is None or self.path is None:
            time.sleep(1.0)
        else:
            try:

                pos = self.data.get(DataKey.SENSOR_POSITION)
                rot = self.data.get(DataKey.SENSOR_DIRECTION)

                if pos is not None and rot is not None:
                    position = icarla.transform(pos[0], pos[1], 12.0).location
                    rotation = icarla.rotation([-85, rot[1], 0])

                    position = icarla.transform(pos[0] + 5 * np.cos(self.path.direction(pos[0:2])[1]),
                                                pos[1] + 5 * np.sin(self.path.direction(pos[0:2])[1]),
                                                12.0).location
                    rotation = icarla.rotation([-85, self.path.direction(pos[0:2])[1] / np.pi * 180.0, 0])

                    self.__spectator_move_and_rotate(position, rotation)
            except RuntimeError as r:
                logger.error(f'Error: {r}')
                logger.warning(f'Setting spectator and path to None')
                self.spectator = None
                self.path = None
            time.sleep(3.5)

    def __spectator_move_and_rotate(self, position, direction):
        # Move only seems to work in safe mode (-> which blocks )
        icarla.move(self.spectator, position)

        # Apparently UE4 spectator doesn't like exact 90 degrees, keep it less?
        icarla.rotate(self.spectator, direction, safe=False)
