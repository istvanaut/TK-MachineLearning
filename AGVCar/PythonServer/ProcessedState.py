import datetime
import struct
import numpy as np
from tinydb import TinyDB, Query, where
from tinydb.table import Document


class ProcessedState:
    def __init__(self, **kwargs):

        # Db Path
        self.dbPath = 'db/statedb.json'

        self.statesList = []

        if kwargs.get('processed_data'):
            self.addProcessedData(kwargs['processed_data'])
        if kwargs.get('query'):
            self.getDataFromDb(kwargs['query'])

    def addProcessedData(self, processed_data):
        # Getting processed parameters
        prev_state = processed_data[0]
        prev_image = processed_data[2]
        curr_state = processed_data[1]
        curr_image = processed_data[3]

        # Creating dictionary for previous states
        prev_state_dict = {
            'light_sensor': int.from_bytes(prev_state[0:4], byteorder='little', signed=False),
            'ultra_sound_left': int.from_bytes(prev_state[4:8], byteorder='little', signed=False),
            'ultra_sound_right': int.from_bytes(prev_state[8:12], byteorder='little', signed=False),
            'laser': int.from_bytes(prev_state[12:16], byteorder='little', signed=False),
            'left_motor': struct.unpack('f', prev_state[16:20]),
            'right_motor': struct.unpack('f', prev_state[20:24]),
            'reward': struct.unpack('f', prev_state[24:28]),
            'image': np.array(prev_image)
        }

        # Creating dictionary for current states
        curr_state_dict = {
            'light_sensor': int.from_bytes(curr_state[0:4], byteorder='little', signed=False),
            'ultra_sound_left': int.from_bytes(curr_state[4:8], byteorder='little', signed=False),
            'ultra_sound_right': int.from_bytes(curr_state[8:12], byteorder='little', signed=False),
            'laser': int.from_bytes(curr_state[12:16], byteorder='little', signed=False),
            'left_motor': None,
            'right_motor': None,
            'reward': None,
            'image': np.array(curr_image)
        }

        self.statesList.append({'prev_state': prev_state_dict, 'curr_state': curr_state_dict})
        return prev_state_dict, curr_state_dict

    # Search from db with TinyDb Queries.
    # Documentation: https://tinydb.readthedocs.io/en/latest/usage.html#handling-data
    # Use 'state' instead of query instance. Expected type: String
    def getDataFromDb(self, query):
        db = TinyDB(self.dbPath)
        query_list = db.search(eval(query))

        # Correcting format of images
        for states in query_list:
            states['prev_state']['image'] = np.array(states['prev_state']['image'])
            states['curr_state']['image'] = np.array(states['curr_state']['image'])

        self.statesList = self.statesList + query_list

    # Saves all states in statesList or if exists, updates.
    def saveActualStates(self):
        db = TinyDB(self.dbPath)
        for states in self.statesList:

            # Changing np.arrays to lists, so we can serialize them
            states['prev_state']['image'] = states['prev_state']['image'].tolist()
            states['curr_state']['image'] = states['curr_state']['image'].tolist()

            date = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            try:
                states['update_date'] = date
                db.update(Document(states, doc_id=states.doc_id))
            except AttributeError:
                states['create_date'] = date
                db.insert(states)