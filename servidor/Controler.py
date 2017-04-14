#!/usr/bin/env python
# -*- coding: utf-8 -*-
# vim: set fileencoding=utf-8 :

class Controler(object):
    def __init__(self):
        super(Controler, self).__init__()
        self.nom = ""
        self.mac = ""
        self.ip = ""
        self.random_number = ""
        self.estat = "DISCONNECTED"
        self.situacio = ""
        self.dispositius = []
        self.tcp_transferencia_dades = None

    def reset_controler(self):
        self.ip = ""
        self.random_number = ""
        self.estat = "DISCONNECTED"
        self.situacio = ""
        self.dispositius = []
        self.tcp_transferencia_dades = None


class Dispositiu(object):
    def __init__(self, nom_dispositiu="", valor=0):
        super(Dispositiu, self).__init__()
        self.nom_dispositiu = nom_dispositiu
        self.valor = valor
