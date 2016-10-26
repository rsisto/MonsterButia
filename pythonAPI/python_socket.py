import socket



def iniciar(address = "localhost", puerto = "9999"):
	s = socket.socket()  
	s.connect((address, puerto))
	return s
	
def cerrar(s):
	s.close()
	
def send(s, dato = "hola"):	
	s.send(dato)
	
def read(s):	
	s.read(dato)
	return dato
		
	
