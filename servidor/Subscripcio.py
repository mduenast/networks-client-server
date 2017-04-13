#!/usr/bin/env python
# -*- coding: utf-8 -*-
# vim: set fileencoding=utf-8 :

class Subscripcio(object):
    class Tipus_paquets(object):
        tipus_paquets = {"SUBS_REQ": 0x00, "SUBS_ACK": 0x01, "SUBS_INFO": 0x02,
                         "INFO_ACK": 0x03, "SUBS_NACK": 0x04, "SUBS_REJ": 0x05}
