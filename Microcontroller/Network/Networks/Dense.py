import torch
import torch.nn as nn
import torch.nn.functional as F


class CNNwDense(nn.Module):
    # __________     ______
    # |Features| ==> |DENSE| => Action
    # ----------     ------
    #
    #
    def __init__(self, dim_features, height, width, n_actions):
        # The parameters are the following
        #   dim_features    - the dimension of the feature vector.
        #   height, width   - the parameters of the input picture.
        #   n_actions         - number of actions on the output of the network
        super(CNNwDense, self).__init__()

        self.dense1 = nn.Linear(dim_features, 64)

        # Recursive Neural Network
        self.dense2 = nn.Linear(64, 64)
        self.dense3 = nn.Linear(64, n_actions)
        # Output layer


    # Called with either one element to determine next action, or a batch
    # during optimization. Returns tensor([[left0exp,right0exp]...]).
    def forward(self, image, features):
        y = features.view(features.size(0), features.size(2))
        y = F.leaky_relu(self.dense1(y))
        y=  F.leaky_relu(self.dense2(y))
        return self.dense3(y)
