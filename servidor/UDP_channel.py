import socket
from threading import Thread

import select


class UDP_channel(Thread):
    def __init__(self):
        super(UDP_channel, self).__init__()
        self.configuracio = None
        self.socket_servidor = None
        self.socket_client = None
        self.shutdown = False

    def run(self):
        self.socket_servidor = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
        self.socket_servidor.bind(("localhost", int(self.configuracio.udp_port)))
        if self.configuracio.debug:
            print "DEBUG => ", "Canal UDP obert"
        while not self.shutdown:
            (read_set, write_set, exception_set) = \
                select.select([self.socket_servidor], [], [], 0)
            for fd in read_set:
                print "connexio udp", fd
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
            print "DEBUG => ", "Canal UDP tancat"
