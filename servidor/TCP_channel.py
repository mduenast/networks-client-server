#!/usr/bin/env python
# -*- coding: utf-8 -*-
# vim: set fileencoding=utf-8 :

import socket
from threading import Thread

import select

import struct


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
            if len(read_set):
                for fd in read_set:
                    if fd is self.socket_servidor:
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


class PDU_TCP(object):
    def __init__(self, tipus_paquet=None, mac=None, numero_aleatori=None,
                 dispositiu=None, valor=None, info=None):
        super(PDU_TCP, self).__init__()
        self.tipus_paquet = tipus_paquet
        self.mac = mac
        self.numero_aleatori = numero_aleatori
        self.dispositiu = dispositiu
        self.valor = valor
        self.info = info

    @staticmethod
    def desempaquetar_pdu(dades):
        pdu = PDU_TCP()
        unpacked_data = struct.unpack("B13s9s8s7s80s", dades)
        pdu.tipus_paquet = str(unpacked_data[0])
        pdu.mac = str(unpacked_data[1])[:-1]
        pdu.numero_aleatori = str(unpacked_data[2])[:-1]
        pdu.dispositiu = str(unpacked_data[3])[:-1]
        pdu.valor = str(unpacked_data[4])[:-1]
        pdu.info = str(unpacked_data[5])[:-1]
        return pdu

    @staticmethod
    def empaquetar_pdu(pdu):
        packed_data = struct.pack("B13s9s8s7s80s", str(pdu.tipus_paquet),
                                  str(pdu.mac), str(pdu.numero_aleatori),
                                  str(pdu.dispositiu), str(pdu.valor),
                                  str(pdu.info))
        return packed_data
