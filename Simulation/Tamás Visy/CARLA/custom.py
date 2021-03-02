# uncomment, run:
# gets the location of the spectator (camera) -> find pos of line!

# # @title main
# # connects to carla client and launches the execution of the task
#
# import glob
# import os
# import sys
# import time
#
# from spectatorpos import set_spec, report
# from task import do
#
# # import random
# # import numpy as np
# # import cv2
# # import time
# try:
#     sys.path.append(glob.glob('../carla/dist/carla-*%d.%d-%s.egg' % (
#         sys.version_info.major,
#         sys.version_info.minor,
#         'win-amd64' if os.name == 'nt' else 'linux-x86_64'))[0])
# except IndexError:
#     print('IndexError while trying to find carla egg')
#     sys.exit()
# try:
#     import carla  # Carla package uses the egg from Carla - Python didn't work
# except ModuleNotFoundError:
#     print('ModuleNotFoundError while importing carla')
#     sys.exit()
#
# HOST = 'localhost'
# PORT = 2000
# MAP_NAME = 'Town01'
#
#
# def main():
#     print('Starting')
#     actors = []
#     try:
#         client = carla.Client(HOST, PORT)
#         client.set_timeout(5.0)  # Even on a local machine, CARLA server takes about 2-3 seconds to respond
#
#         world = client.get_world()
#         # print(dir(world))
#         if world.get_map().name != MAP_NAME:
#             print('Loading map:', MAP_NAME)
#             world = client.load_world(MAP_NAME)
#         else:
#             actors = world.get_actors()
#             for actor in actors.filter('vehicle.*.*'):
#                 actor.destroy()
#             if len(actors.filter('vehicle.*.*')) > 0:
#                 print('Cleaned up old actors')
#             actors = []
#         print('...')
#
#         actors = world.get_actors()
#         for actor in actors.filter('spectator'):
#             # print(dir(actor))
#             set_spec(actor)
#         actors = []
#
#         while True:
#             time.sleep(0.5)
#             report()
#     except RuntimeError as r:
#         print('RuntimeError: ', r.args[0])
#     finally:
#         for actor in actors:
#             actor.destroy()
#         print('Cleaned up')
#
#
# if __name__ == '__main__':
#     main()
