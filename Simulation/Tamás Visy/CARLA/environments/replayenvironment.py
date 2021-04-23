import os

import numpy as np

from environments.environment import Environment
from support.logger import logger

REPLAY_PULLS_FILE_NAME = 'files/replay_pulls.npy'
REPLAY_STATUSES_FILE_NAME = 'files/replay_statuses.npy'


# "Replays" can be created by collecting then saving the proper data in main
# Returnables is a numpy array of (data, path, starting_dir) from CarlaEnvironment.pull()
def load_replay_pulls_from_file():
    if os.path.isfile(REPLAY_PULLS_FILE_NAME):
        return np.load(REPLAY_PULLS_FILE_NAME)
    else:
        logger.critical(f'File {REPLAY_PULLS_FILE_NAME} not found for ReplayEnvironment')
        return None


# Statuses is a numpy array of (status) from CarlaEnvironment.check()
def load_replay_statuses_from_file():
    if os.path.isfile(REPLAY_STATUSES_FILE_NAME):
        return np.load(REPLAY_STATUSES_FILE_NAME)
    else:
        logger.critical(f'File {REPLAY_STATUSES_FILE_NAME} not found for ReplayEnvironment')
        return None


class ReplayEnvironment(Environment):
    def __init__(self):
        self.i = 0
        self.pulls = load_replay_pulls_from_file()
        self.statuses = load_replay_statuses_from_file()

        if self.pulls is None or self.statuses is None or len(self.pulls) < 1 or len(self.statuses) < 1:
            logger.critical('Critical data issues in ReplayEnvironment')
            raise RuntimeError('Critical data issues in ReplayEnvironment')

    def start(self):
        pass

    def reset(self, do_update_path=True):
        pass

    def clear(self):
        pass

    def pull(self):
        self.i += 1
        return self.pulls[self.i % len(self.pulls)]

    def put(self, key, data):
        pass

    def check(self):
        return self.statuses[self.i % len(self.statuses)]

    def setup(self):
        pass
