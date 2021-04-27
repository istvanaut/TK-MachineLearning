import time

import icarla
from support.logger import logger

from support.datakey import DataKey
from threads.haltabledatathread import HaltableDataThread


class SpectatorFollowThread(HaltableDataThread):

    def __init__(self, data):
        super().__init__(data)
        self.spectator = None

    def set_spectator(self, spectator):
        self.spectator = spectator

    def loop(self):
        if self.spectator is None:
            time.sleep(1.0)
        else:
            try:
                # TODO (8) update rot to use line direction at current position

                # position = icarla.transform(self.path.start[0] + 5 * np.cos(self.path.direction()[1]),
                #                             self.path.start[1] + 5 * np.sin(self.path.direction()[1]),
                #                             12.0).location
                # rotation = icarla.rotation([-85, self.path.direction()[1] / np.pi * 180.0, 0])

                pos = self.data.get(DataKey.SENSOR_POSITION)
                rot = self.data.get(DataKey.SENSOR_DIRECTION)

                if pos is not None and rot is not None:
                    position = icarla.transform(pos[0], pos[1], 12.0).location
                    rotation = icarla.rotation([-85, rot[1], 0])

                    # position = icarla.transform(self.path.start[0] + 5 * np.cos(self.path.direction()[1]),
                    #                             self.path.start[1] + 5 * np.sin(self.path.direction()[1]),
                    #                             12.0).location
                    # rotation = icarla.rotation([-85, self.path.direction()[1] / np.pi * 180.0, 0])

                    self.__spectator_move_and_rotate(position, rotation)
            except RuntimeError as r:
                logger.error(f'Error: {r}')
                logger.warning(f'Setting spectator to None')
                self.spectator = None
            time.sleep(0.75)

    def __spectator_move_and_rotate(self, position, direction):
        icarla.move(self.spectator, position)

        # Apparently UE4 spectator doesn't like exact 90 degrees, keep it less?
        icarla.rotate(self.spectator, direction)
