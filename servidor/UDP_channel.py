#!/usr/bin/env python
# -*- coding: utf-8 -*-
# vim: set fileencoding=utf-8 :
import random
import struct
import socket
from threading import Thread

import select

import sys

from Controler import Dispositiu
from Subscripcio import Subscripcio, Atendre_Subs_REQ


class UDP_channel(Thread):
    def __init__(self):
        super(UDP_channel, self).__init__()
        self.configuracio = None
        self.socket_servidor = None
        self.socket_client = None
        self.shutdown = False

    def run(self):
        self.socket_servidor = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.socket_servidor.bind(('', int(self.configuracio.udp_port)))
        if self.configuracio.debug:
            print "DEBUG => ", "Canal UDP obert"
        while not self.shutdown:
            (read_set, write_set, exception_set) = \
                select.select([self.socket_servidor], [], [], 0)
            if len(read_set) > 0:
                for fd in read_set:
                    if fd is self.socket_servidor:
                        (data, (ip, port)) = \
                            self.socket_servidor.recvfrom(103)
                        #desempaquetar les dades
                        pdu = PDU_UDP.desempaquetar_pdu(data)
                        if self.configuracio.debug:
                            print "DEBUG => ",pdu, " des de ",ip, ":", port
                        #comprova tipus de paquet
                        self.comprovar_tipus_paquet(pdu,(ip,port))
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

    def comprovar_tipus_paquet(self,pdu,address):
        if pdu.tipus_paquet == \
                str(Subscripcio.Tipus_paquets.tipus_paquets["SUBS_REQ"]):
            if self.configuracio.debug:
                print "DEBUG => Rebut un paquet SUBS_REQ"
            atendre_peticio_subs_req = Atendre_Subs_REQ(self,pdu,address)
            atendre_peticio_subs_req.start()
        elif pdu.tipus_paquet == \
                str(Subscripcio.Tipus_paquets.tipus_paquets[""]):
            if self.configuracio.debug:
                print "DEBUG => Rebut un paquet SUBS_REQ"

class PDU_UDP(object):
    def __init__(self, tipus_paquet=None, mac=None, numero_aleatori=None, dades=None):
        # 1c
        self.tipus_paquet = tipus_paquet
        # 13c
        self.mac = mac
        # 9c
        self.numero_aleatori = numero_aleatori
        # 80c
        self.dades = dades

    @staticmethod
    def desempaquetar_pdu(dades):
        pdu = PDU_UDP()
        unpacked_data = struct.unpack("B13s9s80s", dades)
        pdu.tipus_paquet = str(unpacked_data[0])
        pdu.mac = str(unpacked_data[1])[:-1]
        pdu.numero_aleatori = str(unpacked_data[2])[:-1]
        pdu.dades = str(unpacked_data[3])[:-1]
        return pdu

    @staticmethod
    def empaquetar_pdu(pdu):
        packed_data = struct.pack("B13s9s80s", pdu.tipus_paquet, str(pdu.mac),
                                  str(pdu.numero_aleatori), str(pdu.dades))
        return packed_data

    def __str__(self):
        return "{" + str(self.tipus_paquet) + " , " + str(self.mac) + " , " + \
               str(self.numero_aleatori) + " , " + str(self.dades) + "}"
