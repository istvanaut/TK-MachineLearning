import numpy as np

from support.logger import logger


class Line:
    """2D line"""

    def __init__(self, start, end):
        self.start = start
        self.end = end

    # def where(self, point):
    #     if in_range(point, self.start):
    #         logger.info(f'Point close to start')
    #     if in_range(point, self.end):
    #         logger.info(f'Point close to end')
    #     dist = self.distance(point)
    #     logger.info(f'The distance from line is {dist}')

    def distance(self, point):
        # Is this correct? Haven't checked... - probably not correct? TODO (5)
        start = np.array(self.start)
        end = np.array(self.end)
        point = np.array(point)
        return abs(np.cross(end-start, point-start) / np.linalg.norm(end-start))

    def distance_from_start(self, point):
        if point is None:
            return None
        diff = np.subtract(self.start, point)
        dist = np.sqrt(diff[0] ** 2 + diff[1] ** 2)
        return dist


def in_range(p0, p1, r=1.0):
    if p0 is None or p1 is None:
        return False
    diff = np.subtract(p0, p1)
    dist = np.sqrt(diff[0] ** 2 + diff[1] ** 2)
    return dist < r
