import time

from agents.kerasagent import KerasAgent
from agents.networkagent import NetworkAgent
from agents.converter import convert, repack
from agents.omniscientagent import OmniscientAgent
from environments.carlaenvironment import CarlaEnvironment
from environments.status import Status
from environments.replayenvironment import ReplayEnvironment
from filters.imagenoisefilter import ImageNoiseFilter
from filters.motornoisefilter import MotorNoiseFilter
from settings import *  # This saves us from multiple lines of individual imports
from support.datakey import DataKey
from support.logger import logger
from threads.dashboardthread import DashboardThread


# TODO (10) update control from (steering, vel) to (left-motor, right-motor)
# TODO (8) refactor to carla, network and simulation parts (make difference clearer)


def main():
    logger.debug('main starting')

    if ENVIRONMENT_TYPE is EnvironmentTypes.CARLA:
        env = CarlaEnvironment()
    elif ENVIRONMENT_TYPE is EnvironmentTypes.Replay:
        env = ReplayEnvironment()
    else:
        logger.critical('Unknown environment type in main, raising error')
        raise RuntimeError('Unknown environment type in main')

    if AGENT_TYPE is AgentTypes.Network:
        agent = NetworkAgent(NETWORK_AGENT_MODEL_TYPE)
    elif AGENT_TYPE is AgentTypes.Keras:
        agent = KerasAgent()

        # Converting to TfLite:
        #   agent.load()
        #   agent.save_as_tflite()
    elif AGENT_TYPE is AgentTypes.Omniscient:
        agent = OmniscientAgent()
    else:
        logger.critical('Unknown agent type in main, raising error')
        raise RuntimeError('Unknown agent type in main')

    dashboard = DashboardThread()

    statefilters = [ImageNoiseFilter(st_dev=0.02)]
    outputfilters = [MotorNoiseFilter(st_dev=0.1)]

    memory = []

    log_settings(agent, env, statefilters, outputfilters)

    try:
        env.setup()
        agent.load()

        env.start()
        dashboard.start()
        logger.info('Run starting...')
        while True:
            do_run(env, agent, dashboard, statefilters, outputfilters, memory)

            dashboard.clear()
            env.reset()

            train(agent, memory)
            logger.info('Continuing...')

    finally:
        agent.save()
        if env is not None:
            for actor in env.actors:
                actor.destroy()
            del env  # Make sure to forget env after we cleaned up it's actors
        logger.debug('Cleaned up')


def log_settings(agent, env, statefilters, outputfilters):
    logger.info(f'Train is {TRAIN}')
    logger.info(f'Train per decision is {TRAIN_PER_DECISION}')
    logger.info(f'Environment is {type(env).__name__}')
    logger.info(f'Agent is {type(agent).__name__}')

    if len(statefilters) > 0:
        logger.info('StateFilters:')
        for i, sf in enumerate(statefilters):
            logger.info(f'\t{i}. {type(sf).__name__}')
    else:
        logger.info('No StateFilters')

    if len(outputfilters) > 0:
        logger.info('OutputFilters:')
        for i, of in enumerate(outputfilters):
            logger.info(f'\t{i}. {type(of).__name__}')
    else:
        logger.info('No OutputFilters')


def do_run(env, agent, dashboard, statefilters, outputfilters, memory):
    env.clear()
    status = Status()
    prev_state = None
    prev_action = None

    # This is the loop of a run
    while status.finished is False:
        frame_start = time.time_ns()

        # Get data from environment and format it
        data, path, starting_dir = env.pull()
        state = convert(repack(data, path, starting_dir))

        # Apply filters to state
        for sf in statefilters:
            state = sf.filter(state)

        if TRAIN and TRAIN_PER_DECISION and prev_state is not None:
            agent.optimize(state)

        # Agent predicts
        action, out = agent.predict(state)

        # Apply filters to output
        for of in outputfilters:
            action, out = of.filter(action, out)

        # Update dashboard
        dashboard.handle(state, out)

        # Tell env about the choice
        if out is not None:
            env.put(DataKey.CONTROL_OUT, out)

        # Update memory
        if prev_state is not None and prev_action is not None and state is not None:
            memory.append([prev_state, prev_action, state])
        prev_state = state
        prev_action = action

        apply_frame_time(frame_start)

        status = env.check()
    logger.info(f'{status}')


def apply_frame_time(frame_start):
    frame_end = time.time_ns()
    diff = None
    if frame_end is not None and frame_start is not None:
        diff = (frame_end - frame_start) // 1_000_000
    if diff is not None and diff // 1_000 < TARGET_FRAME_TIME:
        time.sleep(TARGET_FRAME_TIME - diff // 1_000)
    else:
        logger.warning('Frame time issue')


def train(agent, memory):
    # memory is a list, containing (prev_state, prev_action, state) triplets
    if TRAIN and not TRAIN_PER_DECISION:
        if len(memory) >= TRAIN_MEMORY_SIZE:
            logger.info(f'Starting training with memory of {len(memory)}')
            agent.train_on_memory(memory)
            memory.clear()
        else:
            logger.info(f'Memory not full, {len(memory)}/{TRAIN_MEMORY_SIZE}')


if __name__ == '__main__':
    main()
