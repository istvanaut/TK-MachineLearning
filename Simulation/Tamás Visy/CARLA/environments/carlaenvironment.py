import random

import numpy as np
import icarla
from environments.connection import Connection
from environments.environment import Environment
from environments.spawner import spawn_vehicle, spawn_camera, spawn_radar, spawn_collision, spawn_obstacle
from environments.status import Status
from environments.path import get_path, Path
from settings import MAP_NAME
from support.datakey import DataKey
from support.logger import logger
from threads.controllerthread import ControllerThread
from support.data import Data
from threads.pollerthread import PollerThread
from threads.spectatorfollowthread import SpectatorFollowThread


# Town05 has best paths on the inside of the motorway (still in the town)
# Best map: Town05 or Town03, least performance demanding map: Town02
# Current path on Town05

# TODO (5) refactor every "self.connection.world. ..."
#  in this file and in others


class CarlaEnvironment(Environment):
    def __init__(self):
        self.actors = []
        self.data = Data()
        self.vehicle = None
        self.path = None

        logger.warning('Halting threads')
        self.data.put(DataKey.THREAD_HALT, True)

        self.c = ControllerThread(self.data)
        self.p = PollerThread(self.data)
        self.s = SpectatorFollowThread(self.data)

        self.connection = Connection()

    def setup(self):
        logger.debug('Environment setup')
        self.connection.connect()
        self.__set_conditions()
        # self.__update_path()
        self.__spawn()
        self.reset()

    def start(self):
        self.c.start()
        self.p.start()
        self.s.start()

    def reset(self):
        logger.debug('Resetting actors')
        self.clear()
        logger.warning('Halting threads')
        self.data.put(DataKey.THREAD_HALT, True)
        self.vehicle.apply_control(icarla.vehicle_control(throttle=0, steer=0))
        icarla.set_velocity(self.vehicle, icarla.vector3d())
        logger.debug('Environment reset successful')

    def clear(self):
        # Clears all data -> removes thread_halt -> threads can resume
        logger.debug('Clearing data')
        self.data.clear()

    def pull(self):
        return self.data.get(), Path(self.path.points), self.path.direction()  # data, path and starting direction

    def put(self, key, data):
        self.data.put(key, data)

    def check(self):
        s = Status()
        s.check(self)
        return s

    def __set_conditions(self):
        current_map_name = self.connection.world.get_map().name
        # Loading correct map
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
            for actor in actors.filter('vehicle.*.*'):
                actor.destroy()
            for actor in actors.filter('sensor.*.*'):
                actor.destroy()
            if len(actors.filter('vehicle.*.*')) > 0 and len(actors.filter('sensor.*.*')) > 0:
                logger.debug('Cleaned up old actors')
            else:
                logger.warning('Issues while cleaning up old actors')
        # Setting nice weather
        self.__set_weather()

    def __set_weather(self):
        weather = self.connection.world.get_weather()

        weather.precipitation = 0.0
        weather.precipitation_deposits = 0.0
        weather.wetness = 0.0

        self.connection.world.set_weather(weather)
        logger.debug('Applied nice weather')

    def __spawn(self):
        logger.debug('Spawning actors, sensors')
        if self.path is None:
            spawn_vehicle(self, [0.0, 0.0], [0.0, 0.0, 0.0])
        else:
            spawn_vehicle(self, self.path.start, self.path.direction())
        spawn_camera(self)
        spawn_radar(self)
        spawn_collision(self)
        spawn_obstacle(self)

    def __update_path(self, i):
        self.path = get_path()
        # TODO (3) make prettier
        # if r < 1/6:
        #     logger.info('Environment: normal short')
        #     self.path.slice(None, 20)
        # elif r < 2/6:
        #     logger.info('Environment: backwards short')
        #     self.path.slice(70, None)
        #     self.path.invert()
        # elif
        if i % 4 is 0:
            logger.info('Environment: normal full')
            pass
        elif i % 4 is 1:
            logger.info('Environment: backwards full')
            self.path.invert()
        elif i % 4 is 2:
            logger.info('Environment: normal turn (left)')
            self.path.slice(30, 60)
        elif i % 4 is 3:
            logger.info('Environment: backwards turn (right)')
            self.path.slice(40, 70)
            self.path.invert()

    def load_path(self, i):
        self.__update_path(i)
        icarla.move(self.vehicle, icarla.transform(self.path.start[0], self.path.start[1], 0.25).location)
        icarla.rotate(self.vehicle, icarla.rotation_from_radian(self.path.direction()))

        self.s.set_knowledge(self.connection.world.get_spectator(), self.vehicle, self.path)
