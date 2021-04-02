import random
import time

from ReinforcementModel import ReinforcementModel
from agents.agent import Agent
from agents.state import State, feature_dimension
from sensors import recently, limit_range
from support.datakey import DataKey
from support.image_manipulation import im_resize, im_grayscale
import numpy as np
import math
from support.logger import logger

feature_dimension = feature_dimension()
AGENT_IM_HEIGHT = 32
AGENT_IM_WIDTH = 32
AGENT_MODEL_PATH = 'files/tensor.pt'
choices = [[0.35, 0.0],
           [0.33, 0.3],
           [0.33, -0.3]]
ACTIONS_NUM = len(choices)


class NetworkAgent(Agent):

    def __init__(self):
        super().__init__()

        self.choices = choices

        self.model = ReinforcementModel(dim_features=feature_dimension, height=AGENT_IM_HEIGHT,
                                        width=AGENT_IM_WIDTH, n_actions=ACTIONS_NUM)

    def predict(self, state):
        if state is None:
            return None, None
        action = self.model.predict(state)
        try:
            # Copy value, not reference
            choice = self.choices[action][:]
            # TODO (7) remove noise of agent out
            choice[1] += -0.05 + random.random() / 10
            return action, choice
        except RuntimeError:
            logger.error(f'Error when trying to find right value for {action}')
            return None, None

    def optimize(self, new_state, prev_state=None, action=None):
        try:
            self.model.optimize(new_state, prev_state, action)
        except RuntimeError as r:
            logger.error(f'Error in model.optimize: {r}')

    def save(self, path=AGENT_MODEL_PATH):
        logger.warning(f'Saving model to {path}')
        self.model.save_model(path)

    def load(self, path=AGENT_MODEL_PATH):
        logger.info(f'Loading model from {path}')
        try:
            self.model.load_model(path, dim_features=feature_dimension, image_height=AGENT_IM_HEIGHT,
                                  image_width=AGENT_IM_WIDTH, n_actions=ACTIONS_NUM)
        except FileNotFoundError as f:
            logger.error(f'Failed to find file at {path} - {f}')
        except RuntimeError as r:
            logger.critical(f'Failed to load agent from {path} - {r}')

    # rotation matrices used in calculating car subjective acceleration
    @staticmethod
    def __Rx(theta):
        return np.matrix([[1, 0, 0],
                          [0, math.cos(theta), -math.sin(theta)],
                          [0, math.sin(theta), math.cos(theta)]])

    @staticmethod
    def __Ry(theta):
        return np.matrix([[math.cos(theta), 0, math.sin(theta)],
                          [0, 1, 0],
                          [-math.sin(theta), 0, math.cos(theta)]])

    @staticmethod
    def __Rz(theta):
        return np.matrix([[math.cos(theta), -math.sin(theta), 0],
                          [math.sin(theta), math.cos(theta), 0],
                          [0, 0, 1]])

    @staticmethod
    def repack(data, line, starting_dir):
        ca = data.get(DataKey.SENSOR_CAMERA)
        r = data.get(DataKey.SENSOR_RADAR)
        co = data.get(DataKey.SENSOR_COLLISION)
        v = data.get(DataKey.SENSOR_VELOCITY)
        a = data.get(DataKey.SENSOR_ACCELERATION)
        p = data.get(DataKey.SENSOR_POSITION)
        di = data.get(DataKey.SENSOR_DIRECTION)
        sdi = starting_dir
        o = data.get(DataKey.SENSOR_OBSTACLE)
        if p is not None:
            d = line.distance([p[0], p[1]])
        else:
            d = None
        return ca, r, co, v, a, p, di, sdi, o, d

    @staticmethod
    def convert(state):
        """Converts incoming data into the format the agent accepts"""
        camera, radar, collision, velocity, acceleration, position, direction, starting_direction, obstacle, distance \
            = state

        # TODO (2) check if all conversions are needed

        # SENSOR: unit, format

        # camera: -, image (as numpy array ?)
        camera = im_resize(camera, (AGENT_IM_HEIGHT, AGENT_IM_WIDTH))
        # Removing colors
        camera = im_grayscale(camera)  # when grayscaling also change shape?
        # Reshaping for NN
        # TODO (2) this does not do anything
        if camera is not None:
            camera = np.reshape(camera, [AGENT_IM_HEIGHT, AGENT_IM_WIDTH])

        # radar: m, (float ?) - Can be None (if no valid measurement has occurred, or if the point is outside our range)
        radar = limit_range(radar)
        if radar is None:
            radar = 100.0  # Radar always some number

        # velocity: m/s, [v_x, v_y, v_z] (floats?)
        # Velocity is now converted to 1D (float)
        if velocity is None:
            velocity = 0.0
        else:
            velocity = (velocity[0] ** 2 + velocity[1] ** 2) ** 0.5

        # direction: degrees? 3D? - but agent only cares about yaw (around y axis - like a compass)
        if direction is None:
            direction = [0, 0, 0]
            current_direction = 0
        else:
            current_direction = (direction[1] - starting_direction[1]) / 180 * np.pi

        # acceleration: m/s2, [a_x, a_y, a_z] (floats?)
        if acceleration is None:
            acceleration = [0, 0, 0]

        acceleration = (NetworkAgent.__Ry(direction[1]) @ NetworkAgent.__Rz(direction[2]) @ NetworkAgent.__Rx(
            direction[0])).T @ np.reshape(acceleration, [3, 1])
        acceleration = np.asarray(acceleration).flatten().tolist()

        # position: m, [x, y, z] (floats?)

        # collision: bool, was collision registered? - None if never, False or True if (not) in the last 1.0* second
        collision = recently(collision)
        if collision is None:
            collision = False  # Collision always TRUE/FALSE

        # obstacle: bool, was obstacle registered? - None if never, False or True if (not) in the last 1.0* second
        obstacle = recently(obstacle)
        if obstacle is None:
            obstacle = False  # Obstacle always TRUE/FALSE

        important = camera, velocity, acceleration, position, current_direction
        # Only return data if important inputs (which should not be None) are not None
        # TODO (5) why prev line of comment?
        # TODO (2) direction is changed if None - does it matter?
        if not any(map(lambda x: x is None, important)):
            return State(image=camera, radar=radar, collision=collision, velocity=velocity, acceleration=acceleration,
                         position=position, direction=current_direction, obstacle=obstacle,
                         distance_from_line=distance)
        else:
            return None
