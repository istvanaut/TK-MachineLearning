import time

import torch
import numpy as np

from Microcontroller.Network.Networks.FlatDense import FlatDense
from Microcontroller.Network.ReinforcementModel import ReinforcementModel


class ConnectionTrainer:
    def __init__(self, model, **kwargs):
        self.device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
        self.Reinforcement_model = model
        if kwargs.get('path') is not None:
            self.Reinforcement_model.load_model(kwargs.get('path'), self.get_feature_dimension(), 32, 2, 4,
                                                model=FlatDense)

    def get_feature_dimension(self):
        return 10

    def storeData(self, prev_lightSensor,
                  prev_leftUltrasonicSensor,
                  prev_rightUltrasonicSensor,
                  prev_laserDistance,
                  prev_image,
                  prev_leftMotorValue,
                  prev_right_motorValue,
                  reward,
                  curr_lightSensor,
                  curr_leftUltrasonicSensor,
                  curr_rightUltrasonicSensor,
                  curr_laserDistance,
                  curr_image,
                  **kwargs):
        # lightSensor_Previous
        # left_UltrasonicSensor_Previous
        # right_UltrasonicSensor_Previous
        # laserDistance_Previous
        # image_Previous
        # left_MotorValue_Previous
        # right_MotorValue_Previous
        # rewards
        # image_Current
        # lightSensor_Current
        # left_UltrasonicSensor_Current
        # right_UltrasonicSensor_Current
        # laserDistance_Current

        prev_features = torch.tensor([prev_lightSensor, prev_leftUltrasonicSensor, prev_rightUltrasonicSensor,
                                      prev_laserDistance])
        new_features = torch.tensor([curr_lightSensor, curr_leftUltrasonicSensor, curr_rightUltrasonicSensor,
                                     curr_laserDistance])
        if (prev_leftMotorValue[0] > 0.1 and prev_right_motorValue[0] > 0.1):
            action = 2
        if (prev_leftMotorValue[0] > 0.1 and prev_right_motorValue[0] < 0.1):
            action = 0
        if (prev_leftMotorValue[0] < 0.1 and prev_right_motorValue[0] > 0.1):
            action = 1
        if (prev_leftMotorValue[0] < 0.1 and prev_right_motorValue[0] < 0.1):
            action = 3
        action = torch.tensor([[action]])

        prev_image = np.array(prev_image).reshape(1, 32, 32)

        current_image = np.array(curr_image).reshape(1, 32, 32)
        prev_image = torch.tensor(prev_image / 255)
        current_image = torch.tensor(current_image / 255)
        self.Reinforcement_model.memory.push(prev_image.float(), prev_features.float(), action, current_image.float(),
                                             new_features.float(), torch.tensor(reward).float(), **kwargs)

    def trainModel(self):
        for i in range(self.Reinforcement_model.TARGET_UPDATE):
            self.Reinforcement_model.optimize_ESP()

    def saveMemory(self):
        self.Reinforcement_model.memory.save()

    def reset(self):
        self.prev_image = None
