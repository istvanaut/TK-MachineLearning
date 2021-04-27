from environments.carlaenvironment import CarlaEnvironment
from environments.replayenvironment import ReplayEnvironment
from mainloops import normal_main_loop, train_multiple_main_loop
from settings import ENVIRONMENT_TYPE, EnvironmentTypes, TRAIN, TRAIN_PER_DECISION
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

    dashboard = DashboardThread()

    log_settings(env)

    try:
        env.setup()
        env.start()
        dashboard.start()

        # normal_main_loop(env, dashboard)
        train_multiple_main_loop(env, dashboard)

    finally:
        if env is not None:
            for actor in env.actors:
                actor.destroy()
            del env  # Make sure to forget env after we cleaned up it's actors
        logger.debug('Cleaned up')


def log_settings(env):
    logger.info(f'Train is {TRAIN}')
    logger.info(f'Train per decision is {TRAIN_PER_DECISION}')
    logger.info(f'Environment is {type(env).__name__}')


if __name__ == '__main__':
    main()
