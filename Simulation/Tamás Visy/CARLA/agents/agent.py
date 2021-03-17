import random
import numpy as np

from sensors import recently, limit_range
from support.image_manipulation import im_resize, im_grayscale
from support.logger import logger


class Agent:

    def reset(self):
        logger.error('RESET - not happening')

    def predict(self, state):
        # known_state, hidden_state = state
        # camera, radar, velocity, acceleration, position = known_state
        camera, radar, collision, velocity, acceleration, position, obstacle, distance = state
        if state is not None:
            if radar is not None and radar < 5.0:
                return random.choice([(-0.5, 0.75), (-0.5, -0.75)])
            elif velocity is not None and abs(np.sum(velocity)) < 0.01:
                return random.choice([(-0.5, 0.75), (-0.5, -0.75)])
            return random.choice([(0.5, 0.0), (0.3, 0.3), (0.3, -0.3)])
        return None

    def optimize(self, new_state):
        # known_state, hidden_state = new_state
        # camera, radar, velocity, acceleration, position = known_state
        # collision, obstacle, distance = hidden_state
        camera, radar, collision, velocity, acceleration, position, obstacle, distance = new_state
        pass

    def save(self, path):
        logger.error('SAVE - not happening')

    def load(self, path):
        logger.error('LOAD - not happening')


def convert(state):
    """Converts incoming data into the format the agent accepts"""
    camera, radar, collision, velocity, acceleration, position, direction, obstacle, distance = state

    # TODO (2) check if all conversions are needed

    # SENSOR: unit, format

    # camera: -, image (as numpy array ?)
    from agents.networkagent import AGENT_IM_HEIGHT, AGENT_IM_WIDTH
    camera = im_resize(camera, (AGENT_IM_HEIGHT, AGENT_IM_WIDTH))
    # Removing colors
    # camera = im_grayscale(camera)  # when grayscaling also change shape?
    # Reshaping for NN
    if camera is not None:
        camera = np.reshape(camera, [3, AGENT_IM_HEIGHT, AGENT_IM_WIDTH])

    # radar: m, (float ?) - Can be None (if no valid measurement has occurred, or if the point is outside our 'range')
    radar = limit_range(radar)
    if radar is None:
        radar = 100.0  # Radar always some number

    # velocity: m/s, [v_x, v_y, v_z] (floats?)
    # TODO (6) can't convert velocity to 1D because it breaks the NN - even if State is updated properly - why?
    # Velocity is converted to 1D (float)
    # if velocity is None:
    #     velocity = 0.0
    # else:
    #     velocity = (velocity[0]**2+velocity[1]**2)**0.5

    # acceleration: m/s2, [a_x, a_y, a_z] (floats?)
    # TODO (8) get direction car faces and calc. "subjective" vector for acceleration
    acceleration = acceleration
    # use direction to calc this (dir: degrees, [pitch, yaw, roll])

    # position: m, [x, y, z, pitch, yaw, roll] (floats?)

    # collision: bool, was collision registered? - None if never, False or True if (not) in the last 1.0* second
    collision = recently(collision)
    if collision is None:
        collision = False  # Collision always TRUE/FALSE

    # obstacle: bool, was obstacle registered? - None if never, False or True if (not) in the last 1.0* second
    obstacle = recently(obstacle)
    if obstacle is None:
        obstacle = False  # Obstacle always TRUE/FALSE

    important = camera, velocity, acceleration, position
    if not any(map(lambda x: x is None, important)):
        # Order is important for:
        # from State import State
        # State
        return camera, radar, collision, velocity, acceleration, position, obstacle, distance
    else:
        return None
