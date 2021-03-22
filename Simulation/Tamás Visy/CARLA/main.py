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
                data, line, starting_dir = env.pull()
                inp = agent.__class__.convert(agent.__class__.repack(data, line, starting_dir))
                out = agent.predict(inp)
                if out is not None:
                    env.put(DataKey.CONTROL_OUT, out)

                status = env.check()
            env.reset()

            logger.info(f'~~~ {status} ~~~')

            user_inp = input('Continue after any input. To save and continue, type "SAVE"')
            if user_inp is 'SAVE':
                agent.save()

    finally:
        agent.save()
        for actor in env.actors:
            actor.destroy()
        del env  # Forget env after we cleaned up it's actors
        logger.debug('Cleaned up')


if __name__ == '__main__':
    main()
