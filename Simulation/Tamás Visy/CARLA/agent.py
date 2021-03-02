import random
import time

from sensors import recently


class Agent:

    def reset(self):
        pass

    def predict(self, current_state):
        camera, radar, collision, velocity, acceleration, position, obstacle = current_state
        if current_state is not None:
            if collision is True:
                return random.choice([(-0.5, 0.5), (-0.5, -0.5)])
            return random.choice([(0.5, 0.0), (0.3, 0.3), (0.3, -0.3)])
        return None

    def optimize(self, new_state):
        camera, radar, collision, velocity, acceleration, position, obstacle = new_state
        pass


def convert(state):
    """Converts incoming data into the format the agent accepts"""
    camera, radar, collision, velocity, acceleration, position, obstacle = state

    # SENSOR: unit, format

    # camera: -, image (as numpy array ?)

    # radar: m, (float ?)

    # velocity: m/s, [v_x, v_y, v_z] (floats?)

    # acceleration: m/s2, [a_x, a_y, a_z] (floats?)

    # position: m, [x, y, z] (floats?)

    # collision: bool, was collision registered? - None if never, False or True if (not) in the last 1.0* second
    collision = recently(collision)

    # obstacle: bool, was obstacle registered? - None if never, False or True if (not) in the last 1.0* second
    obstacle = recently(obstacle)

    state = (camera, radar, collision, velocity, acceleration, position, obstacle)
    return state
