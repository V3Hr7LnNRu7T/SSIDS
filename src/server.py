#!/usr/bin/python
#server.py
"""
Centralize mesages from workers
"""

import broker

server = broker.Endpoint()
sub_traffic = server.make_subscriber("/robot1/")

server.listen("127.0.0.1", 9999)


while True :
	sub_traffic.get()
