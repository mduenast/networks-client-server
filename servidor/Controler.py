#!/usr/bin/env python
# -*- coding: utf-8 -*-
# vim: set fileencoding=utf-8 :

class Controler(object):
    def __init__(self):
        super(Controler, self).__init__()
        self.nom = None
        self.mac = None
        self.ip = None
        self.random_number = None
        self.estat = "DISCONNECTED"
        self.situacio = None
        self.dispositius = []
        self.tcp_transferencia_dades = None

    def reset_controler(self):
        self.nom = None
        self.mac = None
        self.ip = None
        self.random_number = None
        self.estat = "DISCONNECTED"
        self.situacio = None
        self.dispositius = []
        self.tcp_transferencia_dades = None


class Dispositiu(object):
    def __init__(self, nom_dispositiu=None, valor=0):
        super(Dispositiu, self).__init__()
        self.nom_dispositiu = nom_dispositiu
        self.valor = valor
