from threading import Thread

import datetime

import TCP_channel
from Controler import Controler


class Espera_connexions(object):
    class Tipus_paquets(object):
        tipus_paquets = {"SEND_DATA": 0x20, "SET_DATA": 0x21,
                         "GET_DATA": 0x22, "DATA_ACK": 0x23,
                         "DATA_NACK": 0x24, "DATA_REJ": 0x25}


class Atendre_espera_connexions(Thread):
    def __init__(self, parent=None, pdu=None, address=None, socket_client=None):
        super(Atendre_espera_connexions, self).__init__()
        self.parent = parent
        self.pdu = pdu
        self.address = address
        self.socket_client = socket_client

    def run(self):
        if self.parent.configuracio.debug:
            print "DEBUG => Attenent connexio tcp amb un Thread", self.getName()
        autenticat = False
        controlador_temp = Controler()
        for controlador in self.parent.configuracio.controladors:
            if self.pdu.mac == controlador.mac \
                    and self.pdu.numero_aleatori == controlador.random_number \
                    and self.pdu.dispositiu in controlador.dispositius \
                    and controlador.estat == "SEND_HELLO":
                autenticat = True
                controlador_temp = controlador
                break
        if autenticat:
            if self.parent.configuracio.debug:
                print "DEBUG => Client autenticat"
            # emmagatzamar les dades
            fitxer = None
            try:
                fitxer = open("{0}-{1}.data".format(controlador_temp.nom, controlador_temp.situacio),
                              "a+")
                dt = datetime.datetime.now()
                fitxer.write("{0}-{1}-{2}".format(dt.day, dt.month, dt.year) \
                             + ";{0}-{1}-{2}".format(dt.hour, dt.minute, dt.second) \
                             + ";SEND_DATA" \
                             + ";" + self.pdu.dispositiu \
                             + ";" + self.pdu.valor + "\n")
            except Exception as ex:
                if self.parent.configuracio.debug:
                    print "SEVERE => ", ex
                # enviar DATA_NACK
                pdu = TCP_channel.PDU_TCP(tipus_paquet= \
                                              Espera_connexions.Tipus_paquets.tipus_paquets["DATA_NACK"],
                                          mac=self.parent.configuracio.mac,
                                          numero_aleatori=controlador_temp.random_number,
                                          dispositiu=self.pdu.dispositiu,
                                          valor=self.pdu.valor,
                                          info="Error al emmagatzemar el valor"
                                          )
                packed_data = TCP_channel.PDU_TCP.empaquetar_pdu(pdu)
                self.socket_client.send(packed_data)
                if self.parent.configuracio.debug:
                    print "DEBUG => Enviat ", pdu, " a ", self.address
            finally:
                if fitxer:
                    fitxer.close()
            # enviar reconeixement
            pdu = TCP_channel.PDU_TCP(tipus_paquet= \
                                          Espera_connexions.Tipus_paquets.tipus_paquets["DATA_ACK"],
                                      mac=self.parent.configuracio.mac,
                                      numero_aleatori=controlador_temp.random_number,
                                      dispositiu=self.pdu.dispositiu,
                                      valor=self.pdu.valor,
                                      info="Acceptat"
                                      )
            packed_data = TCP_channel.PDU_TCP.empaquetar_pdu(pdu)
            self.socket_client.send(packed_data)
            if self.parent.configuracio.debug:
                print "DEBUG => Enviat ", pdu, " a ", self.address
        else:
            if self.parent.configuracio.debug:
                print "DEBUG => Client no autenticat"
                # enviar DATA_REJ
                pdu = TCP_channel.PDU_TCP(tipus_paquet= \
                                              Espera_connexions.Tipus_paquets.tipus_paquets["DATA_REJ"],
                                          mac=self.parent.configuracio.mac,
                                          numero_aleatori=controlador_temp.random_number,
                                          dispositiu=self.pdu.dispositiu,
                                          valor=self.pdu.valor,
                                          info="No autenticat"
                                          )
                packed_data = TCP_channel.PDU_TCP.empaquetar_pdu(pdu)
                self.socket_client.send(packed_data)
                if self.parent.configuracio.debug:
                    print "DEBUG => Enviat ", pdu, " a ", self.address
