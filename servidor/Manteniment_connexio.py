from threading import Thread

import copy

import UDP_channel
from Controler import Controler


class Manteniment_connexio(object):
    class Tipus_paquets(object):
        tipus_paquets = {"HELLO": 0x10, "HELLO_REJ": 0x11}


class Atendre_Hello(Thread):
    V = 2
    X = 4

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
            print "----",controlador.situacio,len(controlador.situacio),len(self.pdu.dades.split(",", 2)[1].strip())
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
            if self.parent.configuracio.debug:
                print "DEBUG => Client autenticat"
            pdu = UDP_channel.PDU_UDP(tipus_paquet=Manteniment_connexio.Tipus_paquets.tipus_paquets["HELLO"],
                                      mac=self.parent.configuracio.mac,
                                      numero_aleatori=controlador_temp.random_number,
                                      dades=str(controlador_temp.nom + "," + controlador_temp.situacio))
            packed_data = UDP_channel.PDU_UDP.empaquetar_pdu(pdu)
            controlador_temp.estat == "SEND_HELLO"
            self.parent.socket_servidor.sendto(packed_data, self.address)
            if self.parent.configuracio.debug:
                print "DEBUG => Enviat", pdu, " des de ", self.address
                if controlador_temp.estat == "SUBSCRIBED":
                    print "DEBUG => El client passa a estat SEND_HELLO"
        else:
            controlador_temp.estat = "DISCONNECTED"
            controlador_temp.reset_controler()
            if self.parent.configuracio.debug:
                print "DEBUG => Client no autenticat"
                print "DEBUG => Client passa a estat DISCONNECTED"
            pdu = UDP_channel.PDU_UDP(tipus_paquet=Manteniment_connexio.Tipus_paquets.tipus_paquets["HELLO_REJ"],
                                      mac=self.parent.configuracio.mac,
                                      numero_aleatori="00000000",
                                      dades="Dades incorrectes")
            packed_data = UDP_channel.PDU_UDP.empaquetar_pdu(pdu)
            self.parent.socket_servidor.sendto(packed_data, self.address)
