from threading import Thread


class UDP_channel(Thread):
    def __init__(self):
        super(UDP_channel, self).__init__()
        self.configuracio = None

    def run(self):
        if self.configuracio.debug:
            print "DEBUG => ", "Canal UDP obert"
        if self.configuracio.debug:
            print "DEBUG => ", "Canal UDP tancat"