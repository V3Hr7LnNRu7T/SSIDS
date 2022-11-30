#!/usr/bin/python
#serveur.py
"""
Centralize mesages from workers
"""

import broker

serveur = broker.Endpoint()
sub_traffic = serveur.make_subscriber("/robot1/")

serveur.listen("127.0.0.1", 9999)


while True :
	sub_traffic.get()
