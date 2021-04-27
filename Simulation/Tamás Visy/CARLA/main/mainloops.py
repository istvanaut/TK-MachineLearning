from agents.kerasagent import KerasAgent
from agents.networkagent import NetworkAgent
from agents.omniscientagent import OmniscientAgent
from filters.imagenoisefilter import ImageNoiseFilter
from filters.motornoisefilter import MotorNoiseFilter
from mainsupport import train, do_run
from settings import AGENT_TYPE, AgentTypes, NETWORK_AGENT_MODEL_TYPE, NetworkAgentModelTypes
from support.logger import logger


def normal_main_loop(env, dashboard):
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

    statefilters = [ImageNoiseFilter(st_dev=0.001)]  # st_dev = 0.02 is alright
    outputfilters = [MotorNoiseFilter(st_dev=0.001)]  # st_dev = 0.05 is alright

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


def log_list(li_name, li):
    if li is not None and len(li) > 0:
        logger.info(f'{li_name}:')
        for i, x in enumerate(li):
            logger.info(f'\t{i}. {type(x).__name__}')
    else:
        logger.info(f'No {li_name}')


def train_multiple_main_loop(env, dashboard):
    # TODO (6) filters name, log, show properly
    trainer_agent = OmniscientAgent()

    trainees = [NetworkAgent(NetworkAgentModelTypes.SCNN), KerasAgent()]

    trainer_statefilters = [ImageNoiseFilter(st_dev=0.0001)]  # st_dev = 0.02 is alright
    trainee_statefilters = []  # st_dev = 0.02 is alright
    trainer_outputfilters = [MotorNoiseFilter(st_dev=0.05)]  # st_dev = 0.05 is alright
    trainee_outputfilters = [MotorNoiseFilter(st_dev=0.001)]  # st_dev = 0.05 is alright

    memory = []

    trainer_agent.load()
    for t in trainees:
        t.load()

    logger.info(f'TrainerAgent is {type(trainer_agent).__name__}')
    log_list('Trainees', trainees)
    log_list('Trainer StateFilters', trainer_statefilters)
    log_list('Trainee StateFilters', trainee_statefilters)
    log_list('Trainer OutputFilters', trainer_outputfilters)
    log_list('Trainee OutputFilters', trainee_outputfilters)

    for j in range(10):
        logger.info(f'Starting epoch {j}')
        for i in range(4):
            env.load_path(i)

            traveled = do_run(trainer_agent, env, dashboard, trainer_statefilters, trainer_outputfilters, memory)
            logger.info(f'TrainerAgent finished Course {i}, traveled {traveled} meters')

            dashboard.clear()
            env.reset()
            logger.info('Continuing...')

        for trainee in trainees:
            train(trainee, memory[:])
            for i in range(4):
                env.load_path(i)

                traveled = do_run(trainee, env, dashboard, trainee_statefilters, trainee_outputfilters, [])
                logger.info(f'Trainee {type(trainee).__name__} finished Course {i}, traveled {traveled} meters')
                # TODO (6) give names to agents - using the hash of their model

                dashboard.clear()
                env.reset()
                logger.info('Continuing...')
