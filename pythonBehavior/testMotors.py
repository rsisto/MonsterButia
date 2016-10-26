import random, math, time
from config import configProperties
from behavior import behavior
from actuator import butiaAPI

butiaAPI = butiaAPI.robot()
BACKWARD_MOTOR = "1"
FORWARD_MOTOR = "0"
WHEEL_SPEED = 400
print("Turning ...")
butiaAPI.set2MotorSpeed(BACKWARD_MOTOR, str(WHEEL_SPEED), FORWARD_MOTOR, str(WHEEL_SPEED))
time.sleep(1)
butiaAPI.set2MotorSpeed(BACKWARD_MOTOR, "0", FORWARD_MOTOR, "0")
print("Stoping ... ")
