import time

from basethread import BaseThread
from key import Key


class PollerThread(BaseThread):
    vehicle = None

    def set_vehicle(self, v):
        self.vehicle = v

    def loop(self):
        if self.vehicle is None:
            time.sleep(1.0)
        else:
            a = self.vehicle.get_acceleration()
            a = [a.x, a.y, a.z]  # m/s2
            self.data.put(Key.SENSOR_ACCELERATION, a)

            p = self.vehicle.get_location()
            p = [p.x, p.y, p.z]  # m
            self.data.put(Key.SENSOR_POSITION, p)

            v = self.vehicle.get_velocity()
            v = [v.x, v.y, v.z]  # m/s
            self.data.put(Key.SENSOR_VELOCITY, v)
            time.sleep(0.05)
