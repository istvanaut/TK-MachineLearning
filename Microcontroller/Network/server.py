#!/usr/bin/env python3
#!/usr/bin/env python3

import socket
import extractor
import struct
#import torch
from ESP_ANN_connection import ConnectionTrainer
import export as exporter
import time

waiting_for_initial=True
eac=ConnectionTrainer()

def mysend(conn, msg):
    totalsent = 0
    while totalsent < len(msg):
        sent = conn.send(msg[totalsent:])
        if sent == 0:
            raise RuntimeError("socket connection broken")
        totalsent = totalsent + sent


def myreceive(conn, msglen):
    chunks = []
    bytes_recd = 0
    while bytes_recd < msglen:
        chunk = conn.recv(min(msglen - bytes_recd, 1024))
        if chunk == b'':
            raise RuntimeError("socket connection broken")
        chunks.append(chunk)
        bytes_recd = bytes_recd + len(chunk)
    return chunks


def processdata(dataArray):
    # This function concatenates incoming data which has been received in chunks
    size = len(dataArray)
    if size == 0:
        return None
    else:
        data = dataArray[0]
        for i in range(1, len(dataArray)):
            data = data + dataArray[i]
        return data


#Recieving data:
    #request: 1 byte number
    #states:
        # [0] prevoius states: 28byte
        # [1] current states: 16 bytes
    #camera:
        # [0] previous picture: 1kbyte
        # [1] current picture: 1kbyte

camera = []
states = []

REQUEST_SIZE = 1  # Request size in byte
PAYLOAD_SIZE = 44  # Payload size in byte
CAMERA_SIZE = 2048  # Camera pic size in byte

REQ_STATES = 2
REQ_WEIGHTS = 1
REQ_START_WEIGHTS = 3
REQ_LAST_CHUNK = 4

WEIGHTS_CHUNKS = 1024 # size of weight chunks

sizesPayload = [28,16,1024,1024]

SIZE = REQUEST_SIZE + PAYLOAD_SIZE + CAMERA_SIZE

HOST = '192.168.43.84'  # Standard loopback interface address (localhost)
PORT = 3333        # Port to listen on (non-privileged ports are > 1023)

response = "Picture received, thank you very much!"
response = bytes(response, 'utf-8')

cnt = 0
k = 0
weights = None
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((HOST, PORT))
    s.listen()
    print("Listening")
    conn, addr = s.accept()
    with conn:
        print('Connected by', addr)
        exporter.export_to_ONNX(eac.Reinforcement_model)
        weights = extractor.extractWeights()
        start = time.time()
        while True:

            dataraw = None
            data = None

            print("Waiting for data")
            dataraw = myreceive(conn,1)
            data = processdata(dataraw)
            #print("Request received")

            if REQ_STATES == int.from_bytes(data,byteorder='little'):
                print("receiving states")
                #print(struct.pack('!B',1))
                #conn.send(struct.pack('!B',1))
                #data_structured.append(conn.recv(2100))
                data_structured = []
                for index in range(4):
                    #data.append()
                    index = index + 1
                    print(index)
                    recv = myreceive(conn, sizesPayload[index-1])
                    print(len(recv))
                    data_structured.append(recv)
                    #conn.send(struct.pack('!B',1))
                print("States received")

                print(len(data_structured[0]))
                print(len(data_structured[1]))
                print(len(data_structured[2]))
                print(len(data_structured[3]))
                statesP = processdata(data_structured[0])
                statesC = processdata(data_structured[1])
                imgP = processdata(data_structured[2])
                imgC = processdata(data_structured[3])
                print("statesP length: " + str(len(statesP)))
                print("statesc length: " + str(len(statesC)))
                print("imgP length: " + str(len(imgP)))
                print("imgC length: " + str(len(imgC)))
                camera.clear()
                states.clear()

                #data_structured += bytearray(data)
                lightsensorP = int.from_bytes(statesP[0:4],byteorder='little',signed=False)
                usLeftP = int.from_bytes(statesP[4:8],byteorder='little',signed=False)
                usRightP = int.from_bytes(statesP[8:12],byteorder='little',signed=False)
                laserP = int.from_bytes(statesP[12:16],byteorder='little',signed=False)
                leftM = struct.unpack('f',statesP[16:20])
                rightM = struct.unpack('f',statesP[20:24])
                reward = struct.unpack('f',statesP[24:28])
                print("LightSensor")
                print(lightsensorP)
                print("usLeftP")
                print(usLeftP)
                print("usRightP")
                print(usRightP)
                print("laserP")
                print(laserP)
                print("leftM")
                print(leftM)
                print("rightM")
                print(rightM)
                print("reward")
                print(reward)
                """
                states.append(data_structured[1][32:64])
                states.append(data_structured[1][64:96])
                states.append(data_structured[1][96:128])
                """
                lightsensorC = int.from_bytes(statesC[0:4],byteorder='little',signed=False)
                usLeftC = int.from_bytes(statesC[4:8],byteorder='little',signed=False)
                usRightC = int.from_bytes(statesC[8:12],byteorder='little',signed=False)
                laserC  = int.from_bytes(statesC[12:16],byteorder='little',signed=False)
                
                print("lightsensorC")
                print(lightsensorC)
                print("usLeftC")
                print(usLeftC)
                print("usRightC")
                print(usRightC)
                print("laserC")
                print(laserC)
                print("CameraP:")
                print(list(imgP))
                print("CameraC:")
                print(list(imgC))
                eac.storeData(lightsensorP,
                              usLeftP,
                              usRightP,
                              laserP,
                              list(imgP),
                              leftM,
                              rightM,
                              reward,
                              lightsensorC,
                              usLeftC,
                              usRightC,
                              laserC,
                              list(imgC),
                              time=start-time.time())
            if REQ_START_WEIGHTS == int.from_bytes(data, byteorder='little'):
                print("REQ_START_WEIGHTS")
                cnt = 0
                k = 0
                if not waiting_for_initial:
                    eac.saveMemory()
                    eac.trainModel()
                    exporter.export_to_ONNX(eac.Reinforcement_model)
                    weights = extractor.extractWeights()
                print("Num of weights to send:")
                print(len(weights))
                print("Num of small whole chunks:")
                print(int(len(weights) / WEIGHTS_CHUNKS))
            if REQ_WEIGHTS == int.from_bytes(data, byteorder='little'):
                """print("REQ_WEIGHTS")"""
                    #print(weights[k * WEIGHTS_CHUNKS:(k + 1) * WEIGHTS_CHUNKS])
                mysend(conn, weights[k * WEIGHTS_CHUNKS:(k + 1) * WEIGHTS_CHUNKS])
                if waiting_for_initial:
                    waiting_for_initial = False
                k = k + 1
                cnt += WEIGHTS_CHUNKS
            if REQ_LAST_CHUNK == int.from_bytes(data, byteorder='little'):
                print("REQ_LAST_CHUNK")
                print(int(int(len(weights) / WEIGHTS_CHUNKS) * WEIGHTS_CHUNKS))
                mysend(conn, weights[int(int(len(weights) / WEIGHTS_CHUNKS) * WEIGHTS_CHUNKS):])
            if not data:
                print("No data")
                break

            #conn.sendall(response)


