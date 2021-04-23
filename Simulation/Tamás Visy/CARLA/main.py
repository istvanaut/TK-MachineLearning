import time

from agents.kerasagent import KerasAgent
from agents.networkagent import NetworkAgent
from agents.converter import convert, repack
from environments.carlaenvironment import CarlaEnvironment
from environments.status import Status
from environments.replayenvironment import ReplayEnvironment
from settings import *  # This saves us from multiple lines of individual imports
from support.datakey import DataKey
from support.logger import logger
from threads.dashboardthread import DashboardThread

logger.warning(f'Train is {TRAIN}')
logger.warning(f'Train per decision is {TRAIN_PER_DECISION}')

# TODO (10) update control from (steering, vel) to (left-motor, right-motor)


def main():
    logger.info('Starting')

    if ENVIRONMENT_TYPE is EnvironmentTypes.CARLA:
        env = CarlaEnvironment()
    elif ENVIRONMENT_TYPE is EnvironmentTypes.Test:
        env = ReplayEnvironment()
    else:
        logger.critical('Unknown environment type in main, raising error')
        raise RuntimeError('Unknown environment type in main')
    logger.info(f'Environment is {type(env).__name__}')

    if AGENT_TYPE is AgentTypes.Network:
        agent = NetworkAgent(NETWORK_AGENT_MODEL_TYPE)
    elif AGENT_TYPE is AgentTypes.Keras:
        agent = KerasAgent()

        # Converting to TfLite:
        #   agent.load()
        #   agent.save_as_tflite()
    else:
        logger.critical('Unknown agent type in main, raising error')
        raise RuntimeError('Unknown agent type in main')
    logger.info(f'Agent is {type(agent).__name__}')

    dashboard = DashboardThread()
    memory = []
    run_index = 0

    try:
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
                frame_start = time.time_ns()

                data, path, starting_dir = env.pull()

                state = convert(repack(data, path, starting_dir))

                if TRAIN and TRAIN_PER_DECISION and prev_state is not None:
                    agent.optimize(state)

                # TODO (9) probably remove cheating
                #  apply noise out here
                action, out = agent.predict(state, pure=pure(run_index), auto=not pure(run_index))
                dashboard.handle(data, path, starting_dir, state, out, pure=pure(run_index))

                if out is not None:
                    env.put(DataKey.CONTROL_OUT, out)

                if prev_state is not None and prev_action is not None and state is not None:
                    memory.append([prev_state, prev_action, state])
                prev_state = state
                prev_action = action

                apply_frame_time(frame_start)
                status = env.check()
            logger.info('Finished')
            dashboard.clear()
            env.reset()

            if pure(run_index):
                logger.info('This run was pure')
            logger.info(f'~~~ {status} ~~~')
            time.sleep(1.0)

            if TRAIN and not TRAIN_PER_DECISION:
                if len(memory) >= TRAIN_MEMORY_SIZE:
                    logger.info(f'Starting training with memory of {len(memory)}')
                    agent.train_on_memory(memory)
                    memory = []
                else:
                    logger.info(f'Memory not full, {len(memory)}/{TRAIN_MEMORY_SIZE}')
            logger.info('Continuing...')
            run_index += 1

    finally:
        agent.save()
        for actor in env.actors:
            actor.destroy()
        del env  # Forget env after we cleaned up it's actors
        logger.debug('Cleaned up')


def apply_frame_time(frame_start):
    frame_end = time.time_ns()
    diff = None
    if frame_end is not None and frame_start is not None:
        diff = (frame_end - frame_start) // 1_000_000
    if diff is not None and diff // 1_000 < TARGET_FRAME_TIME:
        time.sleep(TARGET_FRAME_TIME - diff // 1_000)
    else:
        logger.warning('Frame time issue')


if __name__ == '__main__':
    main()
