# @title main

from agents.networkagent import NetworkAgent
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
        agent.load()
        env.start()
        while True:
            env.clear()
            status = Status()
            while status.finished is False:
                data, line = env.pull()
                inp = agent.__class__.convert(agent.__class__.unpack(data, line))
                out = agent.predict(inp)
                if out is not None:
                    env.put(DataKey.CONTROL_OUT, out)

                status = env.check()
            logger.info(f'~~~ {status} ~~~')

            user_inp = input('...')
            if user_inp is 'SAVE':
                agent.save()

            env.reset()

    finally:
        agent.save()
        for actor in env.actors:
            actor.destroy()
        del env  # Forget env after we cleaned up it's actors
        logger.debug('Cleaned up')


if __name__ == '__main__':
    main()
