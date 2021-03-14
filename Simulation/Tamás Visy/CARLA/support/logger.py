import logging

logFormatter = logging.Formatter('%(asctime)s\t  [%(threadName)s]\t[%(levelname)-3.3s]:\t%(message)s',
                                 '%Y.%m.%d %H:%M:%S')
logger = logging.getLogger('carla_logger')
logger.level = logging.DEBUG

file_found = True
try:
    fileHandler = logging.FileHandler('files/carla.log')
except FileNotFoundError:
    fileHandler = logging.FileHandler('carla.log')

fileHandler.setFormatter(logFormatter)

consoleHandler = logging.StreamHandler()
consoleHandler.setFormatter(logFormatter)

logger.addHandler(fileHandler)
logger.addHandler(consoleHandler)

if fileHandler.baseFilename.find('files\\carla.log') == -1:
    logger.error('Log parent directory \'files\' not found')
