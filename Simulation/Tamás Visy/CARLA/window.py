import sys

import sensors
from support.datakey import DataKey
import numpy as np
import os

from support.logger import logger

os.environ['PYGAME_HIDE_SUPPORT_PROMPT'] = 'hide'
import pygame

TITLE = 'Dashboard'
FONT = 'freesansbold.ttf'

QUIT = pygame.QUIT


class Window:
    def __init__(self, data):
        self.data = data
        colors = []
        color_1 = (200, 200, 200)
        colors.append(color_1)
        color_2 = (100, 200, 100)
        colors.append(color_2)
        color_3 = (100, 100, 200)
        colors.append(color_3)
        color_4 = (10, 10, 10)
        colors.append(color_4)
        window_size = (400 + 200 + 200, 400)

        pygame.init()

        display_surface = pygame.display.set_mode(window_size)
        pygame.display.set_caption(TITLE)
        font = pygame.font.Font(FONT, 20)
        small_font = pygame.font.Font(FONT, 10)

        self.local_data = (display_surface, window_size, colors, font, small_font)

    def work(self):
        """Attention! Contains infinite loop"""
        while True:
            update_screen(self.local_data, self.data)

            for event in pygame.event.get():
                # print(event)
                if event.type == pygame.QUIT:
                    pygame.quit()
                    logger.warning('PyWindow closing')
                    return
            pygame.time.wait(50)
            pygame.display.update()

    def add_event(self, event):
        pygame.event.post(pygame.event.Event(event))


def update_screen(local_data, data):
    image = data.get(DataKey.SENSOR_CAMERA)
    draw_image(local_data, image)

    text = data.get(DataKey.CONTROL_OUT)
    draw_text(local_data, text, 'control', (500, 100))

    radar = data.get(DataKey.SENSOR_RADAR)
    text = sensors.limit_range(radar)
    draw_text(local_data, text, 'dist', (500, 200))

    last_collision = data.get(DataKey.SENSOR_COLLISION)
    text = sensors.recently(last_collision)
    if text is True:
        text = 'COLLISION'
    draw_text(local_data, text, 'coll', (500, 300))

    text = data.get(DataKey.SENSOR_VELOCITY)
    draw_text(local_data, text, 'vel', (700, 100))

    text = data.get(DataKey.SENSOR_ACCELERATION)
    draw_text(local_data, text, 'acc', (700, 200))

    text = data.get(DataKey.SENSOR_POSITION)
    draw_text(local_data, text, 'pos', (700, 300))

    last_obstacle = data.get(DataKey.SENSOR_OBSTACLE)
    text = sensors.recently(last_obstacle)
    if text is True:
        text = '!!!'
    draw_text(local_data, text, 'obs', (600, 375))


def draw_image(local_data, image):
    display_surface, window_size, colors, font, small_font = local_data
    display_surface.fill(colors[0])
    if image is not None:
        i = sensors.resize(image, min(window_size), min(window_size))
        i = i * 255 // 1
        i = i[:, :, ::-1]  # This probably fixed color issues
        # cv2.cvtColor(i, cv2.COLOR_BGR2RGB)  # change color scheme # Error: 'Unsupported depth of input image ...'
        i = np.fliplr(i)
        i = np.rot90(i)
        s = pygame.surfarray.make_surface(i)
        i_rect = pygame.rect.Rect(0, 0, min(window_size), min(window_size))
        display_surface.blit(s, i_rect)


def draw_text(local_data, text, tag, pos):
    if text is None:
        text = '...'
    elif text is False:
        text = '---'
    elif type(text) is str:
        pass
    elif isinstance(text, list) or isinstance(text, tuple):
        # If text is a list - show first 3 char's of first 3 items
        t = []
        for i in range(min(len(text), 4)):
            t.append(str(text[i])[:5])
            t.append(' ')
        text = ''.join(t)
    else:
        # If text is not str or list - show first 4 chars
        text = str(text)
        if len(text) > 4:
            text = text[:4]
    display_surface, window_size, colors, font, small_font = local_data

    text = font.render(text, True, colors[3])
    text_rect = text.get_rect()
    text_rect.center = pos
    display_surface.blit(text, text_rect)

    text = small_font.render(tag, True, colors[3])
    text_rect = text.get_rect()
    text_rect.center = (pos[0], pos[1] - 20)
    display_surface.blit(text, text_rect)
