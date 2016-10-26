import butiaAPI
import time


butiabot = butiaAPI.robot()
modulos = butiabot.listarModulos()
if modulos == -1:
	print "Error al listar modulos"
else:	
	print modulos

butiabot.abrirBoton4()
butiabot.abrirSensor()

cont = 0
errcant = 0
error = False
while not error:
	sen1 = butiabot.getBoton()#leo el valor del sensor
	if sen1 == -1:
		error = True
	else:
		print sen1
	#time.sleep(1)
	#sen1 = butiabot.getValSenAnalog("6")

butiabot.cerrar()
