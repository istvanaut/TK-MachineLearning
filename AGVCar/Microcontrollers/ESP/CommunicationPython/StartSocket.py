from Microcontrollers.ESP.CommunicationPython.Socket import Socket


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