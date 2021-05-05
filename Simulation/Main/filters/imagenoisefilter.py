from filters.statefilter import StateFilter
from support.image_manipulation import im_add_noise


class ImageNoiseFilter(StateFilter):
    def __init__(self, mean=0.0, st_dev=0.02):
        self.mean = mean
        self.st_dev = st_dev

    def filter(self, state):
        if state is not None:
            state.image = im_add_noise(state.image, mean=self.mean, st_dev=self.st_dev)
        return state
