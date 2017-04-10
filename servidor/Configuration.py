class Configuration(object):

    def __init__(self,debug=False):
        super(Configuration, self).__init__()
        self.controladors = []
        self.debug = debug
        self.nom = ""
        self.mac = ""
        self.udp_port = ""
        self.tcp_port = ""

