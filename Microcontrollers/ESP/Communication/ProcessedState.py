import struct
import numpy as np

from Microcontrollers.ESP.Communication.State import State


class ProcessedState:
    def __init__(self, processed_data):
        prev_state = processed_data[0]
        prev_image = processed_data[2]
        self.previous_state = State(light_sensor=int.from_bytes(prev_state[0:4], byteorder='little', signed=False),
                                    ultra_sound_left=int.from_bytes(prev_state[4:8], byteorder='little', signed=False),
                                    ultra_sound_right=int.from_bytes(prev_state[8:12], byteorder='little',
                                                                     signed=False),
                                    laser=int.from_bytes(prev_state[12:16], byteorder='little', signed=False),
                                    left_motor=struct.unpack('f', prev_state[16:20]),
                                    right_motor=struct.unpack('f', prev_state[20:24]),
                                    reward=struct.unpack('f', prev_state[24:28]),
                                    image=np.array(prev_image))
        curr_state = processed_data[1]
        curr_image = processed_data[3]
        self.current_state = State(light_sensor=int.from_bytes(curr_state[0:4], byteorder='little', signed=False),
                                   ultra_sound_left=int.from_bytes(curr_state[4:8], byteorder='little', signed=False),
                                   ultra_sound_right=int.from_bytes(curr_state[8:12], byteorder='little',
                                                                    signed=False),
                                   laser=int.from_bytes(curr_state[12:16], byteorder='little', signed=False),
                                   left_motor=None,
                                   right_motor=None,
                                   reward=None,
                                   image=np.array(curr_image))
