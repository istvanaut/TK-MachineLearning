import time

from support.logger import logger
from threads.basethread import BaseThread
from support.datakey import DataKey


class SpectatorFollowThread(BaseThread):
    spectator = None
    connection = None

    def set_spectator_and_connection (self, s, c):
        self.spectator = s
        self.connection = c

    def loop(self):
        if self.spectator is None or self.connection is None:
            time.sleep(1.0)
        else:
            try:
                pos = self.data.get(DataKey.SENSOR_POSITION)
                # TODO update rot to use line direction at current position
                rot = self.data.get(DataKey.SENSOR_DIRECTION)
                if pos is not None and rot is not None:

                    position = self.spectator.transform(pos[0],
                                                pos[1],
                                                12.0).location
                    rotation = self.spectator.rotation([-85, rot[1], 0])
                    self.__spectator_move_and_rotate(position, rotation)
            except RuntimeError as r:
                logger.error(f'Error: {r}')
                logger.warning(f'Setting spectator to None')
                self.spectator = None

    def __spectator_move_and_rotate(self, position, direction):
        self.spectator.move(self.connection.world.get_spectator(),
                    position)

        # Apparently UE4 spectator doesn't like exact 90 degrees, keep it less?
        self.spectator.rotate(self.connection.world.get_spectator(),
                      direction)