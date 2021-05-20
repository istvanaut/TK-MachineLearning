import numpy as np
import torch

from Networks.FlatDense import FlatDense
from Networks.SCNN import SCNN
from ReinforcementModel import ReinforcementModel
from agents.state import State

# Dummy data with 0 128*128 picture



temp_state=State(image=np.random.rand(1, 32, 32), radar=0, collision=0, velocity=0, acceleration=[0, 0, 0], position=[0, 0, 0],
                 direction=0, obstacle=0, side=0, distance_from_path=0)


next_state=State(image=np.random.rand(1, 32, 32), radar=0, collision=0, velocity=0, acceleration=[0, 0, 0], position=[0, 0, 0],
                 direction=0, obstacle=0, side=0, distance_from_path=0)
def feature_dimensionf():
    return 12


# Create model with feature dimension 10
model = ReinforcementModel(dim_features=feature_dimensionf, height=32, width=32, n_actions=4, model=SCNN)

# Train the model
for i in range(300):
    model.predict(temp_state)
    model.optimize(next_state)
    print(i)

model.memory.save()
