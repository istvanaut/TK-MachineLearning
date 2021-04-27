import random

# This is where the reward functions will be defined
close_distance = 0.5
middle_distance = 0.5
long_distance = 1.0


def base_reward(prev_state, new_state):
    # The functions checks for distance from path,
    # assigns positive reward if its close to it,
    # the further away the less reward, even negative if too far.
    # It also takes into account the distance of the previous state
    reward = 4.0
    distance_point = 5.0

    # if new_state.distance_from_path <= close_distance:
    #     reward += distance_point ** 1.5
    # elif new_state.distance_from_path <= middle_distance:
    #     reward += distance_point
    # elif new_state.distance_from_path <= long_distance:
    #     reward += distance_point ** 0.5
    # else:
    #     reward -= distance_point ** 0.1 - 1
    if new_state.distance_from_path > prev_state.distance_from_path:
        reward += -1.6
    if abs(new_state.direction) > abs(prev_state.direction):
        reward += -3.0
    reward -= 0.25 * (1 + new_state.distance_from_path) ** 1.5  # 0 < d ~<~ 2

    return reward


def inline_reward(prev_state, new_state):
    reward = 0.0
    distance_point = 1
    if new_state.velocity<0.001 and prev_state.velocity<0.001:
        reward-=distance_point
    if new_state.distance_from_path >= close_distance:
        reward -= distance_point * new_state.distance_from_path ** 2
    else:
        reward += 0
    return reward
