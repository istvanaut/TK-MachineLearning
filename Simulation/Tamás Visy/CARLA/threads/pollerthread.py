import time

from support.logger import logger
from threads.haltabledatathread import HaltableDataThread
from support.datakey import DataKey


class PollerThread(HaltableDataThread):
    vehicle = None

    def set_vehicle(self, v):
        self.vehicle = v

    def loop(self):
        if self.vehicle is None:
            time.sleep(1.0)
        else:
            try:
                a = self.vehicle.get_acceleration()
                a = [a.x, a.y, a.z]  # m/s2
                self.data.put(DataKey.SENSOR_ACCELERATION, a)

                t = self.vehicle.get_transform()
                p = t.location
                r = t.rotation
                p = [p.x, p.y, p.z]  # m
                r = [r.pitch, r.yaw, r.roll]  # degrees?
                self.data.put(DataKey.SENSOR_POSITION, p)
                self.data.put(DataKey.SENSOR_DIRECTION, r)

                # estimated time passed since last tick
                # TODO (6) extract this as setting
                time_to_sleep = 0.05
                aa = self.vehicle.get_angular_velocity() / time_to_sleep
                aa = [aa.x, aa.y, aa.y]
                self.data.put(DataKey.SENSOR_ANGULAR_ACCELERATION, aa)

                v = self.vehicle.get_velocity()
                v = [v.x, v.y, v.z]  # m/s
                self.data.put(DataKey.SENSOR_VELOCITY, v)
                time.sleep(time_to_sleep)
            except RuntimeError as r:
                logger.error(f'Error: {r}')
                logger.warning(f'Setting vehicle to None')
                self.vehicle = None
