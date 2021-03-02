# based on agentthread.py from Tamás Visy's temalab_gta repo

import time

from agent import Agent, convert
from basethread import BaseThread
from key import Key


class AgentThread(BaseThread):
    agent = None

    def __init__(self, data):
        super().__init__(data)
        self.agent = Agent()

    def loop(self):
        state = self.state()

        out = self.agent.predict(convert(state))

        self.data.put(Key.CONTROL_OUT, out)

        time.sleep(0.05)

    def state(self):
        ca = self.data.get(Key.SENSOR_CAMERA)
        r = self.data.get(Key.SENSOR_RADAR)
        co = self.data.get(Key.SENSOR_COLLISION)
        v = self.data.get(Key.SENSOR_VELOCITY)
        a = self.data.get(Key.SENSOR_ACCELERATION)
        p = self.data.get(Key.SENSOR_POSITION)
        o = self.data.get(Key.SENSOR_OBSTACLE)
        return ca, r, co, v, a, p, o
