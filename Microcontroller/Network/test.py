import numpy as np
import torch


from ReinforcementModel import ReinforcementModel
from State import feature_dimension

# Dummy data with 0 128*128 picture
temp_state = (np.random.rand(3, 128, 128), 72.333, 0, [0.28, 3.96, 0.001], [0.28, 3.96, 0.001], [0.28, 3.96, 0.001], 0, 2,)

# Create model with feature dimension 10
model=ReinforcementModel(dim_features=feature_dimension, height=128, width=128, n_actions=4)

# Train the model
for i in range(300):
    model.predict(temp_state)
    model.optimize(temp_state)