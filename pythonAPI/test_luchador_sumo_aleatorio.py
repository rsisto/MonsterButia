import butiaAPI
import time
import random

#definicion de constantes
VAL_LIMITE = 470#valor limite para diferenciar entre un negro y un blanco --buscar una mejor manera de hacerlo (automatico o seudo asistida)
VENTANA = 10 #cuanto tiene que contar antes de girar hacia el otro lado
INCSPEED = 20 #cuanto tiene que contar antes de aumentar la velocidad

#variables
contVent = 0
aumentVel = 0

butiabot = butiaAPI.robot()
modulos = butiabot.listarModulos()
print modulos

butiabot.abrirSensor()
butiabot.abrirMotores()

while True:
	sen1 = butiabot.getValSenAnalog("4") #leo el valor del sensor
	print sen1
	if sen1 == "nil value\n" or sen1 == '' or sen1 == " ":
		sen1 = "400"
				
	if sen1 != None and sen1 != "nil value\n" and int(sen1) >= VAL_LIMITE:
		aumentVel = aumentVel + 1
		print aumentVel
		if aumentVel <= INCSPEED:		
			print "avanzo vel 300"
			butiabot.setVelocidadMotores("0","300", "0", "300")
		else:
			print "avanzo vel 600"
			butiabot.setVelocidadMotores("0","600", "0", "600")
	else:
		butiabot.setVelocidadMotores("0", "0", "0", "0") #giro hacia la derecha por el doble del tiempo
		time.sleep(0.5)
		butiabot.setVelocidadMotores("1", "400", "1", "400") #giro hacia la derecha por el doble del tiempo
		time.sleep(2)
		vel = random.randint(200, 1020)
		butiabot.setVelocidadMotores("0", str(vel), "1", str(vel)) #giro hacia la derecha por el doble del tiempo
		time.sleep(random.randint(1, 4))
			
		
