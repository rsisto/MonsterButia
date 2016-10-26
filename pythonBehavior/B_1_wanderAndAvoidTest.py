"""
Script para testear comportamiento wanderAndAvoid
Cambia comportamiento entre:
  wander (cuando tiene suficiente arena delante) y
  avoid (cuando se detecta un obstaculo o hay pocos pixeles de arena)
"""

import time , ast
from actuator import butiaAPI
from sensor import camera , virtualSensor , kinectCamera
from behavior import wanderAndAvoid
from config import configProperties

config = configProperties.ConfigProperties()


camera = camera.Camera()
print("WanderAndAvoid::starting camera ... OK")
wheelMotors = butiaAPI.robot()
print("WanderAndAvoid::starting wheelMotors ... OK")
imageVS = virtualSensor.VirtualSensor(camera)
print("WanderAndAvoid::starting virtual camera sensor ... OK")


safeZoneMinHSV = ast.literal_eval(config.getPropertie("Vision","ARENA_HSV_MIN"))
safeZoneMaxHSV = ast.literal_eval(config.getPropertie("Vision","ARENA_HSV_MAX"))
kinect = kinectCamera.KinectCamera()
kinectVS = virtualSensor.VirtualSensor(kinect)
wanderAndAvoid = wanderAndAvoid.WanderAndAvoid(wheelMotors, kinectVS, imageVS,safeZoneMinHSV,safeZoneMaxHSV)
print("WanderAndAvoid::starting behavior ... OK")

while True:
	wanderAndAvoid.behave()
	time.sleep(1)
	print("WanderAndAvoid::waterSize ", wanderAndAvoid.getName())
