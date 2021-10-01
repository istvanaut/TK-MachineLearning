from dataclasses import dataclass
import numpy as np

@dataclass
class State:
    light_sensor: int
    ultra_sound_left: int
    ultra_sound_right: int
    laser: int
    left_motor: tuple
    right_motor: tuple
    reward: tuple
    image: np.array
