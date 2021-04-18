import numpy as np

from environments.environment import Environment
from support.logger import logger

RETURNABLES_FILE_NAME = 'files/returnables.npy'
STATUSES_FILE_NAME = 'files/statuses.npy'


# "Replays" can be created by collecting then saving the proper data in main
# Returnables is a numpy array of (data, path, starting_dir) from CarlaEnvironment.pull()
def load_returnables_from_file():
    return np.load(RETURNABLES_FILE_NAME)


# Statuses is a numpy array of (status) from CarlaEnvironment.check()
def load_statuses_from_file():
    return np.load(STATUSES_FILE_NAME)


class ReplayEnvironment(Environment):
    def __init__(self):
        self.i = 0
        self.returnables = load_returnables_from_file()
        self.statuses = load_statuses_from_file()

        if self.returnables is None or self.statuses is None or len(self.returnables) < 1 or len(self.statuses) < 1:
            logger.critical('Data issues in TestEnvironment')

    def start(self):
        pass

    def reset(self, do_update_path=True):
        pass

    def clear(self):
        pass

    def pull(self):
        self.i += 1
        return self.returnables[self.i % len(self.returnables)]

    def put(self, key, data):
        pass

    def check(self):
        return self.statuses[self.i % len(self.statuses)]

    def setup(self):
        pass
