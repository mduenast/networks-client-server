#!/usr/bin/env python
# -*- coding: utf-8 -*-
# vim: set fileencoding=utf-8 :

import socket
from threading import Thread

import select


class TCP_channel(Thread):
    def __init__(self):
        super(TCP_channel, self).__init__()
        self.configuracio = None
        self.socket_servidor = None
        self.socket_client = None
        self.shutdown = False

    def run(self):
        self.socket_servidor = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket_servidor.bind(("localhost", int(self.configuracio.tcp_port)))
        self.socket_servidor.listen(len(self.configuracio.controladors))
        if self.configuracio.debug:
            print "DEBUG => ", "Canal TCP obert"
        while not self.shutdown:
            (read_set, write_set, exception_set) = \
                select.select([self.socket_servidor], [], [], 0)
            for fd in read_set:
                if fd is self.socket_servidor:
                    print  "connexio tcp", fd
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
