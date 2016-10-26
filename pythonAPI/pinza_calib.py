import butiaAPI
import time
#la pinza izq se mueve entre 1023(abierto) y 74(cerrado)
#la pinza der se mueve entre 0(abierto) y 879(cerrado)



PINZA_IZQ_CHASIS = "23"
PINZA_IZQ_PZA = "24"
PINZA_DER_CHASIS = "21"
PINZA_DER_PZA = "22"

OPEN_DER = 0
CLOSE_DER = 879
ALT_MAX_DER = 0
ALT_MIN_DER = 0
OPEN_IZQ = 980
CLOSE_IZQ = 74
ALT_MAX_IZQ = 0
ALT_MIN_IZQ = 0

butiabot = butiaAPI.robot()

def initMotor():
  butiabot.joint_mode(PINZA_IZQ_CHASIS, "0", "1023") #1023 is 300 degrees
  butiabot.joint_mode(PINZA_IZQ_PZA, "0", "1023") #1023 is 300 degrees
  butiabot.joint_mode(PINZA_DER_CHASIS, "0", "1023") #1023 is 300 degrees
  butiabot.joint_mode(PINZA_DER_PZA, "0", "1023") #1023 is 300 degrees

"""butiabot.nueva()

butiabot.set_speed(PINZA_IZQ, "0")
butiabot.set_speed(PINZA_DER, "0.2")"""

print "start..."
initMotor()
while 1==1:
  print "izq chasis= ", butiabot.get_position(PINZA_IZQ_CHASIS)
  print "izq pza= ",  butiabot.get_position(PINZA_IZQ_PZA)
  print "der chasis= ", butiabot.get_position(PINZA_DER_CHASIS)
  print "der pza= ",  butiabot.get_position(PINZA_DER_PZA)
  print("#################################\n")
  time.sleep(1)
  
  
butiabot.close()
