#!/usr/bin/env python3
# !/usr/bin/env python3

from PIL import Image
import socket as socket_lib
import time

from AGVCar.NerualNetworks.ESP_ANN_connection import ConnectionTrainer
from AGVCar.NerualNetworks.Networks.FlatDense import FlatDense
from AGVCar.NerualNetworks.ReinforcementModel import ReinforcementModel
from AGVCar.PythonServer.NetworkExporter import NetworkExporter
from AGVCar.PythonServer.ProcessedState import ProcessedState
from AGVCar.PythonServer.SocketConfig import SocketConfig


class Socket:
    def __init__(self):
        self.config = SocketConfig()
        self.exporter = NetworkExporter()
        self.socket = None
        self.conn = None
        self.addr = None
        self.weights = None
        self.start_time = None
        self.processed_states = ProcessedState()
        self.network = ConnectionTrainer(
            model=ReinforcementModel(dim_features=0, height=96, width=96, n_actions=4, model=FlatDense))
        self.waiting_for_initial_weights = True

    def start(self):
        #self.processed_states.getDataFromDb()
        self.extract_network()
        self.socket = socket_lib.socket(socket_lib.AF_INET, socket_lib.SOCK_STREAM)
        self.socket.bind((self.config.HOST, self.config.PORT))
        self.socket.listen()
        print("Listening")
        self.conn, self.addr = self.socket.accept()
        print('Connected by', self.addr)
        self.start_time = time.time()
        self.start_listening()

    def close(self):
        self.conn.close()
        self.socket.close()

    def start_listening(self):
        while True:
            print("Waiting for data")
            data_raw = self.receive(1)
            data = self.process_data(data_raw)
            print("Request received")

            if not data:
                print("No data")
                break

            self.handle_request(int.from_bytes(data, byteorder='little'))

    def handle_request(self, request_type):
        if self.config.REQ_WAITING_FOR_COMMAND == request_type:
            self.ask_for_command()

    def ask_for_command(self):
        try:
            for k, v in self.config.commands.items():
                print("for {} press {}".format(v, k))

            ##def timeoutFunc():
            ##        self.no_new_command(5)
        ##    print("No new command sent. If you have new commands you can send")

            timeout = 3
            ##t = Timer(timeout, timeoutFunc)
            ##t.start()
            prompt = f"\n If you don't make any command in {timeout} seconds no_new_command will be sent...\n"
            command = int(input(prompt))
            assert 0 <= command < len(self.config.commands)
            eval("self." + self.config.commands[command])(command)
        except (AssertionError, ValueError):
            print("No such Command")
            self.ask_for_command()

    def extract_network(self):
        self.exporter.export_to_ONNX(self.network.Reinforcement_model)
        self.weights = self.exporter.extract_weights()

    def send(self, msg):
        total_sent = 0
        while total_sent < len(msg):
            sent = self.conn.send(msg[total_sent:])
            if sent == 0:
                raise RuntimeError("socket connection broken")
            total_sent = total_sent + sent

    def receive(self, msg_len):
        chunks = []
        bytes_recd = 0
        while bytes_recd < msg_len:
            chunk = self.conn.recv(min(msg_len - bytes_recd, 1024))
            if chunk == b'':
                raise RuntimeError("socket connection broken")
            chunks.append(chunk)
            bytes_recd = bytes_recd + len(chunk)
        return chunks

    @staticmethod
    def process_data(data_array):
        # This function concatenates incoming data which has been received in chunks
        if not len(data_array):
            return None
        else:
            data = data_array[0]
            for i in range(1, len(data_array)):
                data = data + data_array[i]
            return data

    def send_action(self):
        self.send(self.config.SEND_IMAGE.to_bytes(length=1, byteorder='little'))
        response = self.receive(1)
        print(response)

    def get_states(self):
        print("Receiving states")
        data_structured = [self.receive(size) for size in self.config.payload_sizes]
        print("States received")

        processed_data = [self.process_data(data) for data in data_structured]
        self.processed_states.addProcessedData(processed_data)

    def prepare_weights(self):
        print("Preping weights")
        if not self.waiting_for_initial_weights:
            self.network.saveMemory()
            self.network.trainModel()
            self.extract_network()

    def send_weights(self):
        print("Sending weights")
        for i in range(len(self.weights) // self.config.WEIGHTS_CHUNKS):
            self.send(self.weights[
                      i * self.config.WEIGHTS_CHUNKS:(i + 1) * self.config.WEIGHTS_CHUNKS])
        print("Sending last chunk of weights")
        self.send(self.weights[
                  len(self.weights) // self.config.WEIGHTS_CHUNKS * self.config.WEIGHTS_CHUNKS:])

    def stop(self, command_key):
        self.send(command_key.to_bytes(length=1, byteorder='little'))
        self.receive(1)

    def line_following(self, command_key):
        self.send(command_key.to_bytes(length=1, byteorder='little'))
        self.receive(1)

    def send_image(self, command_key):
        self.send(command_key.to_bytes(length=1, byteorder='little'))
        self.receive(1)
        chunks = self.receive(self.config.CAMERA_SIZE)
        image_in_bytes = self.process_data(chunks)
        image = Image.frombytes('L', (96, 96), image_in_bytes, 'raw')
        image.show()

    def wait_for_weights(self, command_key):
        self.send(command_key.to_bytes(length=1, byteorder='little'))
        self.receive(1)
        self.prepare_weights()
        self.send_weights()

    def use_network(self, command_key):
        self.send(command_key.to_bytes(length=1, byteorder='little'))

    def no_new_command(self, command_key):
        self.send(command_key.to_bytes(length=1, byteorder='little'))

    def request_states(self, command_key):
        self.send(command_key.to_bytes(length=1, byteorder='little'))
        self.get_states()


def main():
    server_socket = Socket()
    try:
        server_socket.start()
        server_socket.start_listening()
    except Exception as e:
        print(e)
        server_socket.close()


if __name__ == '__main__':
    main()