# @title task
# spawns vehicle, sensors etc. and launches other threads

# TODO (7) change this: task->environment

import glob
import os
import sys
import time

from agentthread import AgentThread
from controllerthread import ControllerThread
from dashboardthread import DashboardThread
from data import Data
from pollerthread import PollerThread

from sensors import process_image, process_coll, process_radar, process_obs, IM_WIDTH, IM_HEIGHT

try:
    sys.path.append(glob.glob('../carla/dist/carla-*%d.%d-%s.egg' % (
        sys.version_info.major,
        sys.version_info.minor,
        'win-amd64' if os.name == 'nt' else 'linux-x86_64'))[0])
except IndexError:
    print('IndexError while trying to find carla egg')
    sys.exit()
try:
    import carla  # Carla package uses the egg from Carla - Python didn't work
except ModuleNotFoundError:
    print('ModuleNotFoundError while importing carla')
    sys.exit()

VEHICLE_BLUEPRINT_NAME = 'vehicle.volkswagen.t2'

CAMERA_BLUEPRINT_NAME = 'sensor.camera.rgb'  # Is actually an RGBA camera (returns image with 4 channels)
SENSOR_SPAWN_POINT = carla.Transform(carla.Location(x=2.1, z=1.2))

# LIDAR_BLUEPRINT_NAME = 'sensor.lidar.ray_cast'
RADAR_BLUEPRINT_NAME = 'sensor.other.radar'

COLL_BLUEPRINT_NAME = 'sensor.other.collision'
OBS_BLUEPRINT_NAME = 'sensor.other.obstacle'
DEFAULT_SPAWN_POINT = carla.Transform(carla.Location())

data = Data()


def do(world, actors):
    print('Doing')

    vehicle = spawn_vehicle(world)
    actors.append(vehicle)

    camera = spawn_camera(world, vehicle)
    actors.append(camera)

    radar = spawn_radar(world, vehicle)
    actors.append(radar)

    coll = spawn_collision(world, vehicle)
    actors.append(coll)

    obs = spawn_obstacle(world, vehicle)
    actors.append(obs)

    a = AgentThread(data)
    c = ControllerThread(data)
    p = PollerThread(data)
    d = DashboardThread(data)
    c.set_vehicle(vehicle)
    p.set_vehicle(vehicle)

    a.start()
    d.start()
    time.sleep(0.5)
    c.start()
    p.start()
    time.sleep(100)


def spawn_camera(world, vehicle):
    bpl = world.get_blueprint_library()
    camera_blueprint = bpl.find(CAMERA_BLUEPRINT_NAME)
    camera_blueprint.set_attribute('image_size_x', f'{IM_WIDTH}')
    camera_blueprint.set_attribute('image_size_y', f'{IM_HEIGHT}')
    # camera_blueprint.set_attribute('fov', '90')
    spawn_point = SENSOR_SPAWN_POINT
    camera = world.spawn_actor(camera_blueprint, spawn_point, attach_to=vehicle)
    camera.listen(lambda i: process_image(data, i))
    print('Camera spawned')
    return camera


def spawn_obstacle(world, vehicle):
    bpl = world.get_blueprint_library()
    obs_blueprint = bpl.find(OBS_BLUEPRINT_NAME)
    spawn_point = SENSOR_SPAWN_POINT
    obs = world.spawn_actor(obs_blueprint, spawn_point, attach_to=vehicle)
    obs.listen(lambda o: process_obs(data, o))
    print('ObstacleSensor spawned')
    return obs


def spawn_collision(world, vehicle):
    bpl = world.get_blueprint_library()
    coll_blueprint = bpl.find(COLL_BLUEPRINT_NAME)
    spawn_point = DEFAULT_SPAWN_POINT
    coll = world.spawn_actor(coll_blueprint, spawn_point, attach_to=vehicle)
    coll.listen(lambda c: process_coll(data, c))
    print('CollisionSensor spawned')
    return coll


def spawn_radar(world, vehicle):
    bpl = world.get_blueprint_library()
    radar_blueprint = bpl.find(RADAR_BLUEPRINT_NAME)
    radar_blueprint.set_attribute('horizontal_fov', '1.0')
    radar_blueprint.set_attribute('vertical_fov', '1.0')
    # radar_blueprint.set_attribute(dict)  # TODO (8) can it work with dict?
    # radar.set(
    #     Channels=32,
    #     Range=50,
    #     PointsPerSecond=100000,
    #     RotationFrequency=10,
    #     UpperFovLimit=10,
    #     LowerFovLimit=-30)
    spawn_point = SENSOR_SPAWN_POINT
    radar = world.spawn_actor(radar_blueprint, spawn_point, attach_to=vehicle)
    radar.listen(lambda r: process_radar(data, r))
    print('Radar spawned')
    return radar


def spawn_vehicle(world):
    bpl = world.get_blueprint_library()
    vehicle_blueprint = bpl.find(VEHICLE_BLUEPRINT_NAME)
    vehicle = world.spawn_actor(vehicle_blueprint, world.get_map().get_spawn_points()[0])
    print('Vehicle spawned')
    return vehicle


if __name__ == '__main__':
    print('Please run main.py instead')
