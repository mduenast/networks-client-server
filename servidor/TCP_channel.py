from threading import Thread


class TCP_channel(Thread):
    def __init__(self):
        super(TCP_channel, self).__init__()
        self.configuracio = None

    def run(self):
        if self.configuracio.debug:
            print "DEBUG => ", "Canal TCP obert"
        if self.configuracio.debug:
            print "DEBUG => ", "Canal TCP tancat"

