# based on key.py from Tamás Visy's temalab_gta repo

from enum import Enum


class DataKey(Enum):
    # Numbers can be chosen however you want
    # (but certain groups can be created and found)

    # Meta
    THREAD_HALT = 10
    # Sensors
    SENSOR_CAMERA = 20
    SENSOR_RADAR = 21
    SENSOR_COLLISION = 25
    SENSOR_OBSTACLE = 26
    SENSOR_ANGULAR_ACCELERATION = 29
    SENSOR_ACCELERATION = 30
    SENSOR_VELOCITY = 31
    SENSOR_POSITION = 32
    SENSOR_DIRECTION = 33
    # Outputs
    CONTROL_OUT = 60
