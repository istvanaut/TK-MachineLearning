import time

from agents.converter import convert, repack
from environments.status import Status
from settings import TRAIN_PER_DECISION, TRAIN, TARGET_FRAME_TIME, TRAIN_MEMORY_SIZE
from support.datakey import DataKey
from support.logger import logger


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
        if TRAIN_MEMORY_SIZE is None or len(memory) >= TRAIN_MEMORY_SIZE:
            logger.info(f'Starting training with memory of {len(memory)}')
            agent.train_on_memory(memory)
            memory.clear()
        else:
            logger.info(f'Memory not full, {len(memory)}/{TRAIN_MEMORY_SIZE}')