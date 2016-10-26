import butiaAPI
import time


butiabot = butiaAPI.robot()
modulos = butiabot.listarModulos()
print modulos

butiabot.abrirSensor()

cont = 0
errcant = 0

while True:
	sen1 = butiabot.getValSenAnalog("4") #leo el valor del sensor
	print sen1
	if sen1 == " " or sen1 == "" or sen1 == "\n" or sen1 == " \n" or sen1 == ' ' or sen1 == '' or sen1 == "nil value\n":
		errcant = errcant + 1
	cont = cont + 1
	print "datos: " + str(cont) + " errores: " + str(errcant)	
	time.sleep(1)
