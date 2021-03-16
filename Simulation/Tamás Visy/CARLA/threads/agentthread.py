# based on agentthread.py from Tamás Visy's temalab_gta repo

import time

from agents.agent import Agent, convert
from agents.networkagent import NetworkAgent, AGENT_MODEL_PATH
from threads.basethread import BaseThread
from support.datakey import DataKey


class AgentThread(BaseThread):

    def __init__(self, data):
        super().__init__(data)
        self.agent = NetworkAgent()
        self.line = None
        self.agent.load(AGENT_MODEL_PATH)
        self.first = True

    def set_line(self, line):
        self.line = line

    def loop(self):
        data = self.unpack_data()

        state = convert(data)

        if state is not None:
            if not self.first:
                self.agent.optimize(new_state=state)
            out = self.agent.predict(state)
            self.first = False

            self.data.put(DataKey.CONTROL_OUT, out)

        time.sleep(0.05)

    def unpack_data(self):
        ca = self.data.get(DataKey.SENSOR_CAMERA)
        r = self.data.get(DataKey.SENSOR_RADAR)
        co = self.data.get(DataKey.SENSOR_COLLISION)
        v = self.data.get(DataKey.SENSOR_VELOCITY)
        a = self.data.get(DataKey.SENSOR_ACCELERATION)
        p = self.data.get(DataKey.SENSOR_POSITION)
        o = self.data.get(DataKey.SENSOR_OBSTACLE)
        if p is not None:
            d = self.line.distance([p[0], p[1]])
        else:
            d = None
        return ca, r, co, v, a, p, o, d

    def finish(self):
        self.agent.save(AGENT_MODEL_PATH)
        super().finish()
