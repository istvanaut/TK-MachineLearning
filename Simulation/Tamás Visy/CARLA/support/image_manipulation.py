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
