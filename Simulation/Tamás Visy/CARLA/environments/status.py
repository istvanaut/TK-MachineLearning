import time

from line import distance
from support.logger import logger
from support.datakey import DataKey

MAX_OFF_DISTANCE = 2.0


class Status:
    def __init__(self, finished=False, successful=False, reason='No reason specified', dist=0.0, traveled=0.0):
        self.finished = finished
        self.successful = successful
        self.reason = reason
        self.dist = dist
        self.traveled = traveled

    def __str__(self):
        return f'Finished: {self.finished}, success: {self.successful}, {self.reason} ' \
               f'-:- {self.dist} meters, or {self.traveled} by line '

    def check(self, environment):
        finished = False
        successful = False
        reason = 'UNKNOWN'

        t = time.time()

        coll = environment.data.get(DataKey.SENSOR_COLLISION)

        pos = environment.data.get(DataKey.SENSOR_POSITION)
        if pos is not None:
            pos = [pos[0], pos[1]]
            d = environment.line.distance(pos)
        else:
            pos = None
            d = -1.0

        if coll is not None:
            finished = True
            reason = 'Collision'
        if d > MAX_OFF_DISTANCE:
            finished = True
            reason = f'Too far from line: {MAX_OFF_DISTANCE} meters'
            logger.debug(pos)
            logger.debug(environment.line.find_segment(pos).start)
        if time.time() - t > 100:
            # TODO (5) time maximization is broken - t is set in this function
            finished = True
            reason = 'Time ran out'
        if pos is not None and distance(environment.line.find_segment(pos).end, environment.line.end) < 0.1:
            finished = True
            successful = True
            reason = 'At finish'
        self.finished = finished
        self.successful = successful
        self.reason = reason
        self.dist = distance(pos, environment.line.start)
        self.traveled = environment.line.distance_along_line(pos)
