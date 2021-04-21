import math

import numpy as np

from settings import AGENT_IM_HEIGHT, AGENT_IM_WIDTH
from agents.state import State
from support.sensors import recently, limit_range
from support.datakey import DataKey
from support.image_manipulation import im_resize, im_grayscale


def __rx(theta):
    return np.matrix([[1, 0, 0],
                      [0, math.cos(theta), -math.sin(theta)],
                      [0, math.sin(theta), math.cos(theta)]])


def __ry(theta):
    return np.matrix([[math.cos(theta), 0, math.sin(theta)],
                      [0, 1, 0],
                      [-math.sin(theta), 0, math.cos(theta)]])


def __rz(theta):
    return np.matrix([[math.cos(theta), -math.sin(theta), 0],
                      [math.sin(theta), math.cos(theta), 0],
                      [0, 0, 1]])


def repack(data, path, starting_dir):
    ca = data.get(DataKey.SENSOR_CAMERA)
    r = data.get(DataKey.SENSOR_RADAR)
    co = data.get(DataKey.SENSOR_COLLISION)
    o = data.get(DataKey.SENSOR_OBSTACLE)
    v = data.get(DataKey.SENSOR_VELOCITY)
    a = data.get(DataKey.SENSOR_ACCELERATION)
    pos = data.get(DataKey.SENSOR_POSITION)
    di = data.get(DataKey.SENSOR_DIRECTION)
    sdi = starting_dir
    path = path
    aa = data.get(DataKey.SENSOR_ANGULAR_ACCELERATION)
    return ca, r, co, o, v, a, pos, di, sdi, path, aa


def convert(state):
    """Converts and normalizes incoming data (into a format the agent accepts)"""
    camera, radar, collision, obstacle, velocity, acceleration, position, direction, starting_direction, path, angular_acceleration \
        = state

    # SENSOR: unit, format
    # -> into format

    # use X_GOOD_VALUE as the value that should be about 0.7 for the agent -> not max, but a large value

    # camera: -, image {ndarray: (400, 400, 3)}
    # -> {ndarray (32, 32)}
    camera = im_resize(camera, (AGENT_IM_HEIGHT, AGENT_IM_WIDTH))
    # Removing colors
    camera = im_grayscale(camera)  # when grayscaling also change shape?
    # Reshaping for NN
    if camera is not None:
        camera = np.reshape(camera, [AGENT_IM_HEIGHT, AGENT_IM_WIDTH])

    # radar: m, {float64} - Can be None (if no valid measurement has occurred, or if the point is outside our range)
    # -> {float}, scaled, normalized
    radar = limit_range(radar)
    RADAR_GOOD_VALUE = 30.0
    if radar is None:
        radar = RADAR_GOOD_VALUE  # Radar always some number
    # Normalize
    # note: radar between [-0.761 or] 0 and 0.761 <- [tanh(-1) and] tanh(1)
    radar = np.tanh(radar / RADAR_GOOD_VALUE)

    # velocity: m/s, {list: 3} - floats
    # -> {float64}, scaled, normalized
    # Velocity is now converted to 1D (float)
    VELOCITY_GOOD_VALUE = 3.0
    if velocity is None:
        velocity = 0.0
    else:
        velocity = (velocity[0] ** 2 + velocity[1] ** 2) ** 0.5
    # Normalize
    velocity = np.tanh(velocity / VELOCITY_GOOD_VALUE)

    # direction: degrees, {list: 3} - floats,
    # -> radian/PI, {float64} (around vertical axis), scaled
    if direction is None:
        direction = [0, 0, 0]
        current_direction = 0
    else:
        current_direction = (direction[1]/180*np.pi - starting_direction[1])
    CURRENT_DIRECTION_MAX = np.pi
    # Scale -> 1 is 180 degrees or PI, -1 is -180 or -PI
    current_direction = current_direction / CURRENT_DIRECTION_MAX

    # acceleration: m/s2, {list: 3} - floats
    # -> {list: 3} - subjective to the car, scaled, normalized
    if acceleration is None:
        acceleration = [0, 0, 0]

    acceleration = (__ry(direction[1]) @ __rz(direction[2]) @ __rx(
        direction[0])).T @ np.reshape(acceleration, [3, 1])
    acceleration = np.asarray(acceleration).flatten().tolist()
    ACCELERATION_EACH_GOOD_VALUE = 2.0
    # Normalize
    acceleration[0] = np.tanh(acceleration[0] / ACCELERATION_EACH_GOOD_VALUE)
    acceleration[1] = np.tanh(acceleration[1] / ACCELERATION_EACH_GOOD_VALUE)
    acceleration[2] = np.tanh(acceleration[2] / ACCELERATION_EACH_GOOD_VALUE)

    if angular_acceleration is None:
        angular_acceleration = [0, 0, 0]

    # position: m, {list: 3} - floats
    # -> {list: 3}, scaled, normalized
    POSITION_EACH_GOOD_VALUE = 200.0
    position_none_holder = False
    if position is None:
        position = [0, 0, 0]
        position_none_holder = None
    else:
        position = position

    distance = path.distance(position[:2])
    if position_none_holder is None:
        distance = None
    side = path.side(position[:2])
    # Normalize
    position[0] = np.tanh(position[0] / POSITION_EACH_GOOD_VALUE)
    position[1] = np.tanh(position[1] / POSITION_EACH_GOOD_VALUE)
    position[2] = np.tanh(position[2] / POSITION_EACH_GOOD_VALUE)

    # collision: bool, was collision registered? - None if never, False or True if (not) in the last 1.0* second
    collision = recently(collision)
    if collision is None:
        collision = False  # Collision always TRUE/FALSE
    # "Normalize"
    if collision is True:
        collision = np.tanh(1.0)
    else:
        collision = np.tanh(0.0)

    # obstacle: bool, was obstacle registered? - None if never, False or True if (not) in the last 1.0* second
    obstacle = recently(obstacle)
    if obstacle is None:
        obstacle = False  # Obstacle always TRUE/FALSE
    # "Normalize"
    if obstacle is True:
        obstacle = np.tanh(1.0)
    else:
        obstacle = np.tanh(0.0)

    # Only return data if important inputs (which should not be None) are not None
    # Because NN cannot accept "None" as any input
    important = camera, velocity, acceleration, position_none_holder, current_direction, distance, angular_acceleration,
    if not any(map(lambda x: x is None, important)):
        return State(image=camera, radar=radar, collision=collision, velocity=velocity, acceleration=acceleration,
                     position=position, direction=current_direction, obstacle=obstacle,
                     distance_from_path=distance, side=side)
    else:
        return None
