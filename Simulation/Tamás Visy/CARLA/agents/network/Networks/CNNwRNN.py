import torch
import torch.nn as nn
import torch.nn.functional as F
import numpy as np

class CNNwRNN(nn.Module):
    # THis network is a convolutional network and a parallel recursive network.
    # The outputs of the networks are concatenated to make the model.
    # Currently it uses a 11, 9, 7, 5, 3 five layer convolution and
    # Encoder from the transformer network
    # __________     _____   ________
    # |Image   | ==> |CNN| =>|      |
    # ----------     -----   |      |
    #                        |Output| => Action
    # __________     _____   |Layer |
    # |Features| ==> |RNN| =>|      |
    # ----------     -----   ---------
    #
    #
    def __init__(self, dim_features, height, width, n_actions):
        # The parameters are the following
        #   dim_features    - the dimension of the feature vector.
        #   height, width   - the parameters of the input picture.
        #   n_actions         - number of actions on the output of the network
        super(CNNwRNN, self).__init__()

        # Convolutional Network
        self.conv1 = nn.Conv2d(3, 16, kernel_size=11, stride=1)
        self.bn1 = nn.BatchNorm2d(16)
        self.conv2 = nn.Conv2d(16, 32, kernel_size=9, stride=1)
        self.bn2 = nn.BatchNorm2d(32)
        self.conv3 = nn.Conv2d(32, 32, kernel_size=7, stride=1)
        self.bn3 = nn.BatchNorm2d(32)
        self.conv4 = nn.Conv2d(32, 32, kernel_size=5, stride=1)
        self.bn4 = nn.BatchNorm2d(32)
        self.conv5 = nn.Conv2d(32, 32, kernel_size=3, stride=1)
        self.bn5 = nn.BatchNorm2d(32)

        # Number of Linear input connections depends on output of conv2d layers
        # and therefore the input image size, so compute it.
        def conv2d_size_out(size, kernel_size, stride=1):
            return (size - (kernel_size - 1) - 1) // stride + 1

        convw = conv2d_size_out(conv2d_size_out(conv2d_size_out(conv2d_size_out(conv2d_size_out(width, 11), 9), 7), 5),
                                3)
        convh = conv2d_size_out(conv2d_size_out(conv2d_size_out(conv2d_size_out(conv2d_size_out(height, 11), 9), 7), 5),
                                3)
        linear_input_size = convw * convh * 32
        self.dense1 = nn.Linear(linear_input_size, 64)

        # Recursive Neural Network
        self.encoder_layer = nn.TransformerEncoderLayer(d_model=10, nhead=1)
        self.transformer_encoder = nn.TransformerEncoder(self.encoder_layer, 32)

        # Output layer
        self.dense2 = nn.Linear(64+dim_features, 32)
        self.dense3 = nn.Linear(32, n_actions)

    # Called with either one element to determine next action, or a batch
    # during optimization. Returns tensor([[left0exp,right0exp]...]).
    def forward(self,  image, features):
        x = F.leaky_relu(self.bn1(self.conv1(image)))
        x = F.leaky_relu(self.bn2(self.conv2(x)))
        x = F.leaky_relu(self.bn3(self.conv3(x)))
        x = F.leaky_relu(self.bn4(self.conv4(x)))
        x = F.leaky_relu(self.bn5(self.conv5(x)))
        x = F.leaky_relu(self.dense1(x.view(x.size(0), -1)))
        y = features.view(features.size(1),features.size(0), features.size(2))
        y = self.transformer_encoder(y)
        y = y.view(y.size(1), y.size(2))
        x = torch.cat((x, y), dim=1)
        x = self.dense2(x)
        return self.dense3(x)
