import glob
import os
import sys
import time
from basethread import BaseThread
from key import Key

try:
    sys.path.append(glob.glob('../carla/dist/carla-*%d.%d-%s.egg' % (
        sys.version_info.major,
        sys.version_info.minor,
        'win-amd64' if os.name == 'nt' else 'linux-x86_64'))[0])
except IndexError:
    print('IndexError while trying to find carla egg')
    sys.exit()
try:
    import carla  # Carla package uses the egg from Carla - Python didn't work
except ModuleNotFoundError:
    print('ModuleNotFoundError while importing carla')
    sys.exit()


class ControllerThread(BaseThread):
    vehicle = None

    def __init__(self, data):
        super().__init__(data)

    def set_vehicle(self, vehicle):
        self.vehicle = vehicle

    def loop(self):
        time.sleep(0.05)
        d = self.data.get(Key.CONTROL_OUT)
        if d is not None:
            self.control(d[0], d[1])

    def control(self, throttle, steering):
        # It is possible to control individual wheels of the car - check carla.WheelPhysicsControl
        if self.vehicle is not None:
            if throttle >= 0.0:
                self.vehicle.apply_control(carla.VehicleControl(throttle=throttle, steer=steering))
            else:
                throttle *= -1.0
                self.vehicle.apply_control(carla.VehicleControl(reverse=True, throttle=throttle, steer=steering))
        else:
            raise RuntimeError('Vehicle not set in ControllerThread')
