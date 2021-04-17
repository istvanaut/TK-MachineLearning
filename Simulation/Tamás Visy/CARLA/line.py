import math
import random

import numpy as np

from support.logger import logger

LINE_FILE_NAME = 'files/line.npy'

line_file_points = np.load(LINE_FILE_NAME)
logger.info(f'Loaded file {LINE_FILE_NAME} for line.py')


class Line:
    # TODO (7) rename path
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

    def find_closest_point_index(self, point):
        dist = distance(point, self.points[0])
        index = 0
        for i, p in enumerate(self.points[1:]):
            if distance(point, p) < dist:
                dist = distance(point, p)
                index = i
        return index

    def find_segment(self, point):
        index = self.find_closest_point_index(point)
        if index is 0:
            return Segment(self.points[0], self.points[1])
        if index is len(self.points)-1:
            return Segment(self.points[-2], self.points[-1])
        distance_from_prev = distance(self.points[index-1], point)
        distance_from_next = distance(self.points[index+1], point)
        if distance_from_prev <= distance_from_next:
            return Segment(self.points[index-1], self.points[index])
        else:
            return Segment(self.points[index], self.points[index+1])

    def distance_along_line(self, point):
        # TODO (6) refactor
        if point is None:
            return None
        closest_segment = self.find_segment(point)
        dist = 0
        closest_index = 0
        seg_found = None
        # Searches for closest segment index
        # Sums up lengths
        for i in range(0, len(self.points) - 1):
            seg = self.segment(i)
            if seg.start[0] == closest_segment.start[0]:
                closest_index = i
                dist = dist + seg.length()
                seg_found = seg
                break
            else:
                dist = dist + seg.length()
        dist = dist + calculate_last_distance(point, seg_found.start, seg_found.end)
        return dist


def calculate_last_distance(point, line_point1, line_point2):
    # TODO (6) refactor
    vec1 = line_point1 - point
    vec2 = line_point2 - point
    last_distance_result = np.abs(np.cross(vec1, vec2)) / np.linalg.norm(line_point1 - line_point2)
    return last_distance_result


def get_line():
    return Line(line_file_points)


def fix(points, min_required_distance=1.0):
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
            if not inserted and distance(p, pf) < min_required_distance:
                i = points_fixed.index(pf)
                points_fixed.remove(pf)
                points_fixed.insert(i, np.average([p, pf], 0).tolist())
                inserted = True
        # If we haven't removed any point (new point is far away from old ones) we append it
        if not inserted:
            points_fixed.append(p)
    return np.asarray(points_fixed)


class Segment:
    # TODO (7) rename line

    def __init__(self, start, end):
        self.start = start
        self.end = end

    def length(self):
        return distance(self.start, self.end)

    def side(self, point):
        # TODO (5) can side change when distance is > 0?
        a = self.start
        b = self.end
        # TODO (3) can't this be done with a numpy function?
        if (b[0] - a[0]) * (point[1] - a[1]) > (b[1] - a[1]) * (point[0] - a[0]):
            return 1.0
        elif (b[0] - a[0]) * (point[1] - a[1]) < (b[1] - a[1]) * (point[0] - a[0]):
            return -1.0
        else:
            return 0.0

    def distance(self, point):
        start = np.array(self.start)
        end = np.array(self.end)
        point = np.array(point)
        return abs(np.cross(end - start, point - start) / np.linalg.norm(end - start))

    def direction(self):
        return direction(self.start, self.end)


def direction(point0, point1):
    """Calculates direction in radians"""
    d = [0, 0, 0]
    vector = [point1[0] - point0[0], point1[1] - point0[1]]
    d[1] = math.atan2(vector[1], vector[0])
    while d[1] <= -np.pi / 2:
        d[1] += np.pi
    return d


def distance(point0, point1):
    """Calculates distance in 2D"""
    if point0 is None or point1 is None:
        return None
    diff = np.subtract(point0, point1)
    return np.sqrt(diff[0] ** 2 + diff[1] ** 2)
