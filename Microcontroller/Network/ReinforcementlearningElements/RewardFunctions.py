from State import State
import random

# This is where the reward functions will be defined
close_distance = 0.5
middle_distance = 1.0
long_distance = 2.0

def base_reward(prev_state, new_state):
    # The functions checks for distance from line,
    # assigns positive reward if its close to it,
    # the further away the less reward, even negative if too far.
    # It also takes into account the distance of the previous state
    reward = 0.0
    distance_point = 5

    if new_state.distance_from_line <= close_distance:
        reward += distance_point**1.5
    elif new_state.distance_from_line <= middle_distance:
        reward += distance_point
    elif new_state.distance_from_line <= long_distance:
        reward += distance_point**0.5
    else:
        reward -= distance_point**0.1 - 1
    if new_state.distance_from_line > prev_state.distance_from_line:
        reward -= distance_point**0.1 - 1
    reward -= new_state.distance_from_line  # 0 < d ~<~ 5-10
    return reward

def inline_reward(prev_state, new_state):
    new_state = State(*new_state)
    prev_state = State(*prev_state)
    reward = 0.0
    distance_point = 10
    if new_state.distance_from_line <= 1.0:
        reward += distance_point

    return reward