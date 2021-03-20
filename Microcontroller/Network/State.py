from collections import namedtuple

import torch
import numpy as np
from torchvision import transforms

transforms.ToTensor()
State = namedtuple('State',
                   ('image', 'radar', 'collision', 'velocity', 'acceleration', 'position',
                    'obstacle', 'distance_from_line'))


# from agents.networkagent import feature_dimension
# feature_dimension

def transform_state(state):
    state = State(*state)
    image = np.expand_dims(state.image, axis=0)
    state = [[[state.radar, state.collision, state.velocity,
               state.acceleration[0], state.acceleration[1], state.acceleration[2],
               state.obstacle, state.distance_from_line]]]
    return torch.from_numpy(image).float(), torch.tensor(state).float()
