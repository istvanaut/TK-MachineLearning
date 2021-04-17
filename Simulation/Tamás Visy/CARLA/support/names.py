# Python 3.8+ implementation - would it work as a function?
# f'{var=}'.split('=')[0]
from collections import Iterable

from support.logger import logger


def nameof(var, outside_locals):
    if var is None:
        logger.debug(f'Not searching for name of None var in nameof()')
        return 'None'
    if issubclass(type(var), Iterable) and all(v is None for v in var):
        logger.debug(f'Not searching for name of Nones var in nameof()')
        return 'Nones'
    for string, val in outside_locals.items():
        if type(var) == type(val) and var == val:
            return string
        elif isinstance(val, object):
            try:
                string = nameof(var, val.__dict__)
                if string is not None:
                    return string
            except AttributeError:
                pass

    logger.warning(f'Did not find name of var {var} in nameof()')
    return None
