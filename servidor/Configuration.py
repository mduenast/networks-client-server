#!/usr/bin/env python
# -*- coding: utf-8 -*-
# vim: set fileencoding=utf-8 :

class Configuration(object):
    def __init__(self, debug=False):
        super(Configuration, self).__init__()
        self.controladors = []
        self.debug = debug
        self.nom = None
        self.mac = None
        self.udp_port = None
        self.tcp_port = None
        self.threads = []


class Element(object):
    def __init__(self):
        self.dispositiu = None
        self.valor = None
