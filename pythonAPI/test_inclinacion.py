import butiaAPI
import time


butiabot = butiaAPI.robot()
modulos = butiabot.listarModulos()
if modulos == -1:
	print "Error al listar modulos"
else:	
	print modulos

cont = 0
errcant = 0
error = False
while not error:
	inclinacion = butiabot.getInclinacion() #leo el valor del sensor de inclinacion
	if inclinacion == -1:
		error = True
	else:
		print inclinacion
	#time.sleep(1)
	#sen1 = butiabot.getValSenAnalog("6")

butiabot.cerrar()
