#VERSION 1.0
import apiSumoUY
import random
import math

#constant definition
#estados del juego
REPOSICIONAR = 0
START = 1
STOP = 2
#constante usada por el path planning (metodo setCmds)
DIST = 200
ANG_1 = 100
ANG_2 = 45
MAX_VEL = 5

misPuntos = 0
susPuntos = 0
ganador = False #controla que no haya errores durante la ejecucion
error = False



# Distancia entre dos puntos en dimension 2.
#	 * @param x1 Coordenada X del punto 1.
#	 * @param y1 Coordenada Y del punto 1.
#	 * @param x2 Coordenada X del punto 2.
#	 * @param y2 Coordenada Y del punto 2.
#	 * @return La distancia entre dos puntos.
def dist (x1, y1, x2, y2):
	x = x2 - x1
	y = y2 - y1
	return math.sqrt(x*x + y*y)


#	 * Normalizacion del un angulo en grados al intervalo (-180,180].
#	 * @param ang Angulo a normalizar.
#	 * @return El angulo normalizado en el intervalo (-180,180].
def norm (ang):
	while (ang > 180):
		ang = ang - 360
	while (ang <= -180):
		ang = ang + 360
	return ang


#	 * Metodo creado con el objetivo de encapsular codigo y encargado de setear los miembros de clase
#	 * vIzq y vDer. Asi, este metodo es utilizado por el metodo de path planning void setCmds(...). Dada
#	 * entonces el angulo diferencia entre los puntos (x0,y0) y (xf,yf) del path planning, se determina
#	 * que comando (parametros vMax y vMin) sera enviado a que rueda del robot (vIzq y vDer). 
#	 * 
#	 * @param dif Angulo diferencia del path planning en el intervalo (-180,180].
#	 * @param vMax Velocidad maxima a ser seteada en una de las ruedas del robot.
#	 * @param vMin Velodiad minima a ser seteada en una de las ruedas del robot.
def setVels(dif, vMax, vMin):
	
	if (dif > 0):
		
		api.enviarVelocidades(int(vMin), int(vMax))
	else:
		api.enviarVelocidades(int(vMax), int(vMin))


#	 * Metodo que implementa el path planning utilizado por el robot jugador. El objetivo general
#	 * de cualquier metodo de path planning es, dado un objeto movil (en nuestro caso, un robot
#	 * jugador de sumo), con determinado estado inicial (en nuestro caso, posicion y rotacion del
#	 * robot), calcular los comandos necesarios para que dicho objeto pase a cierto estado final 
#	 * deseado (en nuestro caso, una posicion final).
#	 * <br><br>
#	 * Este metodo calcula entonces las velocidades a enviarle al robot jugador para que alcance
#	 * una posicion final (notar que no se toma en cuenta en este path planning el angulo de llegada
#	 * del robot, es decir, el angulo con el cual querriamos que el robot alcance dicha posicion final).
#	 * Asi, se tienen en cuenta, para clacular dichas velocidades, la distancia al punto final y el
#	 * angulo diferencia entre los puntos inicial y final (ver metodo Math.atan2(...)).
#	 * <br><br>
#	 * Las velocidades son almacenadas finalmente en los miembros de clase vIzq y vDer.
#	 * 
#	 * @param x0 Coordenada X del punto inicial. 
#	 * @param y0 Coordenada Y del punto inicial.
#	 * @param r0 Rotacion inicial en grados en el intervalo [0,360].
#	 * @param xf Coordenada X del punto final.
#	 * @param yf Coordenada Y del punto final.
def setCmds (x0, y0, r0, xf, yf):
	#normalizo r0 a (-180, 180]
	r0 = norm(r0)

	#calcuo el angulo entre los puntos (x0, y0) y (xf, yf)
	ang = round(float(math.degrees(math.atan2(yf - y0, xf - x0))))
	
	#diferencia entre ang y r0 normalizada (-180, 180]
	dif = norm(ang - r0)

	#distancia entre los puntos (x0, y0) y (xf, yf)
	d = dist(x0, y0, xf, yf)

	if d > DIST:
		minimo = min(ANG_1, math.fabs(dif))
		setVels(dif, MAX_VEL, MAX_VEL - round((minimo * MAX_VEL) / ANG_1))
	else:
		#estoy cerca del punto final
		if math.fabs(dif) > ANG_2:
			#angulo de diferencia grande, debo corregir rapido mi orientacion hacia el punto final
			#para ello, reduzco las velocidades de las ruedas para girar mas cerrado
			setVels(dif, MAX_VEL/2, -1)
		else:
			#angulo de diferencia chico -> maxima velocidad para alcanzar rapidamente el punto final
			minimo = min(ANG_2, math.fabs(dif))
			setVels(dif, MAX_VEL, MAX_VEL - round((minimo * MAX_VEL) / ANG_2))


try:
	api = apiSumoUY.apiSumoUY()
	api.setPuertos(7001, "127.0.0.1", 8001)
	err = api.conectarse()	
except:
	error = True
	print "Error trying to connect..."	
	
			
if err == -1:
	error = True
	
while not error and misPuntos < 3 and susPuntos < 3:
	try:
		
		if api.getInformacion() == -1:
			error = True
		 

		# obtengo el estado de mi robot jugador
		x0 = api.getCoorX()
		y0 = api.getCoorY()
		r0 = api.getRot()
		
		# obtengo el estado del robot oponente
		xf = api.getCoorXOp()
		yf = api.getCoorYOp()
		
		# obtengo los puntos de cada uno
		misPuntos = api.getYukoP()
		susPuntos = api.getYukoPOp()

		if api.getEstado() == STOP:
			# si el juego esta detenido no hago nada
			api.enviarVelocidades(0,0)
		elif api.getEstado() == REPOSICIONAR:

			# debo posicionarme en (posX, posY, rot)
			posX = api.getCoorXR()
			posY = api.getCoorYR()
			rot  = api.getRotR()

			#calculo la distancia entre pos actual y pos destino
			d = dist(x0, y0, posX, posY)

			if (d < 40):
				#estoy cerca no hago nada
				#FIXME: falta tener en cuenta el angulo
				api.enviarVelocidades(0, 0)
			else: 
				#no estoy en la posicion deseada -> voy a ella
				setCmds(x0, y0, r0, posX, posY)
				
		else:
			#el juego esta en proceso -> voy a embestir al robot oponente
			setCmds(x0, y0, r0, xf, yf)
	except:
		error = True
		print "Error en el jugador"		


api.liberarRecursos()
