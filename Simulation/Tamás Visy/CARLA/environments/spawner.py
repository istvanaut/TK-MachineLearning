import icarla
from sensors import process_image, IM_WIDTH, IM_HEIGHT, process_obs, process_radar, process_coll
from support.logger import logger

VEHICLE_BLUEPRINT_NAME = 'vehicle.volkswagen.t2'

CAMERA_BLUEPRINT_NAME = 'sensor.camera.rgb'  # Is actually an RGBA camera (returns image with 4 channels)
RADAR_BLUEPRINT_NAME = 'sensor.other.radar'
COLL_BLUEPRINT_NAME = 'sensor.other.collision'
OBS_BLUEPRINT_NAME = 'sensor.other.obstacle'
DEFAULT_SPAWN_POINT = icarla.transform()

SENSOR_SPAWN_POINT = icarla.transform(x=2.2, z=1.5)  # don't forget: the transform is mutable, use icarla.copy


def spawn_camera(environment):
    bpl = environment.connection.world.get_blueprint_library()
    camera_blueprint = bpl.find(CAMERA_BLUEPRINT_NAME)
    camera_blueprint.set_attribute('image_size_x', f'{IM_WIDTH}')
    camera_blueprint.set_attribute('image_size_y', f'{IM_HEIGHT}')
    # camera_blueprint.set_attribute('fov', '90')
    spawn_point = icarla.copy(SENSOR_SPAWN_POINT)
    spawn_point.rotation = icarla.rotation([-45, 0, 0])
    camera = environment.connection.world.spawn_actor(camera_blueprint, spawn_point, attach_to=environment.vehicle)
    camera.listen(lambda i: process_image(environment.data, i))
    environment.actors.append(camera)
    logger.debug('Camera spawned')


def spawn_obstacle(environment):
    bpl = environment.connection.world.get_blueprint_library()
    obs_blueprint = bpl.find(OBS_BLUEPRINT_NAME)
    spawn_point = SENSOR_SPAWN_POINT
    obs = environment.connection.world.spawn_actor(obs_blueprint, spawn_point, attach_to=environment.vehicle)
    obs.listen(lambda o: process_obs(environment.data, o))
    environment.actors.append(obs)
    logger.debug('ObstacleSensor spawned')


def spawn_collision(environment):
    bpl = environment.connection.world.get_blueprint_library()
    coll_blueprint = bpl.find(COLL_BLUEPRINT_NAME)
    spawn_point = DEFAULT_SPAWN_POINT
    coll = environment.connection.world.spawn_actor(coll_blueprint, spawn_point, attach_to=environment.vehicle)
    coll.listen(lambda c: process_coll(environment.data, c))
    environment.actors.append(coll)
    logger.debug('CollisionSensor spawned')


def spawn_radar(environment):
    bpl = environment.connection.world.get_blueprint_library()
    radar_blueprint = bpl.find(RADAR_BLUEPRINT_NAME)
    radar_blueprint.set_attribute('horizontal_fov', '1.0')
    radar_blueprint.set_attribute('vertical_fov', '1.0')
    # radar_blueprint.set_attribute(dict)  # TODO (2) try setting options with dict
    # radar.set(
    #     Channels=32,
    #     Range=50,
    #     PointsPerSecond=100000,
    #     RotationFrequency=10,
    #     UpperFovLimit=10,
    #     LowerFovLimit=-30)
    spawn_point = SENSOR_SPAWN_POINT
    radar = environment.connection.world.spawn_actor(radar_blueprint, spawn_point, attach_to=environment.vehicle)
    radar.listen(lambda r: process_radar(environment.data, r))
    environment.actors.append(radar)
    logger.debug('Radar spawned')


def spawn_vehicle(environment, start, direction):
    bpl = environment.connection.world.get_blueprint_library()
    vehicle_blueprint = bpl.find(VEHICLE_BLUEPRINT_NAME)
    logger.debug(f'Vehicle spawn is {start}')
    if start is not None:
        spawn_point = icarla.transform(*start, 0.25)
        spawn_point.rotation = icarla.rotation(direction)
        success = False

        while not success:
            try:
                environment.vehicle = environment.connection.world.spawn_actor(vehicle_blueprint, spawn_point)
                success = True
            except RuntimeError as r:
                logger.error(f'{r} {spawn_point.location}')
                spawn_point.location.z += 0.1
    else:
        logger.debug(f'No spawn set, spawning vehicle at default spawn')
        spawn_point = environment.connection.world.get_map().get_spawn_points()[0]
        environment.vehicle = environment.connection.world.spawn_actor(vehicle_blueprint, spawn_point)
    environment.actors.append(environment.vehicle)
    logger.debug(f'Vehicle spawned at {spawn_point}')
    environment.c.set_vehicle(environment.vehicle)
    environment.p.set_vehicle(environment.vehicle)
    logger.debug(f'Threads vehicle set')
