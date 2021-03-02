import glob
import math
import os
import sys
import time

import cv2
from key import Key
import numpy as np

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

IM_WIDTH = 400
IM_HEIGHT = 400


def recently(event_time):
    """Determines whether an event has happened within 1.0 second.\n
    Awaits the time of event in ns since the Epoch.\n
    If the event time is None this function returns None, otherwise it returns a bool."""
    if event_time is None:
        return None
    else:
        event_time /= 1000000000
        now = time.time_ns() / 1000000000
        if now - event_time > 1.0:
            return False
        else:
            return True


def process_image(data, image):
    i = np.array(image.raw_data)
    i = i.reshape((IM_HEIGHT, IM_WIDTH, 4))
    i = i[:, :, :3]
    data.put(Key.SENSOR_CAMERA, i / 255.0)  # normalization


def resize(image, x, y):
    return cv2.resize(image, (x, y))


def process_radar(data, rd):
    points = []
    current_rot = rd.transform.rotation
    for detect in rd:
        azi = math.degrees(detect.azimuth)
        alt = math.degrees(detect.altitude)

        fw_vec = carla.Vector3D(x=detect.depth - 0.25)
        carla.Transform(
            carla.Location(),
            carla.Rotation(
                pitch=current_rot.pitch + alt,
                yaw=current_rot.yaw + azi,
                roll=current_rot.roll)).transform(fw_vec)

        if abs(detect.azimuth) < 0.001 and abs(detect.altitude) < 0.001:
            points.append(np.linalg.norm([fw_vec.x, fw_vec.y, fw_vec.z]))

    if len(points) is not 0:
        # print(points[-1])
        data.put(Key.SENSOR_RADAR, points[-1])


def process_coll(data, coll):
    # print('Collision detected, maybe restart?')
    data.put(Key.SENSOR_COLLISION, time.time_ns())


def process_obs(data, obs):
    data.put(Key.SENSOR_OBSTACLE, time.time_ns())
