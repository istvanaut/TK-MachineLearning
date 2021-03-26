# @title main
import time

from agents.networkagent import NetworkAgent
from environments.environment import Environment
from environments.status import Status
from support.datakey import DataKey
from support.logger import logger


def main():
    logger.debug('Starting')
    env = Environment()
    agent = NetworkAgent()
    memory = []
    # TODO (10) window to main, using the inp of agent

    try:
        env.connect()
        env.setup()
        agent.load()
        env.start()
        while True:
            env.clear()
            status = Status()

            prev_state = None
            prev_action = None
            while status.finished is False:
                data, line, starting_dir = env.pull()

                state = agent.__class__.convert(agent.__class__.repack(data, line, starting_dir))

                action, out = agent.predict(state)
                if out is not None:
                    env.put(DataKey.CONTROL_OUT, out)

                if prev_state is not None and prev_action is not None and state is not None:
                    memory.append([prev_state, prev_action, state])
                prev_state = state
                prev_action = action

                status = env.check()
            env.reset()

            logger.info(f'~~~ {status} ~~~')
            time.sleep(2.0)

            # logger.info('Continue after any input. To save and exit, type "SAVE". To train on memory, type "TRAIN"')
            # time.sleep(0.2)
            # user_in = input()
            # if user_in == 'SAVE':
            #     agent.save()
            #     return
            # if user_in == 'TRAIN':
            for (prev_state, action, new_state) in memory:
                agent.optimize(new_state, prev_state, action)
            logger.debug('Successfully optimized')
            memory = []
            agent.save()
            # logger.debug('Sleeping...')
            # time.sleep(3.0)

            logger.info('Continuing...')

    finally:
        agent.save()
        for actor in env.actors:
            actor.destroy()
        del env  # Forget env after we cleaned up it's actors
        logger.debug('Cleaned up')


if __name__ == '__main__':
    main()
