import numpy as np

from filters.outputfilter import OutputFilter


def fix(out):
    if out[0] < 0.0:
        out[0] = 0.0
    elif out[0] > 1.0:
        out[0] = 1.0

    if out[1] < -1.0:
        out[1] = -1.0
    if out[1] > 1.0:
        out[1] = 1.0
    return out


class MotorNoiseFilter(OutputFilter):
    def __init__(self, mean=0.0, st_dev=0.01):
        self.mean = mean
        self.st_dev = st_dev

    def filter(self, action, out):
        if out is not None:
            out = np.asarray(out)
            noise = np.random.normal(self.mean, self.st_dev, out.shape)
            out = fix(out + noise)
        return action, out
