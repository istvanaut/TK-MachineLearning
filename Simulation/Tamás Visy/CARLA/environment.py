# @title environment
# spawns vehicle, sensors etc. and launches other threads

import time

import icarla
from line import Line, distance
from support.datakey import DataKey
from support.logger import logger
from threads.agentthread import AgentThread
from threads.controllerthread import ControllerThread
from threads.dashboardthread import DashboardThread
from support.data import Data
from threads.pollerthread import PollerThread

from sensors import process_image, process_coll, process_radar, process_obs, IM_WIDTH, IM_HEIGHT

VEHICLE_BLUEPRINT_NAME = 'vehicle.volkswagen.t2'

CAMERA_BLUEPRINT_NAME = 'sensor.camera.rgb'  # Is actually an RGBA camera (returns image with 4 channels)
RADAR_BLUEPRINT_NAME = 'sensor.other.radar'
COLL_BLUEPRINT_NAME = 'sensor.other.collision'
OBS_BLUEPRINT_NAME = 'sensor.other.obstacle'
DEFAULT_SPAWN_POINT = icarla.transform()

SENSOR_SPAWN_POINT = icarla.transform(x=2.0, z=1.4)

TASK_FOLLOW_LINE = 1
TASK_CREATE_LINE = 2


class Environment:

    def __init__(self):
        self.actors = []
        self.world = None
        self.data = Data()
        self.vehicle = None

    def start(self, task):
        if task is TASK_FOLLOW_LINE:
            self.task_follow_line()
        elif task is TASK_CREATE_LINE:
            self.task_create_line()
        else:
            raise RuntimeError(f'Task {task} not recognized')

    def task_follow_line(self):
        # TODO (7) create option to load line from file or similar
        line = Line([191.6866, 296], [191.6866, 112])
        start = [line.start[0], line.start[1], 0.25]
        self.spawn(start, line.direction())
        self.setup(line)
        self.run(line)

    def task_create_line(self):
        spectator = self.world.get_spectator()
        input('Waiting for input...')
        logger.info('Received input, starting in 3 seconds')
        time.sleep(1.0)
        logger.info('Starting in 2 seconds...')
        time.sleep(1.0)
        logger.info('Starting in 1 seconds...')
        time.sleep(1.0)
        while True:
            # TODO (5) update line calculation
            loc = spectator.get_location()
            print(f'[{loc.x},{loc.y}]')
            time.sleep(0.2)

    def setup(self, line):
        logger.info('Environment setup')
        icarla.move(self.vehicle, icarla.transform(line.start[0], line.start[1], 0.25).location)
        icarla.rotate(self.vehicle, [0, -90, 0])
        icarla.move(self.world.get_spectator(), icarla.transform(line.start[0], line.start[1] + 5, 3.0).location)
        icarla.rotate(self.world.get_spectator(), [-15, -90, 0])

    def run(self, line):
        logger.warning('Halting threads')
        self.data.put(DataKey.THREAD_HALT, True)
        a = AgentThread(self.data)
        c = ControllerThread(self.data)
        p = PollerThread(self.data)
        d = DashboardThread(self.data)

        a.set_line(line)
        c.set_vehicle(self.vehicle)
        p.set_vehicle(self.vehicle)
        a.start()
        d.start()
        c.start()
        p.start()

        while True:
            self.test(line)

            self.reset()

            self.setup(line)

    def test(self, line):
        reason = 'UNKNOWN'
        successful = False
        finished = False
        t = time.time()
        pos = None
        logger.debug('Clearing data')
        self.data.clear()
        while not finished:
            time.sleep(1.0)

            coll = self.data.get(DataKey.SENSOR_COLLISION)

            pos = self.data.get(DataKey.SENSOR_POSITION)
            if pos is not None:
                pos = [pos[0], pos[1]]
                dist = line.distance(pos)
            else:
                pos = None
                dist = -1.0

            if coll is not None:
                finished = True
                reason = 'Collision'
            if dist > 8.0:
                finished = True
                reason = 'Too far from line'
            if time.time() - t > 100:
                finished = True
                reason = 'Time ran out'
            if distance(pos, line.end) < 1.0:
                finished = True
                successful = True
                reason = 'At finish'
        logger.info(f'Distance from start: {distance(line.start, pos)}')
        if successful:
            logger.info(f'Successfully finished - {reason}')
        else:
            logger.info(f'Unsuccessful - {reason}')

    def spawn(self, start, direction):
        self.spawn_vehicle(start, direction)
        self.spawn_camera()
        self.spawn_radar()
        self.spawn_collision()
        self.spawn_obstacle()

    def set_world(self, world):
        self.world = world

    def get_actors(self):
        return self.actors

    def reset(self):
        logger.debug('Clearing data')
        self.data.clear()
        logger.warning('Halting threads')
        self.data.put(DataKey.THREAD_HALT, True)
        logger.info('Sleeping...')
        time.sleep(1.0)
        logger.info('Environment reset successful')

    def spawn_camera(self):
        bpl = self.world.get_blueprint_library()
        camera_blueprint = bpl.find(CAMERA_BLUEPRINT_NAME)
        camera_blueprint.set_attribute('image_size_x', f'{IM_WIDTH}')
        camera_blueprint.set_attribute('image_size_y', f'{IM_HEIGHT}')
        # camera_blueprint.set_attribute('fov', '90')
        spawn_point = SENSOR_SPAWN_POINT
        camera = self.world.spawn_actor(camera_blueprint, spawn_point, attach_to=self.vehicle)
        camera.listen(lambda i: process_image(self.data, i))
        self.actors.append(camera)
        logger.debug('Camera spawned')

    def spawn_obstacle(self):
        bpl = self.world.get_blueprint_library()
        obs_blueprint = bpl.find(OBS_BLUEPRINT_NAME)
        spawn_point = SENSOR_SPAWN_POINT
        obs = self.world.spawn_actor(obs_blueprint, spawn_point, attach_to=self.vehicle)
        obs.listen(lambda o: process_obs(self.data, o))
        self.actors.append(obs)
        logger.debug('ObstacleSensor spawned')

    def spawn_collision(self):
        bpl = self.world.get_blueprint_library()
        coll_blueprint = bpl.find(COLL_BLUEPRINT_NAME)
        spawn_point = DEFAULT_SPAWN_POINT
        coll = self.world.spawn_actor(coll_blueprint, spawn_point, attach_to=self.vehicle)
        coll.listen(lambda c: process_coll(self.data, c))
        self.actors.append(coll)
        logger.debug('CollisionSensor spawned')

    def spawn_radar(self):
        bpl = self.world.get_blueprint_library()
        radar_blueprint = bpl.find(RADAR_BLUEPRINT_NAME)
        radar_blueprint.set_attribute('horizontal_fov', '1.0')
        radar_blueprint.set_attribute('vertical_fov', '1.0')
        # radar_blueprint.set_attribute(dict)  # TODO (2) can it work with dict?
        # radar.set(
        #     Channels=32,
        #     Range=50,
        #     PointsPerSecond=100000,
        #     RotationFrequency=10,
        #     UpperFovLimit=10,
        #     LowerFovLimit=-30)
        spawn_point = SENSOR_SPAWN_POINT
        radar = self.world.spawn_actor(radar_blueprint, spawn_point, attach_to=self.vehicle)
        radar.listen(lambda r: process_radar(self.data, r))
        self.actors.append(radar)
        logger.debug('Radar spawned')

    def spawn_vehicle(self, start, direction):
        bpl = self.world.get_blueprint_library()
        vehicle_blueprint = bpl.find(VEHICLE_BLUEPRINT_NAME)
        logger.debug(f'Vehicle spawn is {start}')
        if start is not None:
            spawn_point = icarla.transform(*start)
            spawn_point.rotation = icarla.rotation(*direction)
            success = False

            while not success:
                try:
                    spawn_point.location.z += 0.1
                    self.vehicle = self.world.spawn_actor(vehicle_blueprint, spawn_point)
                    success = True
                except RuntimeError as r:
                    logger.error(f'{r} {spawn_point.location}')
        else:
            logger.debug(f'No spawn set, spawning vehicle at default spawn')
            spawn_point = self.world.get_map().get_spawn_points()[0]
            self.vehicle = self.world.spawn_actor(vehicle_blueprint, spawn_point)
        self.actors.append(self.vehicle)
        logger.debug(f'Vehicle spawned at {spawn_point}')
