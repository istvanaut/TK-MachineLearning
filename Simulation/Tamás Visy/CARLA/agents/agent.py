class Agent:

    def predict(self, state):
        raise NotImplementedError

    def optimize(self, new_state):
        raise NotImplementedError

    def save(self):
        raise NotImplementedError

    def load(self):
        raise NotImplementedError
