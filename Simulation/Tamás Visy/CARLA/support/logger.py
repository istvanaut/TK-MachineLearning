import logging

logFormatter = logging.Formatter('%(asctime)s\t  [%(threadName)s]\t[%(levelname)-3.3s]:\t%(message)s',
                                 '%Y.%m.%d %H:%M:%S')
logger = logging.getLogger()
logger.level = logging.INFO

fileHandler = logging.FileHandler('files/carla.log')
# '{0}/{1}.log'.format(logPath, fileName)
fileHandler.setFormatter(logFormatter)

consoleHandler = logging.StreamHandler()
consoleHandler.setFormatter(logFormatter)

logger.addHandler(fileHandler)
logger.addHandler(consoleHandler)
