# Network library

This library was made to make the implementation of reinforcement learning networks easier.
The design allows for easy interchange between network models, while the training remains the same.
As the project goes on, different modules with different learning techniques will be supported, based on the information we have available, and it will be more customizable.

## Reinforcement Learning

This type of learning tries to maximize a reward function. The agent's actions are controlled by a reinforcement algorithm, which decides the agent's action at every time step. These actions depend on the environments and the state of the agent. Each action takes the agent to a new state, and a reward is calculated based on the difference between the two states. Based on the accumulated rewards the reinforcement algorithm is optimized for better maximization of the reward function.

## Deep Q-Network

This type of network is used for reinforcement learning. It is based on the Q-table approach, where a Q-table is used, which has a size of actions times states, and each row corresponds to the score of each action. This is very hard to maintain, so instead of keeping up a Q-table, the values are estimated by a neural network. The output contains indicators, of which action seems to be the best for the agent. The problem with this type of network that it is hard to train from epoch to epoch.

To achieve supervised learning the library implements a memory module. This module stores a list of all previous actions, states and rewards of the agent. When this memory contains enough entries a new optimization begins, where the network is optimized on a batch of past decisions and their rewards. This highly improves the effectiveness and stability of the algorithm.

## User Guide

First, the user has to define a model. After this to train the model, there are two functions to use. One is `predict` which returns with the new action, and the other is `optimize`, which optimizes the network.

```python

import numpy as np
import torch
from ReinforcementModel import ReinforcementModel
from State import feature_dimension

# Create model with feature dimension and 128*128 image
model=ReinforcementModel(dim_features=feature_dimension, height=128, width=128, n_actions=4)

# Train the model
for i in range(EPOCH):
    model.predict(current_state)
    model.optimize(new_state)
```

`Feature dimension` is defined as a global variable, this determines the number of input features of the network.

## Implemented networks

### CNNwRNN (default)

This network is a combination of a convolutional and a recursive network. The upside of this approach is that none of the networks has to be pre-trained, they can be trained with the Q-learning approach. For more info find the implementation [here](https://github.com/theorska/Microcontroller-ANN-group/blob/TD-networkbase/Network/Networks/CNNwRNN.py).

### CNNfRNN (not yet)

This network is very similar to the one above, but the convolutional network has to be pre-trained to do feature extraction. This makes the approach currently less viable. We are trying to find datasets or ways to create datasets.
