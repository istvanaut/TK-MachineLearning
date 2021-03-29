import time

from line import distance
from support.datakey import DataKey


class Status:
    def __init__(self, finished=False, successful=False, reason='No reason specified', dist=0.0):
        self.finished = finished
        self.successful = successful
        self.reason = reason
        self.dist = dist

    def __str__(self):
        return f'Finished: {self.finished}, success: {self.successful}, {self.reason} - {self.dist} meters'

    def check(self, environment):
        finished = False
        successful = False
        reason = 'UNKNOWN'
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
        if dist > 4.0:
            finished = True
            reason = 'Too far from line'
        if time.time() - t > 100:
            finished = True
            reason = 'Time ran out'
        if pos is not None and distance(pos, environment.line.end) < 1.0:
            finished = True
            successful = True
            reason = 'At finish'
        self.finished = finished
        self.successful = successful
        self.reason = reason
        self.dist = distance(pos, environment.line.start)
