from threading import Thread

import copy

import time

import UDP_channel
from Configuration import Configuration
from Controler import Controler


class Manteniment_connexio(object):
    class Tipus_paquets(object):
        tipus_paquets = {"HELLO": 0x10, "HELLO_REJ": 0x11}


class Atendre_Hello(Thread):
    def __init__(self, parent=None, pdu=None, address=None):
        super(Atendre_Hello, self).__init__()
        self.parent = parent
        self.pdu = pdu
        self.address = address

    def run(self):
        autenticat = False
        controlador_temp = Controler()
        # fase de manteniment de la connexio
        if self.parent.configuracio.debug:
            print "DEBUG => Atenent peticio HELLO amb un Thread:", self.getName()
        for controlador in self.parent.configuracio.controladors:
            if (controlador.estat == "SUBSCRIBED" \
                        or controlador.estat == "SEND_HELLO") and \
                            controlador.mac == self.pdu.mac \
                    and self.pdu.numero_aleatori == controlador.random_number \
                    and (self.pdu.dades.split(",", 2)[0].strip())[:8] == controlador.nom \
                    and (self.pdu.dades.split(",", 2)[1].strip())[:12] == controlador.situacio:
                autenticat = True
                controlador_temp = controlador
                break

        if autenticat:
            controlador_temp.hello_perduts = 0
            if self.parent.configuracio.debug:
                print "DEBUG => Client autenticat"
            pdu = UDP_channel.PDU_UDP(tipus_paquet=Manteniment_connexio.Tipus_paquets.tipus_paquets["HELLO"],
                                      mac=self.parent.configuracio.mac,
                                      numero_aleatori=controlador_temp.random_number,
                                      dades=str(controlador_temp.nom + "," + controlador_temp.situacio))
            packed_data = UDP_channel.PDU_UDP.empaquetar_pdu(pdu)
            controlador_temp.estat = "SEND_HELLO"
            self.parent.socket_servidor.sendto(packed_data, self.address)
            if self.parent.configuracio.debug:
                print "DEBUG => Enviat", pdu, " des de ", self.address
                if controlador_temp.estat == "SUBSCRIBED":
                    print "DEBUG => El client passa a estat SEND_HELLO", controlador_temp.nom
        else:
            controlador_temp.estat = "DISCONNECTED"
            controlador_temp.reset_controler()
            if self.parent.configuracio.debug:
                print "DEBUG => Client no autenticat"
                print "DEBUG => Client passa a estat DISCONNECTED", controlador_temp.nom
            pdu = UDP_channel.PDU_UDP(tipus_paquet=Manteniment_connexio.Tipus_paquets.tipus_paquets["HELLO_REJ"],
                                      mac=self.parent.configuracio.mac,
                                      numero_aleatori="00000000",
                                      dades="Dades incorrectes")
            packed_data = UDP_channel.PDU_UDP.empaquetar_pdu(pdu)
            self.parent.socket_servidor.sendto(packed_data, self.address)


class Comprovar_hello_perduts(Thread):
    V = 2
    X = 4

    def __init__(self, parent=None):
        super(Comprovar_hello_perduts, self).__init__()
        self.parent = parent

    def run(self):
        while not self.parent.shutdown:
            for controlador in self.parent.configuracio.controladors:
                if controlador.estat == "SEND_HELLO" or \
                                controlador.estat == "SUBSCRIBED":
                    time.sleep(Comprovar_hello_perduts.V)
                    controlador.hello_perduts += 1
                    if (controlador.hello_perduts % Comprovar_hello_perduts.X) \
                            == 0:
                        controlador.estat == "DISCONNECTED"
                        controlador.reset_controler()
                        if self.parent.configuracio.debug:
                            print "DEBUG => El client passa a estat DISCONNECTED", controlador.nom
