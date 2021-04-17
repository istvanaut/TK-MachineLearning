# @title main
import numpy as np
import time

from agents.kerasagent import KerasAgent
from agents.networkagent import NetworkAgent
from agents.state import State, convert, repack
from environments.environment import Environment
from environments.status import Status
from support.datakey import DataKey
from support.logger import logger
from threads.dashboardthread import DashboardThread

TRAIN = True
logger.warning(f'Train is {TRAIN}')
TRAIN_PER_DECISION = False
logger.warning(f'Train per decision is {TRAIN_PER_DECISION}')
TRAIN_RESOLUTION_PERCENTAGE = 100
logger.info(f'Train resolution is {TRAIN_RESOLUTION_PERCENTAGE}%')
TARGET_FRAME_TIME = 0.25  # 0.025
MEMORY_SIZE = 1024  # 128 + (10 * (1 / TARGET_FRAME_TIME)) // 1


# TODO (8) extract settings and add make it not sync

def pure(run):
    return True
    return run % 2


def main():
    logger.info('Starting')

    env = Environment()
    # agent = NetworkAgent()
    agent = KerasAgent()
    dashboard = DashboardThread()
    memory = []
    run = 0

    try:
        env.connect()
        env.setup()
        agent.load()

        env.start()
        dashboard.start()
        logger.info('Starting...')
        while True:
            env.clear()
            status = Status()

            prev_state = None
            prev_action = None

            while status.finished is False:
                # TODO (6) extract this
                frame_start = time.time_ns()

                data, path, starting_dir = env.pull()

                state = convert(repack(data, path, starting_dir))
                if TRAIN and TRAIN_PER_DECISION and prev_state is not None:
                    agent.optimize(state)
                action, out = agent.predict(state, pure=pure(run), auto=not pure(run))
                dashboard.handle(data, path, starting_dir, state, out, pure=pure(run))
                if out is not None:
                    env.put(DataKey.CONTROL_OUT, out)
                # TODO (8) direction -1 broken somewhere
                # TODO (6) change order things when of ending a run - why?
                if prev_state is not None and prev_action is not None and state is not None:
                    memory.append([prev_state, prev_action, state])
                prev_state = state
                prev_action = action

                frame_end = time.time_ns()
                diff = None
                if frame_end is not None and frame_start is not None:
                    diff = (frame_end - frame_start) // 1_000_000
                if diff is not None and diff // 1_000 < TARGET_FRAME_TIME:
                    time.sleep(TARGET_FRAME_TIME - diff // 1_000)
                status = env.check()
            logger.info('Finished')
            dashboard.clear()
            env.reset()

            if pure(run):
                logger.info('This run was pure')
            logger.info(f'~~~ {status} ~~~')
            time.sleep(1.0)

            if TRAIN and not TRAIN_PER_DECISION:
                if len(memory) >= MEMORY_SIZE:
                    logger.info(f'Starting training with memory of {len(memory)}*{TRAIN_RESOLUTION_PERCENTAGE}%')
                    agent.train_on_memory(memory)
                    memory = []
                else:
                    logger.info(f'Memory not full, {len(memory)}/{MEMORY_SIZE}')
            logger.info('Continuing...')
            run += 1

    finally:
        agent.save()
        for actor in env.actors:
            actor.destroy()
        del env  # Forget env after we cleaned up it's actors
        logger.debug('Cleaned up')


if __name__ == '__main__':
    main()
