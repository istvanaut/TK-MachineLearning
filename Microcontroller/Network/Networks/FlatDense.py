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
        self.device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
        self.height = height
        self.width = width
        self.dim_features = 0
        self.n_actions = n_actions

        # Input layer
        self.dense1 = nn.Linear(self.height * self.width, 6)
        # Hidden layers
        self.dense2 = nn.Linear(6, 32)
        self.dense3 = nn.Linear(32, self.n_actions)
        # Output layer

        self.dropout1 = nn.Dropout(p=0.2)
        self.dropout2 = nn.Dropout(p=0.2)
    # Called with either one element to determine next action, or a batch
    # during optimization. Returns tensor([[left0exp,right0exp]...]).
    def forward(self, image, features):
        y = torch.flatten(image, start_dim=1)
        y = self.dropout1(F.leaky_relu(self.dense1(y)))
        y = self.dropout2(F.leaky_relu(self.dense2(y)))
        return self.dense3(y)

    def proxy_input(self):
        image = torch.randn(1, self.height * self.width).to(self.device)
        feature = torch.tensor([self.dim_features]).to(self.device)
        return image, feature
