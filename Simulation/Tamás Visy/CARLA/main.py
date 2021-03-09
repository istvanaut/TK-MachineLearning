# @title main
# connects to carla client and launches the execution of the task
import logging
import subprocess
import time

import carla_wrapper
from environment import Environment, TASK_FOLLOW_LINE, TASK_CREATE_LINE

# TODO (8) write some documentation
from support.logger import logger

HOST = 'localhost'  # 'carla.office.creo.hu'  # 'localhost'
PORT = 2000
# Town05 has best paths on the inside of the motorway (still in the town)
MAP_NAME = 'Town02'  # Best map: Town05 or Town03, least performance demanding map: Town02
GAME_PATH = 'C:/Games/CARLA/CarlaUE4.exe -ResX=200 -ResY=200 -quality-level=Low'


def main():
    logger.debug('Starting')
    env = Environment()

    try:
        client, world = connect()

        if client is None or world is None:
            launch_sim()
            client, world = connect()
        while client is None or world is None:
            time.sleep(2.0)
            client, world = connect()

        world = set_conditions(client, world)
        env.set_world(world)

        # env.start(TASK_FOLLOW_LINE)

        env.start(TASK_FOLLOW_LINE)

    finally:
        for actor in env.get_actors():
            actor.destroy()
        del env  # Forget env after we cleaned up it's actors
        logger.debug('Cleaned up')


def set_conditions(client, world):
    current_map_name = world.get_map().name
    if current_map_name != MAP_NAME:
        logger.info(f'Loading map: {MAP_NAME} <- {current_map_name}')
        world = client.load_world(MAP_NAME)
    else:
        actors = world.get_actors()
        for actor in actors.filter('vehicle.*.*'):
            actor.destroy()
        if len(actors.filter('vehicle.*.*')) > 0:
            logger.info('Cleaned up old actors')
    set_weather(world)
    return world


def set_weather(world):
    weather = world.get_weather()
    weather.precipitation = 0.0
    weather.precipitation_deposits = 0.0
    weather.wetness = 0.0
    world.set_weather(weather)
    logger.info('Applied nice weather')


def connect():
    try:
        logger.info(f'Connecting to {HOST}:{PORT}...')
        client = carla_wrapper.client(HOST, PORT)
        client.set_timeout(5.0)  # Even on a local machine, CARLA server takes about 2-3 seconds to respond
        world = client.get_world()
    except RuntimeError as r:
        logger.error(f'Could not connect to server: {r}')
        return None, None
    return client, world


def launch_sim():
    logger.warning('Not launching simulator')
    return  # when main closes sim does too TODO (3) decide to keep this or abandon
    logger.info('Launching Carla server in 3 seconds...')
    time.sleep(3.0)
    try:
        logger.debug(f'Launching executable at {GAME_PATH}')
        subprocess.Popen(GAME_PATH, shell=False, stdin=None, stdout=None, stderr=None,
                         close_fds=True, creationflags=subprocess.DETACHED_PROCESS)
    except RuntimeError:
        raise RuntimeError('Could not launch executable')
    logger.info('Waiting for environment to load')
    time.sleep(10.0)


if __name__ == '__main__':
    main()
