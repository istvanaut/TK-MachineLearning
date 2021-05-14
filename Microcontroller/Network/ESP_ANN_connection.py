import torch
import numpy as np
from ReinforcementModel import ReinforcementModel
from Networks import FlatDense


device = torch.device("cuda" if torch.cuda.is_available() else "cpu")

prev_image = None


def get_feature_dimension():
    return 10


Reinforcement_model = ReinforcementModel(dim_features=get_feature_dimension, height=32,
                                         width=32, n_actions=4, model=FlatDense)


def storeData(prev_lightSensor,
              prev_leftUltrasonicSensor,
              prev_rightUltrasonicSensor,
              prev_laserDistance,
              curr_image,
              prev_leftMotorValue,
              prev_right_motorValue,
              reward,
              curr_lightSensor,
              curr_leftUltrasonicSensor,
              curr_rightUltrasonicSensor,
              curr_laserDistance
              ):
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
    global prev_image

    current_image = curr_image

    if prev_image is None:
        prev_image = current_image
        return

    prev_features = torch.tensor(prev_lightSensor, prev_leftUltrasonicSensor, prev_rightUltrasonicSensor,
                                 prev_laserDistance)
    new_features = torch.tensor(curr_lightSensor, curr_leftUltrasonicSensor, curr_rightUltrasonicSensor,
                                curr_laserDistance)
    action = torch.tensor(prev_leftMotorValue, prev_right_motorValue)

    prev_image = np.array(prev_image).reshape(1, 32, 32)

    current_image = np.array(current_image).reshape(1, 32, 32)

    Reinforcement_model.memory.push(prev_image, prev_features, action, current_image, new_features, reward)

    prev_image = current_image


def trainModel():
    for i in range(Reinforcement_model.TARGET_UPDATE):
        Reinforcement_model.optimize_ESP()
