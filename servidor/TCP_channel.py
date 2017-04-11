import socket
from threading import Thread


class TCP_channel(Thread):
    def __init__(self):
        super(TCP_channel, self).__init__()
        self.configuracio = None
        self.socket_servidor = None
        self.socket_client = None
        self.shutdown = False

    def run(self):
        self.socket_servidor = socket.socket()
        self.socket_servidor.bind(("localhost", int(self.configuracio.tcp_port)))
        self.socket_servidor.listen(len(self.configuracio.controladors))
        if self.configuracio.debug:
            print "DEBUG => ", "Canal TCP obert"
        while not self.shutdown:
            pass
        """
        s = socket.socket()
        s.bind(("localhost", 9999))
        s.listen(1)
        sc, addr = s.accept()
        while True:
              recibido = sc.recv(1024)
              if recibido == "quit":
                 break
              print "Recibido:", recibido
              sc.send(recibido)
        print "adios"
        sc.close()
        s.close()
        """
        self.socket_servidor.close()
        if self.socket_client != None:
            self.socket_client.close()
        if self.configuracio.debug:
            print "DEBUG => ", "Canal TCP tancat"
