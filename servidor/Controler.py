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
        self.dispositius = None
        self.tcp_transferencia_dades = None
