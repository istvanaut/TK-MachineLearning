# @title main
import time

from agents.networkagent import NetworkAgent
from environments.environment import Environment
from environments.status import Status
from support.datakey import DataKey
from support.logger import logger
from threads.dashboardthread import DashboardThread

TRAIN_PER_DECISION = False
logger.info(f'Train per decision is {TRAIN_PER_DECISION}')
TRAIN_RESOLUTION_PERCENTAGE = 20
logger.info(f'Train resolution is {TRAIN_RESOLUTION_PERCENTAGE}%')
MEMORY_SIZE = 5_000


def main():
    logger.info('Starting')

    env = Environment()
    agent = NetworkAgent()
    dashboard = DashboardThread()
    memory = []

    try:
        env.connect()
        env.setup()
        agent.load()

        env.start()
        dashboard.start()
        while True:
            env.clear()
            status = Status()

            prev_state = None
            prev_action = None
            while status.finished is False:
                data, line, starting_dir = env.pull()

                state = agent.__class__.convert(agent.__class__.repack(data, line, starting_dir))
                if TRAIN_PER_DECISION and prev_state is not None:
                    agent.optimize(state)
                action, out = agent.predict(state)
                dashboard.handle(state, out)
                if out is not None:
                    env.put(DataKey.CONTROL_OUT, out)

                if prev_state is not None and prev_action is not None and state is not None:
                    memory.append([prev_state, prev_action, state])
                prev_state = state
                prev_action = action

                status = env.check()
            logger.info('Finished')
            dashboard.handle(None)
            env.reset()

            logger.info(f'~~~ {status} ~~~')
            time.sleep(2.0)

            if not TRAIN_PER_DECISION:
                if len(memory) > MEMORY_SIZE:
                    logger.info(f'Starting training with memory of {len(memory)}*{TRAIN_RESOLUTION_PERCENTAGE}%')
                    for i, (prev_state, action, new_state) in enumerate(memory):
                        if i % (100 / TRAIN_RESOLUTION_PERCENTAGE) is 0:
                            agent.optimize(new_state, prev_state, action)
                    logger.info('Successfully trained')
                    memory = []
                    agent.model.reset()
                    agent.save()
                else:
                    logger.info(f'Memory not full, {len(memory)}/{MEMORY_SIZE}')
            logger.info('Continuing...')

    finally:
        agent.save()
        for actor in env.actors:
            actor.destroy()
        del env  # Forget env after we cleaned up it's actors
        logger.debug('Cleaned up')


if __name__ == '__main__':
    main()
