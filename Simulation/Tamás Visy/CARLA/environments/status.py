from path import distance
from settings import MAX_OFF_DISTANCE
from support.logger import logger
from support.datakey import DataKey



class Status:
    def __init__(self, finished=False, successful=False, reason='No reason specified', dist=0.0, traveled=0.0):
        self.finished = finished
        self.successful = successful
        self.reason = reason
        self.dist = dist
        self.traveled = traveled

    def __str__(self):
        return f'Finished: {self.finished}, success: {self.successful}, {self.reason} ' \
               f'-:- {self.traveled} meters by path, or {self.dist} meters (bee-line)'

    def check(self, environment):
        finished = False
        successful = False
        reason = 'UNKNOWN'

        coll = environment.data.get(DataKey.SENSOR_COLLISION)

        pos = environment.data.get(DataKey.SENSOR_POSITION)
        if pos is not None:
            pos = [pos[0], pos[1]]
            d = environment.path.distance(pos)
        else:
            pos = None
            d = -1.0

        if coll is not None:
            finished = True
            reason = 'Collision'
        if d > MAX_OFF_DISTANCE:
            finished = True
            reason = f'Too far from path: {MAX_OFF_DISTANCE} meters'
            logger.debug(pos)
            logger.debug(environment.path.find_segment(pos).start)
        if pos is not None and distance(environment.path.find_segment(pos).end, environment.path.end) < 0.1:
            finished = True
            successful = True
            reason = 'At finish'
        self.finished = finished
        self.successful = successful
        self.reason = reason
        self.dist = distance(pos, environment.path.start)
        self.traveled = environment.path.distance_along_path(pos)
