class SocketConfig:
    def __init__(self):
        self.REQUEST_SIZE = 1  # Request size in byte
        self.PAYLOAD_SIZE = 44  # Payload size in byte
        self.CAMERA_SIZE = 2048  # Camera pic size in byte
        self.TOTAL_SIZE = self.REQUEST_SIZE + self.PAYLOAD_SIZE + self.CAMERA_SIZE

        # Request types
        self.REQ_WEIGHTS = 1
        self.REQ_STATES = 2
        self.REQ_START_WEIGHTS = 3
        self.REQ_LAST_CHUNK = 4

        self.WEIGHTS_CHUNKS = 1024  # size of weight chunks

        self.payload_sizes = [28, 16, 1024, 1024]

        self.HOST = '127.0.0.1'  # Standard loopback interface address (localhost)
        self.PORT = 3333  # Port to listen on (non-privileged ports are > 1023)
