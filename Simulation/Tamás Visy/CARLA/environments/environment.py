# @title environment
import random

import numpy as np
import icarla
from environments.connection import Connection
from environments.spawner import spawn_vehicle, spawn_camera, spawn_radar, spawn_collision, spawn_obstacle
from environments.status import Status
from line import get_line, Line
from support.datakey import DataKey
from support.logger import logger
from threads.controllerthread import ControllerThread
from support.data import Data
from threads.pollerthread import PollerThread

# Town05 has best paths on the inside of the motorway (still in the town)
MAP_NAME = 'Town05'  # Best map: Town05 or Town03, least performance demanding map: Town02


class Environment:
    # TODO (5) refactor every "self.connection.world. ..."
    #  in this file and in others
    def __init__(self):
        self.actors = []
        self.data = Data()
        self.vehicle = None
        self.line = None

        logger.warning('Halting threads')
        self.data.put(DataKey.THREAD_HALT, True)

        self.c = ControllerThread(self.data)
        self.p = PollerThread(self.data)

        self.connection = Connection()
        # TODO (6) double reset at start?

    def connect(self):
        self.connection.connect()

    def setup(self):
        logger.info('Environment setup')
        self.set_conditions()
        self.get_line()
        self.spawn()
        self.reset()

    def start(self):
        self.c.start()
        self.p.start()

    def set_conditions(self):
        current_map_name = self.connection.world.get_map().name
        # Loading correct map
        # TODO (9) wait longer if load fails first
        if current_map_name != MAP_NAME:
            logger.info(f'Loading map: {MAP_NAME} <- {current_map_name}')
            try:
                self.connection.world = self.connection.client.load_world(MAP_NAME)
            except RuntimeError as r:
                logger.critical(f'{r}')
                raise r
        else:
            # Destroying old actors
            actors = self.connection.world.get_actors()
            # TODO (10) check if this destroys sensor actors as well - if not, performance hit?
            for actor in actors.filter('vehicle.*.*'):
                actor.destroy()
            if len(actors.filter('vehicle.*.*')) > 0:
                logger.info('Cleaned up old actors')
        # Setting nice weather
        self.set_weather()

    def set_weather(self):
        weather = self.connection.world.get_weather()

        weather.precipitation = 0.0
        weather.precipitation_deposits = 0.0
        weather.wetness = 0.0

        self.connection.world.set_weather(weather)
        logger.info('Applied nice weather')

    def spawn(self):
        logger.info('Spawning actors, sensors')
        spawn_vehicle(self, self.line.start, self.line.direction())
        spawn_camera(self)
        spawn_radar(self)
        spawn_collision(self)
        spawn_obstacle(self)

    def reset(self):
        logger.debug('Resetting actors')
        self.clear()
        logger.debug('Halting threads')
        self.data.put(DataKey.THREAD_HALT, True)
        self.vehicle.apply_control(icarla.vehicle_control(throttle=0, steer=0))
        icarla.set_velocity(self.vehicle, icarla.vector3d())

        self.get_line()

        icarla.move(self.vehicle, icarla.transform(self.line.start[0], self.line.start[1], 0.25).location)
        icarla.rotate(self.vehicle, icarla.rotation_from_radian(self.line.direction()))
        icarla.move(self.connection.world.get_spectator(),
                    icarla.transform(self.line.start[0] + 10 * np.cos(self.line.direction()[1]),
                                     self.line.start[1] + 10 * np.sin(self.line.direction()[1]),
                                     12.0).location)
        # Apparently UE4 spectator doesn't like exact 90 degrees, keep it less?
        icarla.rotate(self.connection.world.get_spectator(),
                      icarla.rotation([-85, self.line.direction()[1]/np.pi*180.0, 0]))
        logger.info('Environment reset successful')

    def clear(self):
        logger.debug('Clearing data')
        self.data.clear()

    def pull(self):
        return self.data.copy(), Line(self.line.points), self.line.direction()  # data, line and starting direction

    def put(self, key, data):
        self.data.put(key, data)

    def check(self):
        s = Status()
        s.check(self)
        return s

    def get_line(self):
        self.line = get_line()
        # TODO (3) make prettier
        r = random.random()
        # if r < 1/6:
        #     logger.info('Environment: normal short')
        #     self.line.slice(None, 20)
        # elif r < 2/6:
        #     logger.info('Environment: backwards short')
        #     self.line.slice(70, None)
        #     self.line.invert()
        # elif
        if r < 1/4:
            logger.info('Environment: normal full')
            pass
        elif r < 2/4:
            logger.info('Environment: backwards full')
            self.line.invert()
        elif r < 3/4:
            logger.info('Environment: normal turn (left)')
            self.line.slice(30, 60)
        else:
            logger.info('Environment: backwards turn (right)')
            self.line.slice(40, 70)
            self.line.invert()
