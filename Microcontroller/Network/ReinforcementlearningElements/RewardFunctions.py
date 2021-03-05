from State import State


# This is where the reward functions will be defined

def base_reward(prev_state, new_state):
    # The functions checks for distance from line,
    # assigns positive reward if its close to it,
    # the further away the less reward, even negative if too far.
    # It also takes into account the distance of the previous state
    new_state = State(*new_state)
    prev_state = State(*prev_state)
    reward = 0.0
    distance_point = 10
    if new_state.distance_from_line <= 2.0:
        reward += distance_point
    elif new_state.distance_from_line <= 10.0:
        reward += distance_point * (10.0 / new_state.distance_from_line)
    else:
        reward -= distance_point * (new_state.distance_from_line / 10.0)
    if new_state.distance_from_line > prev_state.distance_from_line:
        reward -= 2 + (new_state.distance_from_line - prev_state.distance_from_line)

    return reward
