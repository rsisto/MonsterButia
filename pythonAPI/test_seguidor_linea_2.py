import butiaAPI
import time

#definicion de constantes
VAL_LIMITE = 160 #valor limite para diferenciar entre un negro y un blanco --buscar una mejor manera de hacerlo (automatico o seudo asistida)
VENTANA = 10 #cuanto tiene que contar antes de girar hacia el otro lado
INCSPEED = 30 #cuanto tiene que contar antes de aumentar la velocidad
GIROIZQ = 0
GIRODER = 1
AVANLEN = 2
AVANRAP = 3

#variables inicialization
lastFind = 0 # cero = derecha, uno = izquierda. Esto dice hacia donde estaba girando the last time I found black
aumentVel = 0
encontre = False
cont = 0
factor = 0
estado = 0

butiabot = butiaAPI.robot()
modulos = butiabot.listarModulos()
print modulos

butiabot.abrirSensor()
butiabot.abrirMotores()

print raw_input("Calibrando blanco, enter para continuar ")
sen1 = butiabot.getValSenAnalog("4") #leo el valor del sensor
if sen1 != None and sen1 != "nil value\n" and sen1 != " \n":
	BLANCO = int(sen1)
	
print raw_input("Calibrando negro, enter para continuar ")
sen1 = butiabot.getValSenAnalog("4") #leo el valor del sensor
if sen1 != None and sen1 != "nil value\n" and sen1 != " \n":
	NEGRO = int(sen1)	

VAL_LIMITE = (BLANCO + NEGRO) // 2
print "Blanco: " + str(BLANCO) + " Negro: " + str(NEGRO)
print "Limit value: " + str(VAL_LIMITE)
print raw_input("Enter para continuar...")

while True:
	sen1 = butiabot.getValSenAnalog("4") #leo el valor del sensor
	print sen1
	
	if sen1 == "nil value\n" or sen1 == '' or sen1 == " ":
		sen1 = VAL_LIMITE - 10
		
	if sen1 != None and sen1 != "nil value\n" and int(sen1) >= VAL_LIMITE:
		print "avanzando..."
		aumentVel = aumentVel + 1
		print aumentVel
		if aumentVel <= INCSPEED and estado != AVANLEN:		
			print "avanzo vel 800"
			butiabot.setVelocidadMotores("0","400", "0", "400")
			estado = AVANLEN
		else:
			if aumentVel > INCSPEED and estado != AVANRAP:		
				print "avanzo vel 800"
				butiabot.setVelocidadMotores("0","800", "0", "800")
				estado = AVANRAP
	else:
		aumentVel = 0 #reseteo la variable que controla la velocidad
		encontre = False
		factor = 2
		cont = 0
		while (not encontre):			
			print "searching black line..."
			if lastFind == 0 and estado != GIRODER:
				print "girando derecha"
				butiabot.setVelocidadMotores("1", "300", "0", "300") #giro hacia la derecha
				estado = GIRODER
			else:
				if lastFind == 1 and estado != GIROIZQ:
					print "girando izquierda"
					butiabot.setVelocidadMotores("0", "300", "1", "300") #giro hacia la izquierda 
					estado = GIROIZQ
			cont = cont + 1
			if cont > factor*VENTANA:
				lastFind = not lastFind
				cont = 0
				factor = factor*2
			print cont
			sen1 = butiabot.getValSenAnalog("4") #leo el valor del sensor
			print sen1
			if sen1 == "nil value\n" or sen1 == '' or sen1 == " ":
				sen1 = VAL_LIMITE - 10		
			if sen1 != None and sen1 != "nil value\n" and int(sen1) >= VAL_LIMITE:
				encontre = True
