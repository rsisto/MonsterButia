#!/usr/bin/env python
#
# butiaRemotoJoystick.py
# Para el proyecto Butia, manejar el butia con joystick
#
# basado en codigo de:
# 	created 19 December 2007
# 	copyleft 2007 Brian D. Wendt
# 	http://principialabs.com/
#
# 	code adapted from:
#	http://svn.lee.org/swarm/trunk/mothernode/python/multijoy.py
#
# Copyleft Guillermo Reisch (2010)
#
# NOTE: This script requires the following Python modules:
#
#  pygame   - http://www.pygame.org/
# Win32 users may also need:
#  pywin32  - http://sourceforge.net/projects/pywin32/
#

import pygame
import math
import butiaAPI

# allow multiple joysticks
joy = []

XPOS = 0.0
YPOS = 0.0

#butia definitions
MOTOR_MAX  = 1000
butiabot = butiaAPI.robot()
butiabot.reconnect("localhost", 2009)
modulos = butiabot.listarModulos()
print modulos

#butiabot.abrirSensor()
butiabot.abrirMotores()


# Arduino USB port address (try "COM5" on Win32)
# usbport = "/dev/ttyUSB0"

# define usb serial connection to Arduino
# ser = serial.Serial(usbport, 9600)


# dada una velocidad setea los motores a la vel qued eberia
def handleSpeed():
	global XPOS
	global YPOS

	print "x: %f , y: %f)" % (XPOS, YPOS)
	# siempre entre -1.0 y 1.0 

	xplusy = XPOS + YPOS
	yminusx = YPOS - XPOS

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
	#llevo el valor que lo limite al intervalo [-1, 1], al intervalo [-1000, 1000]  	
	xplusy = xplusy*1000
	yminusx = yminusx*1000

	#normalizo 
	if xplusy > MOTOR_MAX:
		xplusy = MOTOR_MAX
	if yminusx > MOTOR_MAX:
		yminusx = MOTOR_MAX
	butiabot.setVelocidadMotores(str(sentidoDer),str(yminusx), str(sentidoIzq), str(xplusy))


# handle joystick event
def handleJoyEvent(e):
    global XPOS
    global YPOS

    if e.type == pygame.JOYAXISMOTION:
        axis = "unknown"
        if (e.dict['axis'] == 0):
            axis = "X"

        if (e.dict['axis'] == 1):
            axis = "Y"

        if (e.dict['axis'] == 2):
            axis = "Throttle"

        if (e.dict['axis'] == 3):
            axis = "Z"

        if (axis != "unknown"):
            str = "Axis: %s; Value: %f" % (axis, e.dict['value'])
            # uncomment to debug
            # output(str, e.dict['joy'])

            # Arduino joystick-servo hack
            if (axis == "X"):
                XPOS = e.dict['value']
		handleSpeed()
                # convert joystick position to servo increment, 0-180
                #move = round(pos * 90, 0)
                #if (move < 0):
                #    servo = int(90 - abs(move))
                #else:
                #    servo = int(move + 90)
                # convert position to ASCII character
                #servoPosition = chr(servo)
                # and send to Arduino over serial connection
                #    ser.write(servoPosition)
                # uncomment to debug
                #print servo, servoPosition

            if (axis == "Y"):
                YPOS = e.dict['value']
		handleSpeed()


    elif e.type == pygame.JOYBUTTONDOWN:
        str = "Button: %d" % (e.dict['button'])
        # uncomment to debug
        #output(str, e.dict['joy'])
        # Button 0 (trigger) to quit
        if (e.dict['button'] == 0):
            print "Bye!\n"
            #ser.close()
            quit()
    else:
        pass

# print the joystick position
def output(line, stick):
    print "Joystick: %d; %s" % (stick, line)

# wait for joystick input
def joystickControl():
    global XPOS
    global YPOS

    while True:
        e = pygame.event.wait()
        if (e.type == pygame.JOYAXISMOTION or e.type == pygame.JOYBUTTONDOWN):
            handleJoyEvent(e)

# main method
def main():

    # initialize pygame
    pygame.joystick.init()
    pygame.display.init()
    if not pygame.joystick.get_count():
        print "\nPlease connect a joystick and run again.\n"
        quit()
    print "\n%d joystick(s) detected." % pygame.joystick.get_count()
    for i in range(pygame.joystick.get_count()):
        myjoy = pygame.joystick.Joystick(i)
        myjoy.init()
        joy.append(myjoy)
        print "Joystick %d: " % (i) + joy[i].get_name()
    print "Depress trigger (button 0) to quit.\n"

    # run joystick listener loop
    joystickControl()

# allow use as a module or standalone script
if __name__ == "__main__":
    main()
