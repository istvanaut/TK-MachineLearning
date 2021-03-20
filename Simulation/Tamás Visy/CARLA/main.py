# @title main

from agents.agent import convert, unpack
from agents.networkagent import NetworkAgent, AGENT_MODEL_PATH
from environments.environment import Environment
from environments.status import Status
from support.datakey import DataKey
from support.logger import logger


def main():
    logger.debug('Starting')
    env = Environment()
    agent = NetworkAgent()

    try:
        env.connect()
        env.setup()
        env.start()
        while True:
            env.clear()
            status = Status()
            while status.finished is False:
                data, line = env.pull()
                inp = convert(unpack(data, line))
                out = agent.predict(inp)
                if out is not None:
                    env.put(DataKey.CONTROL_OUT, out)

                status = env.check()
            logger.info(f'~~~ {status} ~~~')
            env.reset()

    finally:
        agent.save(AGENT_MODEL_PATH)
        for actor in env.actors:
            actor.destroy()
        del env  # Forget env after we cleaned up it's actors
        logger.debug('Cleaned up')


if __name__ == '__main__':
    main()
