import random
import numpy as np
from sensors import recently, limit_range, resize
from support.logger import logger

AGENT_MODEL_PATH = 'files/tensor.pt'


class Agent:

    def reset(self):
        logger.error('RESET - not happening')

    def predict(self, state):
        known_state, hidden_state = state
        camera, radar, velocity, acceleration, position = known_state
        if state is not None:
            if radar is not None and radar < 5.0:
                return random.choice([(-0.5, 0.75), (-0.5, -0.75)])
            elif velocity is not None and abs(np.sum(velocity)) < 0.01:
                return random.choice([(-0.5, 0.75), (-0.5, -0.75)])
            return random.choice([(0.5, 0.0), (0.3, 0.3), (0.3, -0.3)])
        return None

    def optimize(self, new_state):
        known_state, hidden_state = new_state
        camera, radar, velocity, acceleration, position = known_state
        collision, obstacle, distance = hidden_state
        pass

    def save(self, path):
        logger.error('SAVE - not happening')

    def load(self, path):
        logger.error('LOAD - not happening')


def convert(state):
    """Converts incoming data into the format the agent accepts"""
    camera, radar, collision, velocity, acceleration, position, obstacle, distance = state

    # TODO (8) get direction car faces and calc. subjective vectors (for vel., acc.)

    # SENSOR: unit, format

    # camera: -, image (as numpy array ?)
    from agents.networkagent import AGENT_IM_HEIGHT, AGENT_IM_WIDTH
    camera = resize(camera, AGENT_IM_HEIGHT, AGENT_IM_WIDTH)

    # radar: m, (float ?) - Can be None (if no valid measurement has occurred, or if the point is outside our 'range')
    radar = limit_range(radar)

    # velocity: m/s, [v_x, v_y, v_z] (floats?)

    # acceleration: m/s2, [a_x, a_y, a_z] (floats?)

    # position: m, [x, y, z] (floats?)

    # collision: bool, was collision registered? - None if never, False or True if (not) in the last 1.0* second
    collision = recently(collision)

    # obstacle: bool, was obstacle registered? - None if never, False or True if (not) in the last 1.0* second
    obstacle = recently(obstacle)

    known_state = (camera, radar, velocity, acceleration, position)
    hidden_state = (collision, obstacle, distance)
    return known_state, hidden_state
