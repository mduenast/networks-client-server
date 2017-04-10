import sys

import Controler


class Args_parser(object):
    def __init__(self):
        super(Args_parser, self).__init__()
        self.configuration_file = "server.cfg"
        self.controlers_file = "controlers.dat"

    def parse(self, configuration):
        for i in range(len(sys.argv)):
            if sys.argv[i] == "-d":
                configuration.debug = True
            elif sys.argv[i] == "-c":
                self.configuration_file = sys.argv[i + 1]
            elif sys.argv[i] == "-u":
                self.controlers_file = sys.argv[i + 1]
        self.read_configuration_file(configuration)
        if configuration.debug:
            print "DEBUG => ", "Llegit el fitxer de configuracio"
        self.read_controlers_file(configuration)
        if configuration.debug:
            print "DEBUG => ", "Llegit el fitxer de controladors"

    def read_configuration_file(self,configuration):
        with open(self.configuration_file,"r") as file:
            for line in file:
                if(line !='\n'):
                    key = (line.split("=",2)[0]).strip()
                    value = (line.split("=",2)[1]).strip()
                    if key == "Name":
                        configuration.nom = value
                    elif key == "MAC":
                        configuration.mac = value
                    elif key == "UDP-port":
                        configuration.udp_port = value
                    elif key == "TCP-port":
                        configuration.tcp_port = value
                    if configuration.debug:
                        print key, value
                else:
                    break

    def read_controlers_file(self,configuration):
        with open(self.controlers_file,"r") as file:
            for line in file:
                (nom,mac) = ((line.split(",",2)[0]).strip(),\
                             (line.split(",",2)[1]).strip())
                controlador = Controler.Controler()
                controlador.nom = nom
                controlador.mac = mac
                configuration.controladors.append(controlador)
                if configuration.debug:
                    print nom , mac