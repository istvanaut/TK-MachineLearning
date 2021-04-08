import math

import numpy as np

from ReinforcementModel import AGENT_IM_HEIGHT, AGENT_IM_WIDTH
from sensors import limit_range, recently
from support.datakey import DataKey
from support.image_manipulation import im_resize, im_grayscale


def feature_dimension():
    return 9


class State:

    def __init__(self, image=None, radar=None, collision=None, velocity=None, acceleration=None, position=None,
                 direction=None, obstacle=None, distance_from_line=None, side=None):
        self.image = image
        self.radar = radar
        self.collision = collision
        self.velocity = velocity
        self.acceleration = acceleration
        self.position = position
        self.direction = direction
        self.obstacle = obstacle
        self.distance_from_line = distance_from_line
        self.side = side

    def print(self):
        if self.acceleration is None:
            acc = [None, None, None]
        else:
            acc = self.acceleration
        print([self.radar, self.collision, self.velocity, acc[0], acc[1], acc[2],
               self.direction, self.obstacle, self.distance_from_line*self.side])

    # TODO (4) refactor to use DataKeys (rename them to Keys)?
    def get_formatted(self):
        if self.acceleration is None:
            acc = [None, None, None]
        else:
            acc = self.acceleration
        if self.position is None:
            pos = [None, None, None]
        else:
            pos = self.position
        # TODO (5) update to self.image.__name__ or something similar
        return self.image, \
               [self.radar, self.collision, self.velocity, acc[0], acc[1], acc[2], pos[0], pos[1], pos[2],
                self.direction, self.obstacle, self.distance_from_line, self.side], \
               ['radar', 'collision', 'velocity', 'accX', 'accY', 'accZ', 'posX', 'posY', 'posZ',
                'direction', 'obstacle', 'distance_from_line', 'side']


# rotation matrices used in calculating car subjective acceleration
def __Rx(theta):
    return np.matrix([[1, 0, 0],
                      [0, math.cos(theta), -math.sin(theta)],
                      [0, math.sin(theta), math.cos(theta)]])


def __Ry(theta):
    return np.matrix([[math.cos(theta), 0, math.sin(theta)],
                      [0, 1, 0],
                      [-math.sin(theta), 0, math.cos(theta)]])


def __Rz(theta):
    return np.matrix([[math.cos(theta), -math.sin(theta), 0],
                      [math.sin(theta), math.cos(theta), 0],
                      [0, 0, 1]])


def repack(data, line, starting_dir):
    ca = data.get(DataKey.SENSOR_CAMERA)
    r = data.get(DataKey.SENSOR_RADAR)
    co = data.get(DataKey.SENSOR_COLLISION)
    o = data.get(DataKey.SENSOR_OBSTACLE)
    v = data.get(DataKey.SENSOR_VELOCITY)
    a = data.get(DataKey.SENSOR_ACCELERATION)
    p = data.get(DataKey.SENSOR_POSITION)
    di = data.get(DataKey.SENSOR_DIRECTION)
    sdi = starting_dir
    li = line
    return ca, r, co, o, v, a, p, di, sdi, li


def convert(state):
    """Converts and normalizes incoming data (into a format the agent accepts)"""
    camera, radar, collision, obstacle, velocity, acceleration, position, direction, starting_direction, line \
        = state

    # TODO (7) refactor this mess

    # TODO (2) check if all conversions are needed

    # SENSOR: unit, format

    # camera: -, image (as numpy array ?)
    camera = im_resize(camera, (AGENT_IM_HEIGHT, AGENT_IM_WIDTH))
    # Removing colors
    camera = im_grayscale(camera)  # when grayscaling also change shape?
    # Reshaping for NN
    # TODO (2) this does not do anything?
    if camera is not None:
        camera = np.reshape(camera, [AGENT_IM_HEIGHT, AGENT_IM_WIDTH])

    # radar: m, (float ?) - Can be None (if no valid measurement has occurred, or if the point is outside our range)
    radar = limit_range(radar)
    RADAR_MAX = 100.0
    if radar is None:
        radar = RADAR_MAX  # Radar always some number
    # Normalize
    radar = np.tanh(radar / RADAR_MAX)  # TODO (3) actually not in range(-1,1) but .76 bc of tanh(1)=0.76 - do we care?

    # velocity: m/s, [v_x, v_y, v_z] (floats?)
    # Velocity is now converted to 1D (float)
    VELOCITY_MAX = 5.0
    if velocity is None:
        velocity = 0.0
    else:
        velocity = (velocity[0] ** 2 + velocity[1] ** 2) ** 0.5
    # Normalize
    velocity = np.tanh(velocity / VELOCITY_MAX)

    # direction: in degrees out radians? 3D? - but agent only cares about yaw (around y axis - like a compass)
    if direction is None:
        direction = [0, 0, 0]
        current_direction = 0
    else:
        current_direction = (direction[1]/ 180 * np.pi - starting_direction[1])  # / 180 * np.pi
    CURRENT_DIRECTION_MAX = np.pi
    # Normalize
    current_direction = np.tanh(current_direction / CURRENT_DIRECTION_MAX)

    # acceleration: m/s2, [a_x, a_y, a_z] (floats?)
    if acceleration is None:
        acceleration = [0, 0, 0]

    acceleration = (__Ry(direction[1]) @ __Rz(direction[2]) @ __Rx(
        direction[0])).T @ np.reshape(acceleration, [3, 1])
    acceleration = np.asarray(acceleration).flatten().tolist()
    ACCELERATION_EACH_MAX = 10.0
    # Normalize
    # TODO (3) make prettier
    acceleration[0] = np.tanh(acceleration[0] / ACCELERATION_EACH_MAX)
    acceleration[1] = np.tanh(acceleration[1] / ACCELERATION_EACH_MAX)
    acceleration[2] = np.tanh(acceleration[2] / ACCELERATION_EACH_MAX)

    # position: m, [x, y, z] (floats?)
    POSITION_EACH_MAX = 200.0
    position_none_holder = False
    if position is None:
        position = [0, 0, 0]
        position_none_holder = None
    else:
        # TODO (3) working workaround - but note that converting here changes values in data bc of references?
        #  Actually this is working as intended, but architecture is weird
        #  Window wants to show accurate inputs of agent, but also objective data...
        position = position

    distance = line.distance(position[:2])
    if position_none_holder is None:
        distance = None
    # TODO (5) side calc is broken?
    side = line.side(position[:2])
    # Normalize
    # TODO (3) make prettier
    position[0] = np.tanh(position[0] / POSITION_EACH_MAX)
    position[1] = np.tanh(position[1] / POSITION_EACH_MAX)
    position[2] = np.tanh(position[2] / POSITION_EACH_MAX)

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

    # Position is actually not needed, but position none holder
    important = camera, velocity, acceleration, position, position_none_holder, current_direction, distance,
    # Only return data if important inputs (which should not be None) are not None
    # Because None can't be the input of the NN
    # TODO (5) test normalize of each input
    # TODO (2) direction is changed if None - does it matter?
    if not any(map(lambda x: x is None, important)):
        return State(image=camera, radar=radar, collision=collision, velocity=velocity, acceleration=acceleration,
                     position=position, direction=current_direction, obstacle=obstacle,
                     distance_from_line=distance, side=side)
    else:
        return None
