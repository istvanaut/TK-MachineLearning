# @title main
import random
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

TRAIN_PER_DECISION = False
logger.info(f'Train per decision is {TRAIN_PER_DECISION}')
TRAIN_RESOLUTION_PERCENTAGE = 100
logger.info(f'Train resolution is {TRAIN_RESOLUTION_PERCENTAGE}%')
TARGET_FRAME_TIME = 0.25  # 0.025
MEMORY_SIZE = 10 # 128 + (10 * (1 / TARGET_FRAME_TIME)) // 1


def main():
    logger.info('Starting')

    env = Environment()
    # agent = NetworkAgent()
    agent = KerasAgent()
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

            logger.info('Starting...')
            while status.finished is False:
                frame_start = time.time_ns()

                data, line, starting_dir = env.pull()

                state = convert(repack(data, line, starting_dir))
                if TRAIN_PER_DECISION and prev_state is not None:
                    agent.optimize(state)
                action, out = agent.predict(state)
                dashboard.handle(data, line, starting_dir, state, out)
                if out is not None:
                    env.put(DataKey.CONTROL_OUT, out)

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

            logger.info(f'~~~ {status} ~~~')
            time.sleep(1.0)

            if not TRAIN_PER_DECISION:
                if len(memory) >= MEMORY_SIZE:
                    logger.info(f'Starting training with memory of {len(memory)}*{TRAIN_RESOLUTION_PERCENTAGE}%')
                    # train_networkagent(agent, memory)
                    train_kerasagent(agent, memory)
                    memory = []
                else:
                    logger.info(f'Memory not full, {len(memory)}/{MEMORY_SIZE}')
            logger.info('Continuing...')

    finally:
        agent.save()
        for actor in env.actors:
            actor.destroy()
        del env  # Forget env after we cleaned up it's actors
        logger.debug('Cleaned up')


def train_kerasagent(agent, memory):
    states = [prev_state for (prev_state, action, new_state) in memory]
    agent.train(states)


def train_networkagent(agent, memory):
    x = 0
    r = [[], []]
    for i, (prev_state, action, new_state) in enumerate(memory):
        if i % (100 // TRAIN_RESOLUTION_PERCENTAGE) is 0:
            x += 1
            reward = agent.optimize(new_state, prev_state, action)
            r[action].append(reward)
    logger.info(f'Successfully trained {x} times')
    for i, action_rewards in enumerate(r):
        logger.info(f'Action rewards (ID, AVG, AMOUNT) '
                    f'-:- {i}; {np.average(action_rewards)}; {len(action_rewards)}')
    agent.model.reset()
    agent.save()


if __name__ == '__main__':
    main()
