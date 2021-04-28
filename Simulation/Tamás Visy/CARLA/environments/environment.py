class Environment:

    def setup(self):
        raise NotImplementedError

    def start(self):
        raise NotImplementedError

    def reset(self):
        raise NotImplementedError

    def clear(self):
        raise NotImplementedError

    def pull(self):
        raise NotImplementedError
        # return data, path, starting_direction

    def put(self, key, data):
        raise NotImplementedError

    def check(self):
        raise NotImplementedError
        # return a Status

    def load_path(self, i):
        raise NotImplementedError
