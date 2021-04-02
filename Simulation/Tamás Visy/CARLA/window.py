import sensors
from support.datakey import DataKey
import numpy as np
import os

from support.image_manipulation import im_resize, im_color
from support.logger import logger

os.environ['PYGAME_HIDE_SUPPORT_PROMPT'] = 'hide'
import pygame

TITLE = 'Dashboard'
FONT = 'freesansbold.ttf'

QUIT = pygame.QUIT
IM_SIZE = (32, 32)


class Window:
    def __init__(self):
        self.state = None
        self.agent_out = None
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
            self.clear_screen()
            if self.state is not None:
                self.update_screen()

            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    pygame.quit()
                    logger.warning('PyWindow closing')
                    return
            pygame.time.wait(50)
            pygame.display.update()

    def handle(self, state, out):
        self.state = state
        self.agent_out = out

    def add_event(self, event):
        pygame.event.post(pygame.event.Event(event))

    def clear_screen(self):
        self.display.fill(self.colors[0])

    def update_screen(self):
        image, data, names = self.state.get_formatted()

        # TODO (5) remove inserted placeholders
        names.insert(0, '')
        data.insert(0, '')

        names.insert(4, '')
        data.insert(4, '')

        names.append('out')
        data.append(self.agent_out)

        self.draw_image(image)

        count = len(names)
        square_sides = np.ceil(np.sqrt(count))
        # TODO (3) better without + 20?
        side_start = min(self.window_size) + 50
        top_start = 50
        side_step = (max(self.window_size) - min(self.window_size))//square_sides
        top_step = self.window_size[1]//square_sides

        for (i, name) in enumerate(names):
            self.draw_text(data[i], name,
                           (side_start + i // square_sides * side_step, top_start + i % square_sides * top_step))

    def draw_image(self, image):
        if image is None:
            # image = 0.2 * np.ones([100, 100, 3])
            image = 0.4 * np.ones([100, 100])
        # Formatting image
        i = im_resize(image, (min(self.window_size), min(self.window_size)))
        # i = image
        i = i * 255 // 1
        # i = i[:, :, ::-1]  # This fixes color issues: BGR -> RGB
        i = np.fliplr(i)
        i = np.rot90(i)
        i = im_color(i)
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
            # If text is not str or list - show first 5 chars
            text = str(text)
            if len(text) > 5:
                text = text[:5]

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
