import sensors
from support.datakey import DataKey
import numpy as np
import os

from support.image_manipulation import im_resize
from support.logger import logger

os.environ['PYGAME_HIDE_SUPPORT_PROMPT'] = 'hide'
import pygame

TITLE = 'Dashboard'
FONT = 'freesansbold.ttf'

QUIT = pygame.QUIT
IM_SIZE = (32, 32)


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

        self.display = display_surface
        self.window_size = window_size
        self.colors = colors
        self.fonts = [font, small_font]

    def work(self):
        # Attention! Contains infinite loop
        while True:
            if self.data.get(DataKey.THREAD_HALT):
                self.clear_screen()
            else:
                self.update_screen()

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

    def clear_screen(self):
        self.draw_image(None)

        self.draw_text(None, 'control', (500, 100))

        self.draw_text(None, 'dist', (500, 200))

        self.draw_text(None, 'coll', (500, 300))

        self.draw_text(None, 'vel', (700, 100))

        self.draw_text(None, 'acc', (700, 200))

        self.draw_text(None, 'pos', (700, 300))

        self.draw_text(None, 'obs', (600, 375))

    def update_screen(self):
        image = self.data.get(DataKey.SENSOR_CAMERA)
        image = im_resize(image, IM_SIZE)
        self.draw_image(image)

        text = self.data.get(DataKey.CONTROL_OUT)
        self.draw_text(text, 'control', (500, 100))

        radar = self.data.get(DataKey.SENSOR_RADAR)
        text = sensors.limit_range(radar)
        self.draw_text(text, 'dist', (500, 200))

        last_collision = self.data.get(DataKey.SENSOR_COLLISION)
        text = sensors.recently(last_collision)
        if text is True:
            text = 'COLLISION'
        self.draw_text(text, 'coll', (500, 300))

        text = self.data.get(DataKey.SENSOR_VELOCITY)
        self.draw_text(text, 'vel', (700, 100))

        text = self.data.get(DataKey.SENSOR_ACCELERATION)
        self.draw_text(text, 'acc', (700, 200))

        text = self.data.get(DataKey.SENSOR_POSITION)
        self.draw_text(text, 'pos', (700, 300))

        last_obstacle = self.data.get(DataKey.SENSOR_OBSTACLE)
        text = sensors.recently(last_obstacle)
        if text is True:
            text = '!!!'
        self.draw_text(text, 'obs', (600, 375))

    def draw_image(self, image):
        if image is None:
            image = 0.2 * np.ones([100, 100, 3])
        self.display.fill(self.colors[0])
        # Formatting image
        i = im_resize(image, (min(self.window_size), min(self.window_size)))
        i = i * 255 // 1
        i = i[:, :, ::-1]  # This fixes color issues: BGR -> RGB
        i = np.fliplr(i)
        i = np.rot90(i)
        # Drawing image
        s = pygame.surfarray.make_surface(i)
        i_rect = pygame.rect.Rect(0, 0, min(self.window_size), min(self.window_size))
        self.display.blit(s, i_rect)

    def draw_text(self, text, tag, pos):
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

        # Rendering text
        text = self.fonts[0].render(text, True, self.colors[3])
        text_rect = text.get_rect()
        text_rect.center = pos
        self.display.blit(text, text_rect)

        # Rendering tag
        tag = self.fonts[1].render(tag, True, self.colors[3])
        tag_rect = tag.get_rect()
        tag_rect.center = (pos[0], pos[1] - 20)
        self.display.blit(tag, tag_rect)
