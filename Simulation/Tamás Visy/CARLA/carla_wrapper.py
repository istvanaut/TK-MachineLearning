import glob
import os
import sys

from support.logger import logger

try:
    sys.path.append(glob.glob('../carla/dist/carla-*%d.%d-%s.egg' % (
        sys.version_info.major,
        sys.version_info.minor,
        'win-amd64' if os.name == 'nt' else 'linux-x86_64'))[0])
except IndexError:
    logger.critical('IndexError while trying to find carla egg')
    sys.exit()
try:
    import carla  # Carla package uses the egg from Carla - Python installed package doesn't work
except ModuleNotFoundError:
    logger.critical('ModuleNotFoundError while importing carla from egg')
    sys.exit()

def client(host, port):
    return carla.Client(host, port)


def transform(x=0.0, y=0.0, z=0.0):
    return carla.Transform(carla.Location(x=x, y=y, z=z))


def vector3D(x=0.0):
    return carla.Vector3D(x=x)


def vehicleControl(reverse=False, throttle=0.0, steer=0.0):
    return carla.VehicleControl(reverse=reverse, throttle=throttle, steer=steer)


def rotation(p, y, r):
    return carla.Rotation(pitch=p, yaw=y, roll=r)
