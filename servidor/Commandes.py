#!/usr/bin/env python
# -*- coding: utf-8 -*-
# vim: set fileencoding=utf-8 :

from threading import Thread

import select

import sys


class Commandes(Thread):
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
                    elif commanda == "get":
                        pass
                    elif commanda == "set":
                        pass
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
                print "* Dispositiu : ", dispositiu.nom_dispositiu, " , Valor : "\
                    ,dispositiu.valor
            print "*********************************************"

    def get(self, nom_controlador, nom_dispositiu):
        pass

    def set(self, nom_controlador, nom_dispositiu, valor):
        pass

    def quit(self):
        for thread in self.configuracio.threads:
            thread.shutdown = True
        print "Sortint ..."
        sys.exit(0)
