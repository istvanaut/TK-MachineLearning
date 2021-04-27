from agents.kerasagent import KerasAgent
from agents.networkagent import NetworkAgent
from agents.omniscientagent import OmniscientAgent
from filters.motornoisefilter import MotorNoiseFilter
from mainsupport import train, do_run
from settings import AGENT_TYPE, AgentTypes, NETWORK_AGENT_MODEL_TYPE, NetworkAgentModelTypes
from support.logger import logger


def normal_main_loop(env, dashboard, statefilters, outputfilters):
    if AGENT_TYPE is AgentTypes.Network:
        agent = NetworkAgent(NETWORK_AGENT_MODEL_TYPE)
    elif AGENT_TYPE is AgentTypes.Keras:
        agent = KerasAgent()
    elif AGENT_TYPE is AgentTypes.Omniscient:
        agent = OmniscientAgent()
    else:
        logger.critical('Unknown agent type in main, raising error')
        raise RuntimeError('Unknown agent type in main')
    agent.load()

    memory = []
    i = 0
    while True:
        env.load_path(i)

        traveled = do_run(agent, env, dashboard, statefilters, outputfilters, memory)
        logger.info(f'Agent finished, traveled {traveled} meters')

        dashboard.clear()
        env.reset()

        train(agent, memory)
        logger.info('Continuing...')
        i += 1


def log_agents_for_train_multiple_main_loop(trainer, trainees):
    if trainer is not None:
        logger.info(f'Trainer is {type(trainer).__name__}')
    else:
        logger.info(f'No Trainer is used')

    if len(trainees) > 0:
        logger.info('Trainees:')
        for i, t in enumerate(trainees):
            logger.info(f'\t{i}. {type(t).__name__}')
    else:
        logger.info(f'No Trainees')


def train_multiple_main_loop(env, dashboard, statefilters, outputfilters):
    # TODO (6) filters name, log, show properly
    trainer_agent = OmniscientAgent()

    trainees = [KerasAgent(), NetworkAgent(NetworkAgentModelTypes.SCNN)]

    trainer_agent.load()
    for t in trainees:
        t.load()

    log_agents_for_train_multiple_main_loop(trainer_agent, trainees)

    memory = []
    while True:
        for i in range(4):
            env.load_path(i)

            traveled = do_run(trainer_agent, env, dashboard, statefilters, [MotorNoiseFilter(st_dev=0.15)], memory)
            logger.info(f'TrainerAgent finished Course {i}, traveled {traveled} meters')

            dashboard.clear()
            env.reset()
            logger.info('Continuing...')

        for trainee in trainees:
            train(trainee, memory)
            for i in range(4):
                env.load_path(i)

                traveled = do_run(trainee, env, dashboard, statefilters, outputfilters, memory)
                logger.info(f'Trainee {type(trainee).__name__} finished Course {i}, traveled {traveled} meters')
                # TODO (6) give names to agents - using the hash of their model

                dashboard.clear()
                env.reset()
                logger.info('Continuing...')
