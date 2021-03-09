# @title environment
# spawns vehicle, sensors etc. and launches other threads

import time

import carla_wrapper
from line import Line, in_range
from support.datakey import DataKey
from support.logger import logger
from threads.agentthread import AgentThread
from threads.controllerthread import ControllerThread
from threads.dashboardthread import DashboardThread
from support.data import Data
from threads.pollerthread import PollerThread

from sensors import process_image, process_coll, process_radar, process_obs, IM_WIDTH, IM_HEIGHT, recently

VEHICLE_BLUEPRINT_NAME = 'vehicle.volkswagen.t2'

CAMERA_BLUEPRINT_NAME = 'sensor.camera.rgb'  # Is actually an RGBA camera (returns image with 4 channels)
RADAR_BLUEPRINT_NAME = 'sensor.other.radar'
COLL_BLUEPRINT_NAME = 'sensor.other.collision'
OBS_BLUEPRINT_NAME = 'sensor.other.obstacle'
DEFAULT_SPAWN_POINT = carla_wrapper.transform()

SENSOR_SPAWN_POINT = carla_wrapper.transform(x=2.1, z=1.2)

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
        line = Line([191.6866, 296], [191.6866, 112])
        start = [line.start[0], line.start[1], 0.25]
        self.spawn(start, direction=[0, 270, 0])
        self.setup(line)
        self.run(line)

    def task_create_line(self):
        spectator = self.world.get_spectator()
        input('Waiting for input...')
        time.sleep(3.0)
        while True:
            loc = spectator.get_location()
            print(f'[{loc.x},{loc.y}]')
            time.sleep(0.2)

    def setup(self, line):
        logger.info('Environment setup')
        self.move(self.vehicle, carla_wrapper.transform(line.start[0], line.start[1]+5, 200.0).location)
        self.rotate(self.vehicle, [0, 270, 0])
        self.move(self.world.get_spectator(), carla_wrapper.transform(line.start[0], line.start[1], 1.0).location)
        self.rotate(self.world.get_spectator(), [0, 270, 0])

    def move(self, actor, pos):
        actor.set_location(pos)
        d = actor.get_location().distance(pos)
        if d > 1.0:
            logger.warning(f'Failed moving {actor.type_id} to {pos} - d: {d/10//0.1} m')
        else:
            logger.info(f'Moved {actor.type_id} to {pos}')

    def rotate(self, actor, rot):
        t = actor.get_transform()
        t.rotation = carla_wrapper.rotation(*rot)
        actor.set_transform(t)
        logger.info(f'Possibly rotated {actor.type_id} to {t.rotation}')

    def run(self, line):
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

            self.reset(line)

            c.set_vehicle(self.vehicle)
            p.set_vehicle(self.vehicle)

    def test(self, line):
        reason = 'UNKNOWN'
        successful = False
        finished = False
        t = time.time()
        pos = None
        while not finished:
            time.sleep(1.0)

            coll = self.data.get(DataKey.SENSOR_COLLISION)

            pos = self.data.get(DataKey.SENSOR_POSITION)
            pos = [pos[0], pos[1]]
            dist = line.distance(pos)

            if coll is not None:
                finished = True
                reason = 'Collision'
            if dist > 8.0:
                finished = True
                reason = 'Too far from line'
            if time.time() - t > 100:
                finished = True
                reason = 'Time ran out'
            if in_range(pos, line.end):
                finished = True
                successful = True
                reason = 'At finish'
        logger.info(f'Distance from start: {line.distance_from_start(pos)}')
        if successful:
            logger.info(f'Successfully finished - {reason}')
        else:
            logger.info(f'Unsuccessful - {reason}')

    def spawn(self, start, direction):
        self.spawn_vehicle(start, direction)
        self.actors.append(self.vehicle)
        camera = self.spawn_camera()
        self.actors.append(camera)
        radar = self.spawn_radar()
        self.actors.append(radar)
        coll = self.spawn_collision()
        self.actors.append(coll)
        obs = self.spawn_obstacle()
        self.actors.append(obs)

    def set_world(self, world):
        self.world = world

    def get_actors(self):
        return self.actors

    def reset(self, line):
        for a in self.actors:
            a.destroy()
        self.actors = []
        start = [line.start[0], line.start[1], 0.25]
        self.spawn(start, direction=[0, 270, 0])
        self.setup(line)
        self.data.clear()

    def spawn_camera(self):
        bpl = self.world.get_blueprint_library()
        camera_blueprint = bpl.find(CAMERA_BLUEPRINT_NAME)
        camera_blueprint.set_attribute('image_size_x', f'{IM_WIDTH}')
        camera_blueprint.set_attribute('image_size_y', f'{IM_HEIGHT}')
        # camera_blueprint.set_attribute('fov', '90')
        spawn_point = SENSOR_SPAWN_POINT
        camera = self.world.spawn_actor(camera_blueprint, spawn_point, attach_to=self.vehicle)
        camera.listen(lambda i: process_image(self.data, i))
        logger.debug('Camera spawned')
        return camera

    def spawn_obstacle(self):
        bpl = self.world.get_blueprint_library()
        obs_blueprint = bpl.find(OBS_BLUEPRINT_NAME)
        spawn_point = SENSOR_SPAWN_POINT
        obs = self.world.spawn_actor(obs_blueprint, spawn_point, attach_to=self.vehicle)
        obs.listen(lambda o: process_obs(self.data, o))
        logger.debug('ObstacleSensor spawned')
        return obs

    def spawn_collision(self):
        bpl = self.world.get_blueprint_library()
        coll_blueprint = bpl.find(COLL_BLUEPRINT_NAME)
        spawn_point = DEFAULT_SPAWN_POINT
        coll = self.world.spawn_actor(coll_blueprint, spawn_point, attach_to=self.vehicle)
        coll.listen(lambda c: process_coll(self.data, c))
        logger.debug('CollisionSensor spawned')
        return coll

    def spawn_radar(self):
        bpl = self.world.get_blueprint_library()
        radar_blueprint = bpl.find(RADAR_BLUEPRINT_NAME)
        radar_blueprint.set_attribute('horizontal_fov', '1.0')
        radar_blueprint.set_attribute('vertical_fov', '1.0')
        # radar_blueprint.set_attribute(dict)  # TODO (4) can it work with dict?
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
        logger.debug('Radar spawned')
        return radar

    def spawn_vehicle(self, start, direction):
        bpl = self.world.get_blueprint_library()
        vehicle_blueprint = bpl.find(VEHICLE_BLUEPRINT_NAME)
        logger.debug(f'Vehicle spawn is {start}')
        if start is not None:
            spawn_point = carla_wrapper.transform(*start)
            spawn_point.rotation = carla_wrapper.rotation(*direction)
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
        logger.debug(f'Vehicle spawned at {spawn_point}')
