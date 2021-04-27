from mainsupport import train, do_run
from support.logger import logger


def normal_main_loop(agent, env, dashboard, statefilters, outputfilters):
    memory = []
    i = 0
    while True:
        env.load_path(i)

        do_run(env, agent, dashboard, statefilters, outputfilters, memory)

        dashboard.clear()
        env.reset()

        train(agent, memory)
        logger.info('Continuing...')
        i += 1
