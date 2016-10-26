#butiaRemoto
import butiaAPI
import time
import struct
import math

#def getXY(in_file_f):
#    x = 0		
#    y = 0
#    z = 0
#    event = in_file_f.read(16)
#    (time1,time2, type, code, value) = \
#    	struct.unpack(fmt,event)
#    time = time2 / 1000.0
#    if type == 2 or type == 3:
#	    if code == 0:
#		    x = value
#	    if code == 1:
#		    y = value
#	    if code == 2:
#		    z = value
#   if type == 0 and code == 0:
#	    sum = 0
#    return x,y

def getY(in_file_f):
	y = 0
	fmt = 'iihhi'	
	event = in_file_f.read(16)
	while not event:
		event = in_file_f.read(16)
	(time1,time2, type, code, value) = struct.unpack(fmt,event)
	time = time2 / 1000.0
	if type == 2 or type == 3:
		if code == 1:
			y = value
#			print("y: " + str(y))
	return y

def getX(in_file_f):
	x = 0
	fmt = 'iihhi'
	event = in_file_f.read(16)
	while not event:    
		event = in_file_f.read(16)
	(time1,time2, type, code, value) = struct.unpack(fmt,event)
	time = time2 / 1000.0
	if type == 2 or type == 3:
		if code == 0:
			x = value
#			print("x: " + str(x))
	return x

butiabot = butiaAPI.robot()
modulos = butiabot.listarModulos()
print modulos

#butiabot.abrirSensor()
butiabot.abrirMotores()
a = 'q' #no hace nada

time.sleep(1)
secondsensorfile = "/dev/input/event3"
#int, int, short, short, int
fmt = 'iihhi'
#open file in binary mode
in_file = open(secondsensorfile,"rb")
ANGULO_MAX = 500
MOTOR_MAX  = 1000

while True:
	x1 = getX(in_file)
	while x1 == 0:
		x1 = getX(in_file)
	y1 = getY(in_file)
	while y1 == 0:
		y1 = getY(in_file)
	print("x" + str(x1) + "y" + str(y1))
	#topeo a la mitad del recorrido
	if x1 > ANGULO_MAX:
		x1 = ANGULO_MAX
	elif x1 < -ANGULO_MAX:
		x1 = -ANGULO_MAX	
	if y1 > ANGULO_MAX:
		y1 = ANGULO_MAX
	elif y1 < -ANGULO_MAX:
		y1 = -ANGULO_MAX		

	xplusy = x1 + y1
	yminusx = y1 - x1

	#rueda izq la de id mayor
	sentidoIzq = 0 #forward
	if xplusy < 0:
		xplusy = abs(xplusy)
		sentidoIzq = 1

	#rueda der la de id menor
	sentidoDer = 0 #forward
	if yminusx < 0:
		yminusx = abs(yminusx)
		sentidoDer = 1
	#llevo el valor que lo limite al intervalo [-500, 500], al intervalo [-1000, 1000]  	
	xplusy = xplusy*2
	yminusx = yminusx*2

	#normalizo 
	if xplusy > MOTOR_MAX:
		xplusy = MOTOR_MAX
	if yminusx > MOTOR_MAX:
		yminusx = MOTOR_MAX
	butiabot.setVelocidadMotores(str(sentidoDer),str(yminusx), str(sentidoIzq), str(xplusy))			
print v		
in_file.close()		
print "fin"		
						
		
