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

        self.connection = Connection()

    def setup(self):
        logger.info('Environment setup')
        self.connection.connect()
        self.__set_conditions()
        self.__update_path()
        self.__spawn()
        self.reset(do_update_path=False)

    def start(self):
        self.c.start()
        self.p.start()

    def reset(self, do_update_path=True):
        logger.debug('Resetting actors')
        self.clear()
        logger.debug('Halting threads')
        self.data.put(DataKey.THREAD_HALT, True)
        self.vehicle.apply_control(icarla.vehicle_control(throttle=0, steer=0))
        icarla.set_velocity(self.vehicle, icarla.vector3d())

        self.__update_path()

        icarla.move(self.vehicle, icarla.transform(self.path.start[0], self.path.start[1], 0.25).location)
        icarla.rotate(self.vehicle, icarla.rotation_from_radian(self.path.direction()))
        position = icarla.transform(self.path.start[0] + 5 * np.cos(self.path.direction()[1]),
                                    self.path.start[1] + 5 * np.sin(self.path.direction()[1]),
                                    12.0).location
        rotation = icarla.rotation([-85, self.path.direction()[1] / np.pi * 180.0, 0])
        self.__spectator_move_and_rotate(position, rotation)
        logger.info('Environment reset successful')

    def __spectator_move_and_rotate(self, position, direction):
        icarla.move(self.connection.world.get_spectator(),
                    position)

        # Apparently UE4 spectator doesn't like exact 90 degrees, keep it less?
        icarla.rotate(self.connection.world.get_spectator(),
                      direction)

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

        pos = self.data.get(DataKey.SENSOR_POSITION)
        # TODO update rot to use line direction at current position
        rot = self.data.get(DataKey.SENSOR_DIRECTION)
        if pos is not None and rot is not None:
            # position = icarla.transform(pos[0] + 5 * np.cos(rot[1]),
            #                             pos[1] + 5 * np.sin(rot[1]),
            #                             12.0).location
            position = icarla.transform(pos[0],
                                        pos[1],
                                        12.0).location
            rotation = icarla.rotation([-85, rot[1], 0])
            self.__spectator_move_and_rotate(position, rotation)
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
                logger.info('Cleaned up old actors')
                # Setting nice weather
        self.__set_weather()

    def __set_weather(self):
        weather = self.connection.world.get_weather()

        weather.precipitation = 0.0
        weather.precipitation_deposits = 0.0
        weather.wetness = 0.0

        self.connection.world.set_weather(weather)
        logger.info('Applied nice weather')

    def __spawn(self):
        logger.info('Spawning actors, sensors')
        spawn_vehicle(self, self.path.start, self.path.direction())
        spawn_camera(self)
        spawn_radar(self)
        spawn_collision(self)
        spawn_obstacle(self)

    def __update_path(self):
        self.path = get_path()
        # TODO (3) make prettier
        r = random.random()
        # if r < 1/6:
        #     logger.info('Environment: normal short')
        #     self.path.slice(None, 20)
        # elif r < 2/6:
        #     logger.info('Environment: backwards short')
        #     self.path.slice(70, None)
        #     self.path.invert()
        # elif
        if r < 1 / 4:
            logger.info('Environment: normal full')
            pass
        elif r < 2 / 4:
            logger.info('Environment: backwards full')
            self.path.invert()
        elif r < 3 / 4:
            logger.info('Environment: normal turn (left)')
            self.path.slice(30, 60)
        else:
            logger.info('Environment: backwards turn (right)')
            self.path.slice(40, 70)
            self.path.invert()
