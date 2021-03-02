import sys
import time

import pygame

import sensors
from key import Key
import numpy as np

TITLE = 'Dashboard'
FONT = 'freesansbold.ttf'

# TODO (1) Fix BGR -> RGB
print('Issue with colors - probably in PyGame - not important')


def create(data):
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

    local_data = (display_surface, window_size, colors, font, small_font)
    while True:
        update_screen(local_data, data)

        for event in pygame.event.get():
            # print(event)
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
        pygame.time.wait(50)
        pygame.display.update()


def update_screen(local_data, data):
    image = data.get(Key.SENSOR_CAMERA)
    draw_image(local_data, image)

    text = data.get(Key.CONTROL_OUT)
    draw_text(local_data, text, 'control', (500, 100))

    text = data.get(Key.SENSOR_RADAR)
    draw_text(local_data, text, 'dist', (500, 200))

    last_collision = data.get(Key.SENSOR_COLLISION)
    r = sensors.recently(last_collision)
    if r is None:
        text = '...'
    elif r is False:
        text = '---'
    elif r is True:
        text = 'COLLISION'
    draw_text(local_data, text, 'coll', (500, 300))

    text = data.get(Key.SENSOR_VELOCITY)
    draw_text(local_data, text, 'vel', (700, 100))

    text = data.get(Key.SENSOR_ACCELERATION)
    draw_text(local_data, text, 'acc', (700, 200))

    text = data.get(Key.SENSOR_POSITION)
    draw_text(local_data, text, 'pos', (700, 300))

    last_obstacle = data.get(Key.SENSOR_OBSTACLE)
    r = sensors.recently(last_obstacle)
    if r is None:
        text = '...'
    elif r is False:
        text = '---'
    elif r is True:
        text = '!!!'
    draw_text(local_data, text, 'obs', (600, 375))


def draw_image(local_data, image):
    display_surface, window_size, colors, font, small_font = local_data
    display_surface.fill(colors[0])
    if image is not None:
        i = sensors.resize(image, min(window_size), min(window_size))
        i = i * 255 // 1
        # i = i[:]
        i = np.fliplr(i)
        i = np.rot90(i)
        s = pygame.surfarray.make_surface(i)
        i_rect = pygame.rect.Rect(0, 0, min(window_size), min(window_size))
        display_surface.blit(s, i_rect)


def draw_text(local_data, text, tag, pos):
    if text is None:
        text = '...'
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
