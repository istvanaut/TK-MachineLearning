import logging
import threading
import time
import serial
import extractor

def sendWeights():
    """COM3, 115200baud, no timeout, no parity, 1 stop bit"""
    ser = serial.Serial('COM3', 115200, timeout=None, parity=serial.PARITY_NONE, rtscts=1)

    print("waiting for signal")
    ser.read_until(b"ST")
    print("Signal received. Sending data...")
    ser.write(b"OK")
    #ser.flush()
    print(ser.write(extractor.extractWeights()))
    print("Data sent.")

    ser.close()



