#!/usr/bin/env python

import time
import socket, sys
import struct

#------------------------------
# main 
#------------------------------
def main():
    stServSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    stServSock.sendto("hello world", ('localhost', 29382))
    stServSock.close()

main()
