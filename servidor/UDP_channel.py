#!/usr/bin/env python
# -*- coding: utf-8 -*-
# vim: set fileencoding=utf-8 :
import struct
from struct import *
import socket
from threading import Thread

import select

from Estat import Estats
from Subscripcio import Subscripcio


class UDP_channel(Thread):
    def __init__(self):
        super(UDP_channel, self).__init__()
        self.configuracio = None
        self.socket_servidor = None
        self.socket_client = None
        self.shutdown = False

    def run(self):
        self.socket_servidor = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.socket_servidor.bind(("localhost", int(self.configuracio.udp_port)))
        if self.configuracio.debug:
            print "DEBUG => ", "Canal UDP obert"
        while not self.shutdown:
            (read_set, write_set, exception_set) = \
                select.select([self.socket_servidor], [], [], 0)
            for fd in read_set:
                if fd is self.socket_servidor:
                    (data, (ip, port)) = self.socket_servidor.recvfrom(103)
                    pdu = PDU_UDP.empaquetar_pdu(data)
                    if self.configuracio.debug:
                        print "DEBUG => Rebut ", pdu, "desde ", ip, ":", port
                    self.rebre_paquet(pdu)
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

    def rebre_paquet(self, pdu):
        if pdu.tipus_paquet == str(Subscripcio.Tipus_paquets.tipus_paquets["SUBS_REQ"]):
            for controlador in self.configuracio.controladors:
                if controlador.mac == pdu.mac and \
                    controlador.nom == pdu.dades.split(",", 2)[0] \
                    and pdu.numero_aleatori == "00000000" and\
                    controlador.estat == "DISCONNECTED":
                    print "pene"


class PDU_UDP(object):
    def __init__(self):
        # 1c
        self.tipus_paquet = None
        # 13c
        self.mac = None
        # 9c
        self.numero_aleatori = None
        # 80c
        self.dades = None

    @staticmethod
    def empaquetar_pdu(dades):
        pdu = PDU_UDP()
        unpacked_data = struct.unpack("B13s9s80s", dades)
        pdu.tipus_paquet = str(unpacked_data[0])
        pdu.mac = str(unpacked_data[1])[:-1]
        pdu.numero_aleatori = str(unpacked_data[2])[:-1]
        pdu.dades = str(unpacked_data[3])[:-1]
        return pdu

    def __str__(self):
        return "{" + self.tipus_paquet + " , " + self.mac + " , " + \
               self.numero_aleatori + " , " + self.dades + "}"
