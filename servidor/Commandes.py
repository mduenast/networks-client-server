#!/usr/bin/env python
# -*- coding: utf-8 -*-
# vim: set fileencoding=utf-8 :
import socket
from threading import Thread

import select

import sys

from Espera_connexions import *
from TCP_channel import PDU_TCP


class Commandes(Thread):
    W = 3
    def __init__(self):
        super(Commandes, self).__init__()
        self.shutdown = False
        self.configuracio = None

    def run(self):
        while not self.shutdown:
            (read_set, write_set, exception_set) = \
                select.select([sys.stdin], [], [], 0)
            for fd in read_set:
                if fd is sys.stdin:
                    commanda = raw_input()
                    if commanda == "list":
                        self.list()
                    elif "get" in commanda:
                        try:
                            nom_controlador = commanda.split(" ",3)[1]
                            nom_dispositiu = commanda.split(" ",3)[2]
                            if len(nom_controlador) > 0 and len(nom_dispositiu) > 0:
                                self.get(nom_controlador,nom_dispositiu)
                            else:
                                print "Comanda incorrecta"
                        except Exception as ex:
                            print "Comanda incorrecta"
                            print "SEVERE => ",ex
                    elif "set" in commanda:
                        try:
                            nom_controlador = (commanda.split(" ", 4))[1]
                            nom_dispositiu = (commanda.split(" ", 4))[2]
                            valor = (commanda.split(" ", 4))[3]
                            if len(nom_controlador) > 0 and len(nom_dispositiu) > 0:
                                self.set(nom_controlador, nom_dispositiu,valor)
                            else:
                                print "Comanda incorrecta"
                        except Exception as ex:
                            print "Comanda incorrecta"
                            print "SEVERE => ",ex
                    elif commanda == "quit":
                        self.quit()
                    else:
                        print "Comanda incorrecta"

    def list(self):
        for controlador in self.configuracio.controladors:
            print "*********************************************"
            print "* Nom : ", controlador.nom
            print "* Mac : ", controlador.mac
            print "* Ip : ", controlador.ip
            print "* Numero aleatori : ", controlador.random_number
            print "* Estat : ", controlador.estat
            print "* Situacio : ", controlador.situacio
            for dispositiu in controlador.dispositius:
                print "* Dispositiu : ", str(dispositiu)
            print "*********************************************"

    def get(self, nom_controlador, nom_dispositiu):
        existeix = False
        controlador_temp = None
        for controlador in self.configuracio.controladors:
            if controlador.nom == nom_controlador and \
                            nom_dispositiu in controlador.dispositius:
                existeix = True
                controlador_temp = controlador
                break
        if existeix:
            print "Existeix"
        else:
            print "No existeix"

    def set(self, nom_controlador, nom_dispositiu, valor):
        existeix = False
        controlador_temp = None
        for controlador in self.configuracio.controladors:
            if controlador.nom == nom_controlador and \
                            nom_dispositiu in controlador.dispositius:
                existeix = True
                controlador_temp = controlador
                break
        if existeix:
            print "Existeix"
            pdu = PDU_TCP(tipus_paquet=Espera_connexions.\
                          Tipus_paquets.tipus_paquets["SET_DATA"],
                          mac=str(self.configuracio.mac),
                          numero_aleatori=str(controlador_temp.random_number),
                          dispositiu=str(nom_dispositiu),
                          valor=str(valor),
                          info="set")
            packed_data = PDU_TCP.empaquetar_pdu(pdu)
            print controlador_temp.tcp_transferencia_dades
            socket_client = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
            socket_client.connect((controlador_temp.ip,
                                   int(controlador_temp.tcp_transferencia_dades)))
            socket_client.send(packed_data)
            if self.configuracio.debug:
                print "DEBUG => Envia ",pdu, "des de ",socket_client.getsockname()
            # espera la resposta del client
            (read_set,write_set,exception_set) =\
                select.select([socket_client],[],[],Commandes.W)
            if len(read_set)> 0:
                for fd in read_set:
                    if fd is socket_client:
                        data = socket_client.recv(118)
                        pdu = PDU_TCP.desempaquetar_pdu(data)
                        if self.configuracio.debug:
                            print "DEBUG => Rebut", pdu, "des de ",socket_client.getsockname()
                        # comprovacio del paquet de resposta
                        if pdu.tipus_paquet == \
                            str(Espera_connexions.Tipus_paquets.tipus_paquets["DATA_ACK"]):
                            if pdu.mac == controlador_temp.mac \
                                    and pdu.numero_aleatori == controlador_temp.random_number \
                                and pdu.dispositiu == nom_dispositiu \
                                    and pdu.valor.rstrip(' \t\r\n\0') == valor:
                                self.emmagatzemar_dades(controlador_temp,pdu)
                                if self.configuracio:
                                    print "DEBUG => Dades acceptades"
                            else:
                                controlador_temp.estat = "DISCONNECTED"
                                if self.configuracio.debug:
                                    print "DEBUG => Dades incorrectes"
                                    print "DEBUG => Client passa a estat DISCONNECTED"
                        elif pdu.tipus_paquet == \
                            str(Espera_connexions.Tipus_paquets.tipus_paquets["DATA_NACK"]):
                            if self.configuracio.debug:
                                print "DEBUG => La operacio ha fallat"
                        elif pdu.tipus_paquet == \
                            str(Espera_connexions.Tipus_paquets.tipus_paquets["DATA_REJ"]):
                            controlador_temp.estat = "DISCONNECTED"
                            if self.configuracio.debug:
                                print "DEBUG => La operacio ha fallat"
                                print "DEBUG => Client passa a estat DISCONNECTED"


            else:
                controlador_temp.estat = "DISCONNECTED"
                if self.configuracio.debug:
                    print "DEBUG => Client passa a estat DISCONNECTED"
            socket_client.close()
        else:
            print "No existeix"

    def quit(self):
        for thread in self.configuracio.threads:
            thread.shutdown = True
        print "Sortint ..."
        sys.exit(0)

    def emmagatzemar_dades(self,controlador,pdu):
        # emmagatzamar les dades
        fitxer = None
        try:
            fitxer = open("{0}-{1}.data".format(controlador.nom, controlador.situacio),
                          "a+")
            dt = datetime.datetime.now()
            fitxer.write("{0}-{1}-{2}".format(dt.day, dt.month, dt.year) \
                         + ";{0}-{1}-{2}".format(dt.hour, dt.minute, dt.second) \
                         + ";SEND_DATA" \
                         + ";" + pdu.dispositiu \
                         + ";" + pdu.valor + "\n")
        except Exception as ex:
            if self.parent.configuracio.debug:
                print "SEVERE => ", ex
        finally:
            if fitxer:
                fitxer.close()
