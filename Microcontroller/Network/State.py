from collections import namedtuple

import torch
import numpy as np
from torchvision import transforms
transforms.ToTensor()
State = namedtuple('State',
                   ('image', 'radar', 'collision', 'velocity', 'acceleration', 'position',
                    'obstacle', 'distance_from_line'))


def transform_state(state):
    state = State(*state)
    image=np.expand_dims(state.image, axis=0)
    state = [[ [state.radar, state.collision, state.velocity[0], state.velocity[1], state.velocity[2],
             state.acceleration[0], state.acceleration[1], state.acceleration[2],
             state.obstacle, state.distance_from_line]]]
    transform=transforms.ToTensor()
    return torch.from_numpy(image).float(), torch.tensor(state)


feature_dimension = 10
