from threading import Thread


class Manteniment_connexio(object):
    class Tipus_paquets(object):
        tipus_paquets = {"HELLO": 0x10, "HELLO_REJ": 0x11}


class Atendre_Hello(Thread):
    V = 2

    def __init__(self, parent=None, pdu=None, address=None):
        super(Atendre_Hello, self).__init__()
        self.parent = parent
        self.pdu = pdu
        self.address = address

    def run(self):
        # fase de manteniment de la connexio
        if self.parent.configuracio.debug:
            print "DEBUG => Atenent peticio SUBS_REQ amb un Thread:", self.getName()

