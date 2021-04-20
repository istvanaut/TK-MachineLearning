import time

import icarla
from settings import RADAR_RANGE
from support.datakey import DataKey
import numpy as np

IM_WIDTH = 400
IM_HEIGHT = 400
RADAR_RANGE = 40.0


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
    i = i.reshape((int(np.sqrt(len(i))//2), int(np.sqrt(len(i))//2), 4))  # actual resizing is done by converter.convert
    i = i[:, :, :3]
    data.put(DataKey.SENSOR_CAMERA, i / 255.0)  # normalization


def process_radar(data, rd):
    points = []
    for detect in rd:

        fw_vec = icarla.vector3d(x=detect.depth - 0.25)

        if abs(detect.azimuth) < 0.001 and abs(detect.altitude) < 0.001:
            points.append(np.linalg.norm([fw_vec.x, fw_vec.y, fw_vec.z]))

    if len(points) is not 0:
        data.put(DataKey.SENSOR_RADAR, points[-1])


def limit_range(radar):
    if radar is not None and radar > RADAR_RANGE:
        return None
    else:
        return radar


def process_coll(data, coll):
    data.put(DataKey.SENSOR_COLLISION, time.time_ns())


def process_obs(data, obs):
    data.put(DataKey.SENSOR_OBSTACLE, time.time_ns())
