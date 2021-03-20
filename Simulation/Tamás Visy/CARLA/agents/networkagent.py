from ReinforcementModel import ReinforcementModel
from agents.agent import Agent

from support.logger import logger

feature_dimension = 8
AGENT_IM_HEIGHT = 128
AGENT_IM_WIDTH = 128
ACTIONS_NUM = 4
AGENT_MODEL_PATH = 'files/tensor.pt'


class NetworkAgent(Agent):
    def __init__(self):
        super().__init__()
        self.model = ReinforcementModel(dim_features=feature_dimension, height=AGENT_IM_HEIGHT,
                                        width=AGENT_IM_WIDTH, n_actions=ACTIONS_NUM)
        self.model.summary()

    def predict(self, state):
        p = self.model.predict(state)
        if p is 0:
            return 0.4, -0.4
        if p is 1:
            return 0.6, -0.1
        if p is 2:
            return 0.6, 0.1
        if p is 3:
            return 0.4, 0.4
        logger.error(f'Could not find right value for {p}')
        return 0.25, 0

    def optimize(self, new_state):
        try:
            self.model.optimize(new_state)
        except RuntimeError as r:
            logger.error(f'Error in model.optimize: {r}')

    def save(self, path):
        logger.info(f'Saving model to {path}')
        self.model.save_model(path)

    def load(self, path):
        logger.info(f'Loading model from {path}')
        try:
            self.model.load_model(path, dim_features=feature_dimension, image_height=AGENT_IM_HEIGHT,
                                  image_width=AGENT_IM_WIDTH, n_actions=ACTIONS_NUM)
        except FileNotFoundError as f:
            logger.error(f'Failed to find file at {path} - {f}')
        except RuntimeError as r:
            logger.critical(f'Failed to load agent from {path} - {r}')
