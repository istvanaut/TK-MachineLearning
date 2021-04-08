import random

# This is where the reward functions will be defined
close_distance = 0.1
middle_distance = 0.5
long_distance = 1.0


def base_reward(prev_state, new_state):
    # The functions checks for distance from line,
    # assigns positive reward if its close to it,
    # the further away the less reward, even negative if too far.
    # It also takes into account the distance of the previous state
    reward = 4.0
    distance_point = 5.0

    # if new_state.distance_from_line <= close_distance:
    #     reward += distance_point ** 1.5
    # elif new_state.distance_from_line <= middle_distance:
    #     reward += distance_point
    # elif new_state.distance_from_line <= long_distance:
    #     reward += distance_point ** 0.5
    # else:
    #     reward -= distance_point ** 0.1 - 1
    if new_state.distance_from_line > prev_state.distance_from_line:
        reward += -1.6
    if abs(new_state.direction) > abs(prev_state.direction):
        reward += -3.0
    reward -= 0.25*(1+new_state.distance_from_line)**1.5  # 0 < d ~<~ 2

    return reward


def inline_reward(prev_state, new_state):
    reward = 0.0
    distance_point = 1
    if new_state.distance_from_line <= close_distance:
        reward += distance_point
    else:
        reward -= 1

    return reward
