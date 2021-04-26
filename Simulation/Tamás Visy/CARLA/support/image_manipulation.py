import cv2
import numpy as np


def im_resize(image, size):
    """Returns a new version with given size of a (not normalized) image."""
    if image is None:
        return None
    return cv2.resize(image, size)


def im_grayscale(im):
    """Returns a grayscale version of a (not normalized) BGR image."""
    if im is not None:
        im = im.astype(np.float32)
        im = cv2.cvtColor(im, cv2.COLOR_BGR2GRAY)
        return im
    return None


def im_color(im):
    """Returns a colored version of a (not normalized) grayscale image."""
    if im is not None:
        im = im.astype(np.float32)
        im = cv2.cvtColor(im, cv2.COLOR_GRAY2RGB)
        return im
    return None


def im_fix(im):
    """Returns a fixed version of a normalized image"""
    # Can fix issues when grayscale color is out of bounds
    im = np.where(im < 0.0, 0.0, im)
    im = np.where(im >= 1.0, 1.0, im)
    return im


def im_add_noise(im, mean=0, st_dev=0.1):
    if im is not None:
        gauss = np.random.normal(mean, st_dev, im.shape)
        im_with_noise = im + gauss
        return im_fix(im_with_noise)
    return None
