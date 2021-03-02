# @title main
# connects to carla client and launches the execution of the task

import glob
import os
import subprocess
import sys
import time

from task import do

# import random
# import numpy as np
# import cv2
# import time
try:
    sys.path.append(glob.glob('../carla/dist/carla-*%d.%d-%s.egg' % (
        sys.version_info.major,
        sys.version_info.minor,
        'win-amd64' if os.name == 'nt' else 'linux-x86_64'))[0])
except IndexError:
    print('IndexError while trying to find carla egg')
    sys.exit()
try:
    import carla  # Carla package uses the egg from Carla - Python didn't work
except ModuleNotFoundError:
    print('ModuleNotFoundError while importing carla')
    sys.exit()

HOST = 'localhost'
PORT = 2000
# Town05 has best paths
MAP_NAME = 'Town02'  # Best map: Town05 or Town03, least performance demanding map: Town02
GAME_PATH = 'C:/Games/CARLA/WindowsNoEditor/CarlaUE4.exe -ResX=200 -ResY=200 -quality-level=Low'


def main():
    print('Starting')
    actors = []

    try:
        client, world = connect()

        world = set_conditions(client, world)

        do(world, actors)
    finally:
        for actor in actors:
            actor.destroy()
        print('Cleaned up')


def set_conditions(client, world):
    current_map_name = world.get_map().name
    if current_map_name != MAP_NAME:
        print(f'Loading map: {MAP_NAME} <- {current_map_name}')
        world = client.load_world(MAP_NAME)
    else:
        actors = world.get_actors()
        for actor in actors.filter('vehicle.*.*'):
            actor.destroy()
        if len(actors.filter('vehicle.*.*')) > 0:
            print('Cleaned up old actors')
    set_weather(world)
    return world


def set_weather(world):
    weather = world.get_weather()
    weather.precipitation = 0.0
    weather.precipitation_deposits = 0.0
    weather.wetness = 0.0
    world.set_weather(weather)
    print('Applied nice weather!')


def connect():
    try:
        client, world = connect_client()
    except RuntimeError:
        print('Could not connect to client. Trying to launch Carla server...')
        try:
            exec_independently(GAME_PATH)
        except RuntimeError:
            raise RuntimeError('Could not launch executable')
        try:
            print('Waiting for environment to load (about 10 sec)')
            time.sleep(10.0)
            client, world = connect_client()
        except RuntimeError:
            raise RuntimeError('Could not connect to client on 2nd try')
    return client, world


def exec_independently(path):
    print(f'Launching executable at {path}')
    subprocess.Popen(path, shell=False, stdin=None, stdout=None, stderr=None,
                     close_fds=True, creationflags=subprocess.DETACHED_PROCESS)


def connect_client():
    print('Connecting...')
    client = carla.Client(HOST, PORT)
    client.set_timeout(5.0)  # Even on a local machine, CARLA server takes about 2-3 seconds to respond
    world = client.get_world()
    return client, world


if __name__ == '__main__':
    main()
