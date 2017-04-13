#!/usr/bin/env python
# -*- coding: utf-8 -*-
# vim: set fileencoding=utf-8 :
import random
import struct
import socket
from threading import Thread

import select

import sys

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
        self.socket_servidor.bind(('', int(self.configuracio.udp_port)))
        if self.configuracio.debug:
            print "DEBUG => ", "Canal UDP obert"
        while not self.shutdown:
            (read_set, write_set, exception_set) = \
                select.select([self.socket_servidor], [], [], 0)
            for fd in read_set:
                if fd is self.socket_servidor:
                    (data, (ip, port)) = self.socket_servidor.recvfrom(103)
                    pdu = PDU_UDP.desempaquetar_pdu(data)
                    if self.configuracio.debug:
                        print "DEBUG => Rebut ", pdu, "desde ", ip, ":", port
                    peticio_udp = Peticio_udp(pdu, self, ip, port)
                    peticio_udp.start()
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


class Peticio_udp(Thread):
    S = 2

    def __init__(self, pdu=None, parent=None, ip=None, port=None):
        super(Peticio_udp, self).__init__()
        self.pdu = pdu
        self.parent = parent
        self.ip = ip
        self.port = port
        self.new_udp_port = None
        self.new_socket_servidor_udp = None
        self.controlador = None

    def run(self):
        self.rebre_paquet(self.pdu, self.ip, self.port)

    def rebre_paquet(self, pdu, ip, port):
        if pdu.tipus_paquet == str(Subscripcio.Tipus_paquets.tipus_paquets["SUBS_REQ"]):
            self.subs_req_response(pdu, ip, port)

    def subs_req_response(self, pdu, ip, port):
        for controlador in self.parent.configuracio.controladors:
            if controlador.mac == pdu.mac and \
                            controlador.nom == pdu.dades.split(",", 2)[0] \
                    and pdu.numero_aleatori == "00000000" and \
                            controlador.estat == "DISCONNECTED":
                self.controlador = controlador

                if self.parent.configuracio.debug:
                    print "DEBUG => Controlador autoritzat"
                random_number = random.randint(0, 99999999)

                # obrir un nou port udp per a la segona part de la subscripcio
                self.new_socket_servidor_udp = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
                self.new_socket_servidor_udp.bind(('localhost', 0))
                self.new_udp_port = self.new_socket_servidor_udp.getsockname()[1]
                if self.parent.configuracio.debug:
                    print "DEBUG => Obert nou port UDP per la subscripcio"
                # enviar reconeixement de la primera part de la subscripcio
                pdu = PDU_UDP(Subscripcio.Tipus_paquets.tipus_paquets["SUBS_ACK"],
                              self.parent.configuracio.mac, random_number,
                              str(self.new_udp_port))
                self.parent.socket_servidor.sendto(PDU_UDP.empaquetar_pdu(pdu),
                                                   (ip, int(port)))
                if self.parent.configuracio.debug:
                    print "DEBUG => Enviat ", pdu

                (read_set, write_set, exception_set) = \
                    select.select([self.new_socket_servidor_udp],
                                  [], [],Peticio_udp.S)
                for fd in read_set:
                    if fd is self.new_socket_servidor_udp:
                        (data, (ip, port)) = self.new_socket_servidor_udp.recvfrom(103)
                        pdu = PDU_UDP.desempaquetar_pdu(data)
                        if self.parent.configuracio.debug:
                            print "DEBUG => Rebut ", pdu, "desde ", ip, ":", port
                        if pdu.tipus_paquet == Subscripcio.Tipus_paquets.tipus_paquets["SUBS_INFO"]:
                            if controlador.mac == pdu.mac  \
                                    and pdu.numero_aleatori == controlador.numero_aleatori and \
                                            controlador.estat == "DISCONNECTED":
                                print "peneee"
                break
            else:
                pdu = PDU_UDP(Subscripcio.Tipus_paquets.tipus_paquets["SUBS_REJ"],
                              self.parent.configuracio.mac, "00000000", "Controlador no autoritzat")
                self.parent.socket_servidor.sendto(PDU_UDP.empaquetar_pdu(pdu), (ip, int(port)))
                if self.parent.configuracio.debug:
                    print "DEBUG => Enviat ", pdu
                controlador.estat = "DISCONNECTED"
                if self.parent.configuracio.debug:
                    print "El client ", controlador.nom, "passa a estat DISCONNECTED"
                break
