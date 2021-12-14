import numpy as np
import time

from CARLA import icarla
from support.logger import logger
from threads.haltabledatathread import HaltableDataThread


class SpectatorFollowThread(HaltableDataThread):

    def __init__(self, data):
        super().__init__(data)
        self.spectator = None
        self.vehicle = None
        self.path = None

    def set_knowledge(self, spectator, vehicle, path):
        self.spectator = spectator
        self.vehicle = vehicle
        self.path = path

    def loop(self):
        if self.spectator is None or self.vehicle is None or self.path is None:
            time.sleep(1.0)
        else:
            try:
                self.update()
            except RuntimeError as r:
                logger.error(f'Error: {r}')
                logger.warning(f'Setting vehicle to None')
                self.vehicle = None

    # TODO (6) this makes spectatorfollowthread not haltable -> use new (base/) parent class
    def run(self):
        self.beginning()
        while not self.stop:
            while not self.stop:
                self.loop()
            logger.debug(f'{self.__class__.__name__} sleeping')
            time.sleep(1.0)
        self.finish()

    def update(self):
        p = self.vehicle.get_location()
        pos = [p.x, p.y, p.z]
        # TODO (8) fix some issue - example: flipping ("backwards") at the second half of the path
        position = icarla.transform(pos[0] + 5 * np.cos(self.path.direction(pos[0:2])[1]),
                                    pos[1] + 5 * np.sin(self.path.direction(pos[0:2])[1]),
                                    12.0).location
        rotation = icarla.rotation([-85, self.path.direction(pos[0:2])[1] / np.pi * 180.0, 0])

        self.__spectator_move_and_rotate(position, rotation)

    def __spectator_move_and_rotate(self, position, direction):
        # Move only seems to work in safe mode (-> which blocks )
        icarla.move(self.spectator, position)

        # Apparently UE4 spectator doesn't like exact 90 degrees, keep it less?
        icarla.rotate(self.spectator, direction)
