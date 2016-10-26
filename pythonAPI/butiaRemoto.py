#butiaRemoto
import butiaAPI

butiabot = butiaAPI.robot()
modulos = butiabot.listarModulos()
print modulos

butiabot.abrirSensor()
butiabot.abrirMotores()
a = 'q' #no hace nada

while a != 'z':
	a = raw_input("Choose your action f, v, j, k: ")
	
	if a == 'f':
		butiabot.setVelocidadMotores("0","500", "0", "500")	#foreward
	elif a == 'v':
			butiabot.setVelocidadMotores("1","500", "1", "500")	#backward
	elif a == 'j':
				butiabot.setVelocidadMotores("0","400", "1", "400")	#left
	elif a == 'k':
		butiabot.setVelocidadMotores("1","400", "0", "400")	#right	
	else:	
		butiabot.setVelocidadMotores("0","0", "0", "0")	#stop	
		
		
						
		
