import numpy as np

from support.logger import logger


class Line:
    # TODO (8) add more complex lines
    def __init__(self, start, end):
        self.start = start
        self.end = end

    def distance(self, point):
        # Is this correct? Haven't checked... - probably not correct? TODO (5)
        start = np.array(self.start)
        end = np.array(self.end)
        point = np.array(point)
        return abs(np.cross(end-start, point-start) / np.linalg.norm(end-start))

    def direction(self):
        # TODO (6) calculate
        logger.warning('Returning not calculated line direction')
        # Returns the direction the vehicle must spawn
        return [0, -90, 0]


def distance(point0, point1):
    if point0 is None or point1 is None:
        return False
    diff = np.subtract(point0, point1)
    # Calculates distance in 2D
    return np.sqrt(diff[0] ** 2 + diff[1] ** 2)
