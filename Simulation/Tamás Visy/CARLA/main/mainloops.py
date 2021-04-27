from agents.kerasagent import KerasAgent
from agents.networkagent import NetworkAgent
from agents.omniscientagent import OmniscientAgent
from mainsupport import train, do_run
from settings import AGENT_TYPE, AgentTypes, NETWORK_AGENT_MODEL_TYPE
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


def train_multiple_main_loop(env, dashboard, statefilters, outputfilters):
    trainer_agent = OmniscientAgent()

    trainees = [KerasAgent(), NetworkAgent('SCNN')]

    memory = []
    while True:
        for i in range(4):
            env.load_path(i)

            traveled = do_run(trainer_agent, env, dashboard, statefilters, outputfilters, memory)
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
