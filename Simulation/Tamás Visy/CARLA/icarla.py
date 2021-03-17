import glob
import os
import sys
import time

from support.logger import logger

try:
    sys.path.append(glob.glob('files/carla-*%d.%d-%s.egg' % (
        sys.version_info.major,
        sys.version_info.minor,
        'win-amd64' if os.name == 'nt' else 'linux-x86_64'))[0])
except IndexError:
    logger.critical('IndexError while trying to find carla egg')
    logger.info('Place carla egg into files directory')
    sys.exit()
try:
    import carla  # Carla package uses the egg from Carla - Python installed package doesn't work
except ModuleNotFoundError:
    logger.critical('ModuleNotFoundError while importing carla from egg')
    sys.exit()


def client(host, port):
    return carla.Client(host, port)


def transform(x=0.0, y=0.0, z=0.0, pitch=0.0, yaw=0.0, roll=0.0):
    return carla.Transform(carla.Location(x=x, y=y, z=z), carla.Rotation(pitch=pitch, yaw=yaw, roll=roll))


def copy(transformation):
    loc = transformation.location
    rot = transformation.rotation
    return carla.Transform(carla.Location(x=loc.x, y=loc.y, z=loc.z),
                           carla.Rotation(pitch=rot.pitch, yaw=rot.yaw, roll=rot.roll))


def location(x=0.0, y=0.0, z=0.0):
    return carla.Location(x=x, y=y, z=z)


def vector3d(x=0.0):
    return carla.Vector3D(x=x)


def vehicle_control(reverse=False, throttle=0.0, steer=0.0):
    return carla.VehicleControl(reverse=reverse, throttle=throttle, steer=steer)


def rotation(p, y, r):
    return carla.Rotation(pitch=p, yaw=y, roll=r)


def move(actor, loc):
    actor.set_location(loc)
    time.sleep(0.5)  # for some reason we must wait for the simulator to process this
    d = actor.get_location().distance(loc)
    if d > 1.0:
        logger.warning(f'Failed moving {actor.type_id} to {loc}, retry...')
        logger.info(f'Actors transform is {actor.get_transform()}')
        actor.set_transform(carla.Transform(addloc(loc, location(0, 0, 1)), rotation(0, 0, 0)))
        time.sleep(0.5)
        d = actor.get_location().distance(loc)
        if d > 3.0:
            logger.error(f'Failed moving {actor.type_id} to {loc}')
            logger.info(f'Actors transform is {actor.get_transform()}')
            return
    logger.info(f'Moved {actor.type_id} to {loc}')


def addloc(loc0, loc1):
    return carla.Location(x=loc0.x + loc1.x, y=loc0.y + loc1.y, z=loc0.z + loc1.z)


def rotate(actor, rot):
    t = actor.get_transform()
    t.rotation = rotation(*rot)
    actor.set_transform(t)
    time.sleep(0.5)  # for some reason we must wait for the simulator to process this
    r1 = actor.get_transform().rotation
    r2 = rotation(*rot)
    diff = ((r1.pitch - r2.pitch) ** 2 + (r1.yaw - r2.yaw) ** 2 + (r1.roll - r2.roll) ** 2) ** 0.5
    if diff > 1.0:
        logger.warning(f'Failed rotating {actor.type_id} to {t.rotation}')
    else:
        logger.info(f'Rotated {actor.type_id} to {t.rotation}')
