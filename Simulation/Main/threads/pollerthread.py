import time

from data.datakey import DataKey
from support.logger import logger
from threads.haltabledatathread import HaltableDataThread

POLL_TIME = 0.05


class PollerThread(HaltableDataThread):
    vehicle = None

    def set_vehicle(self, v):
        self.vehicle = v

    def loop(self):
        if self.vehicle is None:
            time.sleep(1.0)
        else:
            try:
                self.poll_acceleration()

                self.poll_position_and_direction()

                self.poll_angular_acceleration()

                self.poll_velocity()

                time.sleep(POLL_TIME)
            except RuntimeError as r:
                logger.error(f'Error: {r}')
                logger.warning(f'Setting vehicle to None')
                self.vehicle = None

    def poll_velocity(self):
        v = self.vehicle.get_velocity()
        v = [v.x, v.y, v.z]  # m/s
        self.data.put(DataKey.SENSOR_VELOCITY, v)

    def poll_angular_acceleration(self):
        aa = self.vehicle.get_angular_velocity() / POLL_TIME  # poll time is an estimation
        aa = [aa.x, aa.y, aa.y]
        self.data.put(DataKey.SENSOR_ANGULAR_ACCELERATION, aa)

    def poll_position_and_direction(self):
        t = self.vehicle.get_transform()
        p = t.location
        r = t.rotation
        p = [p.x, p.y, p.z]  # m
        r = [r.pitch, r.yaw, r.roll]  # degrees?
        self.data.put(DataKey.SENSOR_POSITION, p)
        self.data.put(DataKey.SENSOR_DIRECTION, r)

    def poll_acceleration(self):
        a = self.vehicle.get_acceleration()
        a = [a.x, a.y, a.z]  # m/s2
        self.data.put(DataKey.SENSOR_ACCELERATION, a)
