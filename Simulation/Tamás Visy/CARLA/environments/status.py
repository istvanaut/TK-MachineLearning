import time

from line import distance
from support.logger import logger
from support.datakey import DataKey

MAX_OFF_DISTANCE = 2.0


# TODO (6) traveled -> distance form start along line

class Status:
    def __init__(self, finished=False, successful=False, reason='No reason specified', traveled=0.0, traveled_line=0.0):
        self.finished = finished
        self.successful = successful
        self.reason = reason
        self.traveled = traveled
        self.traveled_line = traveled_line

    def __str__(self):
        return f'Finished: {self.finished}, success: {self.successful}, {self.reason} ' \
               f'-:- {self.traveled} meters, or {self.traveled_line} by line '

    def check(self, environment):
        finished = False
        successful = False
        reason = 'UNKNOWN'

        # TODO (5) time maximization is broken
        t = time.time()

        coll = environment.data.get(DataKey.SENSOR_COLLISION)

        pos = environment.data.get(DataKey.SENSOR_POSITION)
        if pos is not None:
            pos = [pos[0], pos[1]]
            dist = environment.line.distance(pos)
        else:
            pos = None
            dist = -1.0

        if coll is not None:
            finished = True
            reason = 'Collision'
        if dist > MAX_OFF_DISTANCE:
            finished = True
            reason = f'Too far from line: {MAX_OFF_DISTANCE} meters'
            logger.warning(pos)
            logger.debug(environment.line.find_segment(pos).start)
        if time.time() - t > 100:
            finished = True
            reason = 'Time ran out'
        if pos is not None and distance(environment.line.find_segment(pos).end, environment.line.end) < 0.1:
            finished = True
            successful = True
            reason = 'At finish'
        self.finished = finished
        self.successful = successful
        self.reason = reason
        self.traveled = distance(pos, environment.line.start)
        self.traveled_line = environment.line.distance_along_line(pos)
