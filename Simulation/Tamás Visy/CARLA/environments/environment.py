class Environment:

    def setup(self):
        raise NotImplementedError

    def start(self):
        raise NotImplementedError

    def reset(self, do_update_path=True):
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
