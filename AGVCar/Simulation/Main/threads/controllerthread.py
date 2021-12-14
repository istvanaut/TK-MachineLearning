import time

from CARLA import icarla
from data.datakey import DataKey
from support.logger import logger
from threads.haltabledatathread import HaltableDataThread


class ControllerThread(HaltableDataThread):
    vehicle = None

    def __init__(self, data):
        super().__init__(data)

    def set_vehicle(self, vehicle):
        self.vehicle = vehicle

    def loop(self):
        if self.vehicle is not None:
            time.sleep(0.05)
            d = self.data.get(DataKey.CONTROL_OUT)
            if d is not None:
                self.control(d[0], d[1])
            else:
                self.control(0, 0)
        else:
            time.sleep(1.0)

    def control(self, throttle, steering):
        if self.vehicle is not None:
            if throttle >= 0.0:
                try:
                    self.vehicle.apply_control(icarla.vehicle_control(throttle=throttle, steer=steering))
                except RuntimeError as r:
                    logger.error(f'Error: {r}')
                    logger.warning(f'Setting vehicle to None')
                    self.vehicle = None
            else:
                try:
                    throttle *= -1.0
                    self.vehicle.apply_control(icarla.vehicle_control(reverse=True, throttle=throttle, steer=steering))
                except RuntimeError as r:
                    logger.error(f'Error: {r}')
                    logger.warning(f'Setting vehicle to None')
                    self.vehicle = None
        else:
            raise RuntimeError('Vehicle not set in ControllerThread')
