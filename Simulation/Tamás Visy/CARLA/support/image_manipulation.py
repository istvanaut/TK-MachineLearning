import cv2
import numpy as np

from support.logger import logger


def im_resize(image, size):
    if image is None:
        return None
    return cv2.resize(image, size)


def im_grayscale(im):
    if im is not None:
        im = im_denormalize(im)
        im = im.astype(np.float32)
        logger.debug(np.shape(im))
        im = cv2.cvtColor(im, cv2.COLOR_BGR2GRAY)
        im = im_normalize(im)
        return im
    return None


def im_denormalize(im):
    if im is not None:
        return im / 255
    return None


def im_normalize(im):
    if im is not None:
        return im / 255
    return None
