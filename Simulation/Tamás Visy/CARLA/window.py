import numpy as np
import os

from settings import TITLE, FONT
from support.image_manipulation import im_resize, im_color
from support.logger import logger

os.environ['PYGAME_HIDE_SUPPORT_PROMPT'] = 'hide'
import pygame

QUIT = pygame.QUIT
# TODO (7) make window less ugly


class Window:
    def __init__(self):
        self.data = None
        self.path = None
        self.starting_dir = None
        self.state = None
        self.agent_out = None
        self.pure = False
        self.i = 0
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
            self.i += 1
            pygame.time.wait(50)
            pygame.display.update()

    def clear(self):
        self.handle(None, None, None, None, None, False)

    def handle(self, data, path, starting_dir, state, out, pure):
        self.data = data
        self.path = path
        self.starting_dir = starting_dir
        self.state = state
        self.agent_out = out
        self.pure = pure

    def add_event(self, event):
        pygame.event.post(pygame.event.Event(event))

    def clear_screen(self):
        self.display.fill(self.colors[0])

    def update_screen(self):
        image, data, names = self.state.get_formatted()

        self.insert_placeholder(data, names)
        names.insert(4, 'some kind of cheaty drive')
        data.insert(4, (not self.pure))

        names.append('out')
        data.append(self.agent_out)

        self.draw_image(image)

        count = len(names)
        square_sides = np.ceil(np.sqrt(count))
        # TODO (3) better without + 50?
        side_start = min(self.window_size) + 50
        top_start = 50
        side_step = (max(self.window_size) - min(self.window_size))//square_sides
        top_step = self.window_size[1]//square_sides

        for (i, name) in enumerate(names):
            self.draw_text(data[i], name,
                           (side_start + i // square_sides * side_step, top_start + i % square_sides * top_step))

    def insert_placeholder(self, data, names):
        # TODO (5) remove inserted placeholder
        names.insert(0, 'Placeholder')
        if self.i % 8 is 0:
            data.insert(0, '---')
        if self.i % 8 is 1:
            data.insert(0, '>--')
        elif self.i % 8 is 2:
            data.insert(0, '->-')
        elif self.i % 8 is 3:
            data.insert(0, '-->')
        elif self.i % 8 is 4:
            data.insert(0, '---')
        elif self.i % 8 is 5:
            data.insert(0, '--<')
        elif self.i % 8 is 6:
            data.insert(0, '-<-')
        elif self.i % 8 is 7:
            data.insert(0, '<--')

    def draw_image(self, image):
        if image is None:
            image = 0.4 * np.ones([100, 100])
        # Formatting image
        i = im_resize(image, (min(self.window_size), min(self.window_size)))
        i = i * 255 // 1
        # Fixing directions
        i = np.fliplr(i)
        i = np.rot90(i)
        i = im_color(i)
        # Drawing image
        s = pygame.surfarray.make_surface(i)
        i_rect = pygame.rect.Rect(0, 0, min(self.window_size), min(self.window_size))
        self.display.blit(s, i_rect)

    def draw_text(self, text, tag, pos):
        if text is None or text is 0.0:
            text = '.N.'
        elif text is False or text is -1.0:
            text = '-F-'
        elif text is True or text is 1.0:
            text = '~T~'
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
            # Example when needed: text = 4.232e-9 ===> not 4.23... but 0.0...
            try:
                text = np.round(text, 4)
            except RuntimeError:
                pass
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
