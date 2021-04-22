# Please don't add your settings to commits, only include this when you enhance this file

import logging
from enum import Enum


class AgentTypes(Enum):  # Implementations of Agent
    Network = 1
    Keras = 2


class NetworkAgentModelTypes(Enum):  # Implementations of NetworkAgents Model
    CNNwDense = 1
    CNNwRNN = 2
    LCNN = 3
    SCNN = 4


class EnvironmentTypes(Enum):  # Implementations of Environment
    CARLA = 1
    Test = 2


# Settings of training
TRAIN = True
TRAIN_PER_DECISION = False
AGENT_TYPE = AgentTypes.Keras
NETWORK_AGENT_MODEL_TYPE = NetworkAgentModelTypes.SCNN
ENVIRONMENT_TYPE = EnvironmentTypes.CARLA
TARGET_FRAME_TIME = 0.25
TRAIN_MEMORY_SIZE = 1024  # 128 + (10 * (1 / TARGET_FRAME_TIME)) // 1


# Determines if the run is 'pure' -> no noise or auto-driving applied to agent output
def pure(run_index):
    return True


# Agent settings
choices = [[0.25, -0.1],
           [0.25, 0.1]]
choices_count = len(choices)

AGENT_IM_HEIGHT = 32
AGENT_IM_WIDTH = 32

# Logging settings
WRITE_FILE = False
LEVEL = logging.INFO

# Environment & status settings
MAP_NAME = 'Town05'
MAX_OFF_DISTANCE = 2.0

# Sensor settings
IM_WIDTH = 400
IM_HEIGHT = 400
RADAR_RANGE = 40.0

# Connection to carla
HOST = 'localhost'
PORT = 2000

# Dashboard settings
TITLE = 'Dashboard'
FONT = 'freesansbold.ttf'
