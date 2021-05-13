import torch
import torch.nn as nn
import torch.nn.functional as F


class FlatDense(nn.Module):
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
        super(FlatDense, self).__init__()

        self.dense1 = nn.Linear(height * width, 32)

        # Recursive Neural Network
        self.dense2 = nn.Linear(32, 64)
        self.dense3 = nn.Linear(64, 64)
        self.dense4 = nn.Linear(64, n_actions)
        # Output layer

        self.dropout = nn.Dropout(p=0.2)

    # Called with either one element to determine next action, or a batch
    # during optimization. Returns tensor([[left0exp,right0exp]...]).
    def forward(self, image, features):
        y = torch.flatten(image, start_dim=1)
        y = self.dropout(F.leaky_relu(self.dense1(y)))
        y = self.dropout(F.leaky_relu(self.dense2(y)))
        y = self.dropout(F.leaky_relu(self.dense3(y)))
        return self.dense4(y)
