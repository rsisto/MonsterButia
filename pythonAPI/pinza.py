import butiaAPI
import time

PINZA_IZQ_CHASIS = "23"
PINZA_IZQ_PZA = "24"
PINZA_DER_CHASIS = "21"
PINZA_DER_PZA = "22"
OPEN_DER = 311
CLOSE_DER = 666
ALT_MAX_DER = 368
ALT_MIN_DER = 823
OPEN_IZQ = 980
CLOSE_IZQ = 74
ALT_MAX_IZQ = 0
ALT_MIN_IZQ = 0

butiabot = butiaAPI.robot()
#set grip motors in joint_mode
def initMotor():
	butiabot.joint_mode(PINZA_IZQ_CHASIS, "0", "1023") #1023 is 300 degrees
	butiabot.joint_mode(PINZA_IZQ_PZA, "0", "1023") #1023 is 300 degrees
	butiabot.joint_mode(PINZA_DER_CHASIS, "0", "1023") #1023 is 300 degrees
	butiabot.joint_mode(PINZA_DER_PZA, "0", "1023") #1023 is 300 degrees


"""butiabot.nueva()

butiabot.set_speed(PINZA_IZQ, "0")
butiabot.set_speed(PINZA_DER, "0.2")"""

print "start..."

#vel = raw_input("tiempo espera formato 0.05 > ")
#inc = int(raw_input("pase el incremento 10 > "))

posDerPza = OPEN_DER
posDerCha = ALT_MAX_DER
#posActDer = openDer
#posActIzq = openIzq

#inicio levantado y abierto
while True:
	initMotor()
	butiabot.set_position(PINZA_DER_CHASIS, str(ALT_MAX_DER))
	time.sleep(2)
	butiabot.set_position(PINZA_DER_PZA, str(OPEN_DER))
	time.sleep(2)
	butiabot.set_position(PINZA_DER_CHASIS, str(500))
	time.sleep(2)
	butiabot.set_position(PINZA_DER_CHASIS, str(ALT_MIN_DER))
	time.sleep(2)
	butiabot.set_position(PINZA_DER_PZA, str(CLOSE_DER))
	time.sleep(2)
	

"""
while posActDer < closeDer or  posActIzq > closeIzq:
	posDer += inc
	posIzq += -inc
	butiabot.set_position(PINZA_DER, str(posDer))
	butiabot.set_position(PINZA_IZQ, str(posIzq))

	time.sleep(float(vel))

	#print "dif= ", )
	if abs(posActDer-butiabot.get_position(PINZA_DER)) < 5:
		butiabot.set_position(PINZA_DER, str(posActDer-500))

	posActDer = butiabot.get_position(PINZA_DER)
	posActIzq = butiabot.get_position(PINZA_IZQ)

	#print "posActDer= ", posActDer

	
	#if abs(posActDer - butiabot.get_position(PINZA_DER))
	
	"""
		
"""	#open and close motion
	butiabot.set_position(PINZA_DER, str(openDer))
	butiabot.set_position(PINZA_IZQ, str(openIzq))
	butiabot.set_position(PINZA_DER, str(openDer))
	butiabot.set_position(PINZA_IZQ, str(openIzq))
	time.sleep(0.3)
	print "izq= ",butiabot.get_position(PINZA_IZQ)
	print "der= ", butiabot.get_position(PINZA_DER)
	#time.sleep(1)
	
	if openDer<= 0:
		inc = 100
	elif openDer> 850:
		inc = -100
		time.sleep(0.5)
	openDer+=inc
	openIzq-=inc
	time.sleep(0.3)"""
	  
butiabot.close()
