from agents.agent import Agent
from settings import choices
from support.logger import logger


class OmniscientAgent(Agent):
    def predict(self, state):
        if state.side is -1:
            return 0, choices[0]
        elif state.side is 1:
            return 1, choices[1]
        else:
            logger.warning('OmniscientAgent is confused!')
            return None, None

    def optimize(self, new_state):
        logger.error('OmniscientAgent does not optimize')

    def save(self):
        logger.error('OmniscientAgent does not save')

    def load(self):
        logger.error('OmniscientAgent does not load')
