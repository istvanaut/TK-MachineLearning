import random
import numpy as np

from support.logger import logger


class Agent:

    def predict(self, state, pure=True, auto=False):
        if pure is not True:
            logger.warning('Agent can only use pure')
        if auto is True:
            logger.critical('Agent can not auto-drive')
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
        logger.error('OPTIMIZE - not happening')
        pass

    def save(self):
        logger.error('SAVE - not happening')

    def load(self):
        logger.error('LOAD - not happening')
