 
 # Authors:	Andres Aguirre, Pablo Gindel, Jorge Visca, Gonzalo Tejera, Santiago Margini, Federico Andrade
 # MINA Group, Facultad de Ingenieria, Universidad de la Republica, Uruguay.
 # 
 # License at file end. 
 
# This program requires the installation of the package python-ctypes
import ctypes

# Imports serialcomm library for Linux
libc = ctypes.CDLL("./serialcomm.so")

# Non blocking send over the serial port
# Parameter data is the data to be sent
# Parameter device is the name of the serial port (e.g. "/dev/tty.usbserial","COM1")
def sendData(data, device):
	libc.send_data.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
	res_send = libc.send_data.restype = ctypes.c_int
	res_send = libc.send_data(data, device)
	return res_send

def receiveData(data, device, length):
	libc.write_data.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_int]
	res_receive = libc.receive_data.restype = ctypes.c_int
	res_reveive = libc.receive_data(data, device, length)
	return res_receive

# Python bindings for serialcomm
# Copyright 2010 MINA Group, Facultad de Ingenieria, Universidad de la
# Republica, Uruguay. 
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
# CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
# TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
# SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
