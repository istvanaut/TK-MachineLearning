import time

from support.logger import logger
from threads.basethread import BaseThread
from support.datakey import DataKey


class PollerThread(BaseThread):
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

                p = self.vehicle.get_location()
                p = [p.x, p.y, p.z]  # m
                self.data.put(DataKey.SENSOR_POSITION, p)

                v = self.vehicle.get_velocity()
                v = [v.x, v.y, v.z]  # m/s
                self.data.put(DataKey.SENSOR_VELOCITY, v)
                time.sleep(0.05)
            except RuntimeError as r:
                logger.error(f'Error: {r}')
                logger.warning(f'Setting vehicle to None')
                self.vehicle = None
