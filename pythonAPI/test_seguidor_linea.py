import butiaAPI
import time

#definicion de constantes
VAL_LIMITE = 420 #valor limite para diferenciar entre un negro y un blanco --buscar una mejor manera de hacerlo (automatico o seudo asistida)
VENTANA = 5 #cuanto tiene que contar antes de girar hacia el otro lado
INCSPEED = 20 #cuanto tiene que contar antes de aumentar la velocidad

#variables inicialization
contVent = 0
lastFind = 0 # cero = derecha, uno = izquierda. Esto dice hacia donde estaba girando the last time I found black
aumentVel = 0
encontre = False
cont = 0
factor = 0

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
		contVent = 0 #reseteo el factor de creicimiento de la ventana de busqueda de la linea negra
		if aumentVel <= INCSPEED:		
			print "avanzo vel 300"
			butiabot.setVelocidadMotores("0","300", "0", "300")
		else:
			print "avanzo vel 600"
			butiabot.setVelocidadMotores("0","600", "0", "600")
	else:
		#me pongo a buscar a linea negra otra vez
		aumentVel = 0 #reseteo la variable que controla la velocidad
		encontre = False
		cont = 0
        factor = 2
        

		
		while not encontre:
			
			if lastFind == 0:
				print "girando derecha"
                butiabot.setVelocidadMotores("1", "100", "0", "100") #giro hacia la derecha
                
			else:
                print "girando izquierda"
                butiabot.setVelocidadMotores("0", "100", "1", "100") #giro hacia la izquierda 
			cont = cont + 1
            if cont > factor*VENTANA:
                lastFind = not lastFind
                cont = 0
                factor = factor*2
            

			print cont
			sen1 = butiabot.getValSenAnalog("4") #leo el valor del sensor
			print sen1
			if sen1 == "nil value\n" or sen1 == '' or sen1 == " ":
				sen1 = "400"
		
			if sen1 != None and sen1 != "nil value\n" and int(sen1) >= VAL_LIMITE:
				encontre = True
				
