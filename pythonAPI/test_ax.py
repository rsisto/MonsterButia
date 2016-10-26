import butiaAPI
import time

MOTOR_1 = "1"
MOTOR_2 = "2"
butiabot = butiaAPI.robot()
butiabot.joint_mode(MOTOR_1, "0", "1023") #1023 is 300 degrees
butiabot.joint_mode(MOTOR_2, "0", "1023") #1023 is 300 degrees

error = False
while not error:
    butiabot.set_position(MOTOR_1,"0")
    butiabot.set_position(MOTOR_2,"0")
    time.sleep(2)
    print "motors set to 0 position and the position of the motors is", butiabot.get_position(MOTOR_1), butiabot.get_position(MOTOR_2)
    butiabot.set_position(MOTOR_1,"300")
    butiabot.set_position(MOTOR_2,"300")
    time.sleep(2)
    print "motors set to 300 position and the position of the motors is", butiabot.get_position(MOTOR_1), butiabot.get_position(MOTOR_2)
    butiabot.set_position(MOTOR_1,"600")
    butiabot.set_position(MOTOR_2,"600")
    time.sleep(2)
    print "motors set to 600 position and the position of the motors is", butiabot.get_position(MOTOR_1), butiabot.get_position(MOTOR_2)
    butiabot.set_position(MOTOR_1,"900")
    butiabot.set_position(MOTOR_2,"900")
    time.sleep(2)
    print "motors set to 900 position and the position of the motors is", butiabot.get_position(MOTOR_1), butiabot.get_position(MOTOR_2)
    butiabot.set_position(MOTOR_1,"1020")
    butiabot.set_position(MOTOR_2,"1020")
    time.sleep(2)
    print "motors set to 1020 position and the position of the motors is", butiabot.get_position(MOTOR_1), butiabot.get_position(MOTOR_2)
butiabot.close()
