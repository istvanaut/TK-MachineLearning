# based on key.py from Tamás Visy's temalab_gta repo

from enum import Enum


class Key(Enum):
    SENSOR_CAMERA = 20
    SENSOR_RADAR = 21
    SENSOR_COLLISION = 25
    SENSOR_OBSTACLE = 26
    SENSOR_ACCELERATION = 30
    SENSOR_VELOCITY = 31
    SENSOR_POSITION = 32
    CONTROL_OUT = 60
