#!/usr/bin/env python
# -*- coding: utf-8 -*-
# vim: set fileencoding=utf-8 :
import random
import socket
from threading import Thread

import select

from Controler import Controler
import UDP_channel


class Subscripcio(object):
    class Tipus_paquets(object):
        tipus_paquets = {"SUBS_REQ": 0x00, "SUBS_ACK": 0x01, "SUBS_INFO": 0x02,
                         "INFO_ACK": 0x03, "SUBS_NACK": 0x04, "SUBS_REJ": 0x05}


class Atendre_Subs_REQ(Thread):
    S = 2

    def __init__(self, parent=None, pdu=None, address=None):
        super(Atendre_Subs_REQ, self).__init__()
        self.parent = parent
        self.pdu = pdu
        self.address = address
        self.new_socket_udp = None
        self.new_port_udp = None
        self.address_second_socket_udp = None

    def run(self):
        if self.parent.configuracio.debug:
            print "DEBUG => Atenent peticio SUBS_REQ amb un Thread:", self.getName()

        # comprovar que es un controlador autoritzat
        autoritzat = False
        controlador_temp = Controler()
        for controlador in self.parent.configuracio.controladors:
            if controlador.nom == str(self.pdu.dades.split(",", 2)[0]) \
                    and controlador.mac == str(self.pdu.mac) and \
                            self.pdu.numero_aleatori == "00000000":
                if controlador.estat != "DISCONNECTED":
                    return 0
                if len(self.pdu.dades.split(",", 2)[0]) > 0 \
                        and len(self.pdu.dades.split(",", 2)[1]) > 0:
                    controlador.situacio = (str(self.pdu.dades.split(",", 2)[1]))[:12]
                    if self.parent.configuracio.debug:
                        print "DEBUG => Controlador autoritzat"
                    autoritzat = True
                    controlador.ip = str(self.address[0])
                    controlador_temp = controlador
                    break
        # Si el controlador no esta autoritzat es rebutja el paquet
        if not autoritzat:
            if self.parent.configuracio.debug:
                print "DEBUG => Controlador no autoritzat"
            pdu = UDP_channel.PDU_UDP(tipus_paquet= \
                                          Subscripcio.Tipus_paquets.tipus_paquets["SUBS_REJ"],
                                      mac=self.parent.configuracio.mac, numero_aleatori="00000000",
                                      dades="Controlador no autoritzat")
            packed_data = UDP_channel.PDU_UDP.empaquetar_pdu(pdu)
            self.parent.socket_servidor.sendto(packed_data, self.address)

        # Si el controlador esta autoritzat i
        # en un estat legal es prepara la seguent fase
        if controlador_temp.estat == "DISCONNECTED" and autoritzat:
            # generar un numero aleatori per al controlador
            controlador_temp.random_number = str(random.randint(0, 99999999))

            # obrir un nou port udp per a la segona part de la subscripcio

            self.new_socket_udp = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            self.new_socket_udp.bind(('', 0))
            self.new_port_udp = self.new_socket_udp.getsockname()[1]
            if self.parent.configuracio.debug:
                print "DEBUG => Obert un nou socket UDP al port", self.new_port_udp
            # enviament del reconeixement del SUBS_REQ -> SUBS_ACK
            pdu = UDP_channel.PDU_UDP(tipus_paquet= \
                                          Subscripcio.Tipus_paquets.tipus_paquets["SUBS_ACK"],
                                      mac=self.parent.configuracio.mac,
                                      numero_aleatori=controlador_temp.random_number,
                                      dades=str(self.new_port_udp)
                                      )
            packed_data = UDP_channel.PDU_UDP.empaquetar_pdu(pdu)
            self.parent.socket_servidor.sendto(packed_data, self.address)

            # espera a rebre un paquet SUBS_INFO
            (read_set, write_set, exception_set) = select.select([self.new_socket_udp],
                                                                 [], [], Atendre_Subs_REQ.S)
            if len(read_set) > 0:
                for fd in read_set:
                    if fd is self.new_socket_udp:
                        # rebem el paquet SUBS_INFO
                        (data, (ip, port)) = self.new_socket_udp.recvfrom(103)
                        self.address_second_socket_udp = (ip, port)
                        pdu = UDP_channel.PDU_UDP.desempaquetar_pdu(data)
                        if self.parent.configuracio.debug:
                            print "DEBUG => Rebut ", pdu, " des de ", ip, ":", port
                        # comprovar la recepcio de un SUBS_INFO
                        if pdu.tipus_paquet == \
                                str(Subscripcio.Tipus_paquets.tipus_paquets["SUBS_INFO"]):
                            if self.parent.configuracio.debug:
                                print "DEBUG => S'ha rebut un SUBS_INFO"
                            # comprovar les dades del paquet SUBS_INFO
                            if controlador_temp.mac == str(pdu.mac) and \
                                            controlador_temp.random_number == pdu.numero_aleatori:
                                # capturem el port tcp del controlador
                                controlador_temp.tcp_transferencia_dades = \
                                    str(pdu.dades.split(",", 2)[0])
                                # capturem els dispositius
                                dispositius = (pdu.dades.split(",", 2)[1]).split(";")
                                for i in range(len(dispositius) - 1):
                                    controlador_temp.dispositius.append(str(dispositius[i]))
                                if self.parent.configuracio.debug:
                                    print "DEBUG => Dades comprovades"
                                # enviament de reconeixement del SUBS_INFO -> INFO_ACK
                                pdu = UDP_channel.PDU_UDP(tipus_paquet= \
                                                              Subscripcio.Tipus_paquets.tipus_paquets["INFO_ACK"],
                                                          mac=self.parent.configuracio.mac,
                                                          numero_aleatori=controlador_temp.random_number,
                                                          dades=str(self.parent.configuracio.tcp_port))
                                packed_data = UDP_channel.PDU_UDP.empaquetar_pdu(pdu)
                                self.new_socket_udp.sendto(packed_data, self.address_second_socket_udp)
                                controlador_temp.estat = "SUBSCRIBED"
                                if self.parent.configuracio.debug:
                                    print "DEBUG => Envia ", pdu, "des de ", self.address_second_socket_udp
                                    print "DEBUG => El client passa a estat SUBSCRIBED"
                        else:
                            # envia un SUBS_REJ
                            pdu = UDP_channel.PDU_UDP(tipus_paquet= \
                                                          Subscripcio.Tipus_paquets.tipus_paquets["SUBS_REJ"],
                                                      mac=self.parent.configuracio.mac,
                                                      numero_aleatori="00000000",
                                                      dades="Dades incorrectes")
                            packed_data = UDP_channel.PDU_UDP.empaquetar_pdu(pdu)
                            self.new_socket_udp.sendto(packed_data, self.address_second_socket_udp)
                            # passa a estat desconectat i torna a començar un nou proces de subscripcio
                            controlador_temp.estat = "DISCONNECTED"
                            controlador_temp.reset_controler()
                            if self.parent.configuracio.debug:
                                print "DEBUG => El client passa a estat DISCONNECTED"
            else:
                controlador_temp.estat = "DISCONNECTED"
                controlador_temp.reset_controler()
                if self.parent.configuracio.debug:
                    print "DEBUG => El client passa a estat DISCONNECTED"
            # tancar el segon socket udp
            # finalitzada la fase de subscripcio del controlador
            self.new_socket_udp.close()
            if self.parent.configuracio.debug:
                print "DEBUG => Tancat el socket UDP al port", self.new_port_udp
