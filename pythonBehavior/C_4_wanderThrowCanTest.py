"""
  Test de comportamiento conjunto prioritario de:
    Avoid (prioridad 0)
    FindThrowTrash (prioridad 0.5)
    Wander (prioridad 1)
    
  Comentando el comportamiento de avoid, funciona!s
"""

import time , ast
from actuator import butiaAPI , cameraMotors
from sensor import camera , virtualSensor , kinectCamera
from behavior import wanderAvoidGoToTrashCan
from config import configProperties

config = configProperties.ConfigProperties()

#configurando camara
camera = camera.Camera()
print("WanderAvoidGoToTrashCan::starting camera ... OK")
MIN_H = int(config.getPropertie("CameraMotors","MIN_H"))
MAX_H = int(config.getPropertie("CameraMotors","MAX_H"))
MIN_V = int(config.getPropertie("CameraMotors","MIN_V"))
MAX_V = int(config.getPropertie("CameraMotors","MAX_V"))
cameraMotors = cameraMotors.CameraMotors(MIN_H, MAX_H, MIN_V, MAX_V)

#configurando comportamientos
print("WanderAvoidGoToTrashCan::starting motors ... OK")
wheelMotors = butiaAPI.robot()
print("WanderAvoidGoToTrashCan::starting wheelMotors ... OK")
imageVS = virtualSensor.VirtualSensor(camera)
print("WanderAvoidGoToTrashCan::starting virtual camera sensor ... OK")

safeZoneMinHSV = ast.literal_eval(config.getPropertie("Vision","ARENA_HSV_MIN"))
safeZoneMaxHSV = ast.literal_eval(config.getPropertie("Vision","ARENA_HSV_MAX"))
canMinHSV = ast.literal_eval(config.getPropertie("Vision","TACHOBASURA_HSV_MIN"))
canMaxHSV = ast.literal_eval(config.getPropertie("Vision","TACHOBASURA_HSV_MAX"))
canMinSize = int(config.getPropertie("Vision","TACHO_MIN_SIZE"))
nearObjectPixelThreshold = int(config.getPropertie("Tacho", "NEAR_TACHO_SIZE"))
kinect = kinectCamera.KinectCamera()
kinectVS = virtualSensor.VirtualSensor(kinect)

wanderAvoidGoToTrashCan = wanderAvoidGoToTrashCan.WanderAvoidGoToTrashCan(wheelMotors, kinectVS, imageVS,cameraMotors,canMinHSV,canMaxHSV,safeZoneMinHSV,safeZoneMaxHSV , canMinSize , nearObjectPixelThreshold)

print("WanderAvoidGoToTrashCan::starting behavior ... OK")

while True:
  wanderAvoidGoToTrashCan.behave()
  print("WanderAvoidGoToTrashCan::behavior ", wanderAvoidGoToTrashCan.getName())
  time.sleep(0.5)


