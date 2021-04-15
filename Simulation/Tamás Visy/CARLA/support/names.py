# Python 3.8+ implementation - would it work as a function?
# f'{var=}'.split('=')[0]

from support.logger import logger


def nameof(var, outside_locals):
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

    logger.error(f'Did not find name of var {var} in nameof()')
    return None
