# NOT WORKING possibly because pygame dislikes threads

# import random
# from threading import Thread
# import image
# from data import Data
# from key import Key
# import numpy as np
# import pygame
#
#
# class DashboardThread(Thread):
#     data = None
#     colors = []
#     window_size = (None, None)
#     font = None
#     display_surface = None
#
#     def __init__(self, data):
#         super().__init__()
#         self.data = data
#         color_1 = (200, 200, 200)
#         self.colors.append(color_1)
#         color_2 = (100, 200, 100)
#         self.colors.append(color_2)
#         color_3 = (100, 100, 200)
#         self.colors.append(color_3)
#         color_4 = (10, 10, 10)
#         self.colors.append(color_4)
#         self.window_size = (400 + 200, 400)
#
#         pygame.init()
#
#         self.display_surface = pygame.display.set_mode(self.window_size)
#         pygame.display.set_caption('Title')
#         self.font = pygame.font.Font('freesansbold.ttf', 20)
#
#     def run(self):
#         while True:
#             self.draw_image()
#             self.draw_texts()
#
#             # pygame.event.pump()
#             # pygame.event.wait()
#             for event in pygame.event.get():
#                 print(event)
#                 if event.type == pygame.QUIT:
#                     pygame.quit()
#                     return
#             pygame.time.wait(300)
#             pygame.display.update()
#
#     def draw_texts(self):
#         text = self.data.get(Key.CONTROL_OUT)
#         if text is None:
#             text = random.choice(('Empty', 'Test', 'abc123def456'))
#         self.set_text(text, 100)
#         text = self.data.get(Key.SENSOR_ULTRASOUND)
#         if text is None:
#             text = random.choice(('Empty', 'Test', 'abc123def456'))
#         self.set_text(text, 300)
#
#     def draw_image(self):
#         i = self.data.get(Key.SENSOR_CAMERA)
#         self.display_surface.fill(self.colors[0])
#         if i is not None:
#             i = image.resize(i, min(self.window_size), min(self.window_size))
#             i = i * 255 // 1
#             i = np.fliplr(i)
#             i = np.rot90(i)
#             s = pygame.surfarray.make_surface(i)
#             i_rect = pygame.rect.Rect(0, 0, min(self.window_size), min(self.window_size))
#             self.display_surface.blit(s, i_rect)
#
#     def set_text(self, text, height=200):
#         t = self.font.render(str(text), True, self.colors[3])
#         text_rect = t.get_rect()
#         text_rect.center = (500, height)
#         self.display_surface.blit(t, text_rect)
#
#
# if __name__ == '__main__':
#     data = Data()
#     d = DashboardThread(data)
#     i = np.concatenate((random.random() * np.ones([200, 200, 3]), random.random() * np.ones([200, 200, 3])))
#     data.put(Key.SENSOR_CAMERA, i)
#     del i
#     d.start()
