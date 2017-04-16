from threading import Thread


class Espera_connexions(object):
    class Tipus_paquets(object):
        tipus_paquets ={"SEND_DATA":0x20,"SET_DATA":0x21,
                        "GET_DATA":0x22,"DATA_ACK":0x23,
                        "DATA_NACK":0x24,"DATA_REJ":0x25}

class Atendre_espera_connexions(Thread):
    def __init__(self,parent=None,pdu=None,address=None):
        super(Atendre_espera_connexions, self).__init__()
        self.parent = parent
        self.pdu = pdu
        self.address = address
    def run(self):
        if self.parent.configurario.debug:
            print "DEBUG => Attenent connexio tcp amb un Thread",self.getName()

