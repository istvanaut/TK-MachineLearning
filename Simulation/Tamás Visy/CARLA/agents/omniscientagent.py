import numpy as np

from agents.agent import Agent
from settings import choices
from support.logger import logger


class OmniscientAgent(Agent):
    def predict(self, state):
        if state is None:
            return None, None

        if np.sign(state.side) < 0:
            return 1, choices[1]
        elif np.sign(state.side) > 0:
            return 0, choices[0]
        else:
            logger.warning('OmniscientAgent is confused!')
            return 0, choices[0]

    def optimize(self, new_state):
        logger.error('OmniscientAgent does not optimize')

    def save(self):
        logger.error('OmniscientAgent does not save')

    def load(self):
        logger.error('OmniscientAgent does not load')
