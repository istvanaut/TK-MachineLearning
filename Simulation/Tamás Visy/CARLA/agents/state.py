def feature_dimension():
    return 12


class State:

    def __init__(self, image=None, radar=None, collision=None, velocity=None, acceleration=None, position=None,
                 direction=None, obstacle=None, distance_from_line=None):
        self.image = image
        self.radar = radar
        self.collision = collision
        self.velocity = velocity
        self.acceleration = acceleration
        self.position = position
        self.direction = direction
        self.obstacle = obstacle
        self.distance_from_line = distance_from_line

    def get_formatted(self):
        if self.acceleration is None:
            acc = [None, None, None]
        else:
            acc = self.acceleration
        if self.position is None:
            pos = [None, None, None]
        else:
            pos = self.position
        return self.image, \
               [self.radar, self.collision, self.velocity, acc[0], acc[1], acc[2], pos[0], pos[1], pos[2],
                self.direction, self.obstacle, self.distance_from_line], \
               ['radar', 'collision', 'velocity', 'accX', 'accY', 'accZ', 'posX', 'posY', 'posZ',
                'direction', 'obstacle', 'distance_from_line']
