import math

import numpy as np

from support.logger import logger

LINE_FILE_NAME = 'files/line.npy'


class Line:
    # a 2D Line consisting of segments

    def __init__(self, points):
        self.points = None
        self.start = None
        self.end = None
        self.__set_points(points)

    def __set_points(self, points):
        self.points = points
        self.start = self.points[0]
        self.end = self.points[-1]

    def slice(self, start=None, to=None):
        if start is None:
            start = 0
        if to is None:
            to = len(self.points)
        self.__set_points(self.points[start:to])

    def invert(self):
        self.__set_points(self.points[::-1])

    def side(self, point):
        return self.find_segment(point).side(point)

    def distance(self, point):
        return self.find_segment(point).distance(point)

    def segment(self, i):
        return Segment(self.points[i], self.points[i + 1])

    def direction(self, point=None):
        if point is None:
            return self.segment(0).direction()
        else:
            self.find_segment(point).direction()
            return None

    def find_segment(self, point):
        # This could be done much better
        # TODO (3) line can't contain loops or this breaks
        dist0 = distance(point, self.points[0])
        p0 = self.points[0]
        dist1 = distance(point, self.points[1])
        p1 = self.points[1]
        for p in self.points[2:]:
            if distance(point, p) < dist0:
                dist0 = distance(point, p)
                p0 = p
            elif distance(point, p) < dist1:
                dist1 = distance(point, p)
                p1 = p
        return Segment(p0, p1)


line_file_points = np.load(LINE_FILE_NAME)
logger.info(f'Loaded file {LINE_FILE_NAME}')


def get_line():
    return Line(line_file_points)


def fix(points, difference=1.0):
    """
    Simplifies the list of points

    | Each point in old list is either appended to the new list or modifies one already present point"""
    # TODO (4) could probably be simpler and better
    parr = np.array(points).tolist()
    points_fixed = []
    while len(parr) > 0:
        p = parr.pop(0)
        # Search from most recently added points
        inserted = False
        for pf in points_fixed[::-1]:
            # If one is close to the current point, we remove it and insert their average
            if not inserted and distance(p, pf) < difference:
                i = points_fixed.index(pf)
                points_fixed.remove(pf)
                points_fixed.insert(i, np.average([p, pf], 0).tolist())
                inserted = True
        # If we haven't removed any point (new point is far away from old ones) we append it
        if not inserted:
            points_fixed.append(p)
    parr = np.asarray(points_fixed)
    return parr


class Segment:
    def __init__(self, start, end):
        self.start = start
        self.end = end

    def length(self):
        return distance(self.start, self.end)

    def side(self, point):
        return np.sign(direction(self.start, point)[1] - direction(self.start, self.end)[1])

    def distance(self, point):
        start = np.array(self.start)
        end = np.array(self.end)
        point = np.array(point)
        return abs(np.cross(end - start, point - start) / np.linalg.norm(end - start))

    def direction(self):
        return direction(self.start, self.end)


def direction(point0, point1):
    d = [0, 0, 0]
    vector = [point1[0] - point0[0], point1[1] - point0[1]]
    d[1] = math.atan2(vector[1], vector[0]) / np.pi * 180.0
    while d[1] <= -180.0:
        d[1] += 360.0
    return d


def distance(point0, point1):
    if point0 is None or point1 is None:
        return None
    diff = np.subtract(point0, point1)
    # Calculates distance in 2D
    return np.sqrt(diff[0] ** 2 + diff[1] ** 2)
