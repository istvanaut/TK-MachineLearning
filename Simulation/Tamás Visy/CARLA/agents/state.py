from support.names import nameof


def get_feature_dimension():
    return len(State().get_formatted()[1])


class State:

    def __init__(self, image=None, radar=None, collision=None, velocity=None, acceleration=None, position=None,
                 direction=None, obstacle=None, distance_from_path=None, side=None):
        self.image = image
        self.radar = radar
        self.collision = collision
        self.velocity = velocity
        self.acceleration = acceleration
        self.position = position
        self.direction = direction
        self.obstacle = obstacle
        self.distance_from_path = distance_from_path
        self.side = side

    def print(self):
        if self.acceleration is None:
            acc = [None, None, None]
        else:
            acc = self.acceleration
        print([self.radar, self.collision, self.velocity, acc[0], acc[1], acc[2],
               self.direction, self.obstacle, self.distance_from_path * self.side])

    def get_formatted(self):
        if self.acceleration is None:
            acc = [None, None, None]
        else:
            acc = self.acceleration
        if self.position is None:
            pos = [None, None, None]
        else:
            pos = self.position
        return (self.image,
                [self.radar, self.collision, self.velocity,
                 acc[0], acc[1], acc[2],
                 pos[0], pos[1], pos[2],
                 self.direction, self.obstacle,
                 self.distance_from_path, self.side],
                [nameof(self.radar, locals()), nameof(self.collision, locals()), nameof(self.velocity, locals()),
                 f'{nameof(acc, locals())}X', f'{nameof(acc, locals())}Y', f'{nameof(acc, locals())}Z',
                 f'{nameof(pos, locals())}X', f'{nameof(pos, locals())}Y', f'{nameof(pos, locals())}Z',
                 nameof(self.direction, locals()), nameof(self.obstacle, locals()),
                 nameof(self.distance_from_path, locals()), nameof(self.side, locals())])
