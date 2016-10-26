"""
  Test de comportamiento conjunto prioritario de:
    Avoid (prioridad 0)
    FindTakeCan (prioridad 0.5)
    Wander (prioridad 1)
    
  Comentando el comportamiento de avoid funciona ok!
"""

import time , ast
from actuator import butiaAPI , cameraMotors
from sensor import camera , virtualSensor
from behavior import wanderAvoidGoToCan
from config import configProperties

config = configProperties.ConfigProperties()

#configurando camara
camera = camera.Camera()
print("WanderAvoidGoToCan::starting camera ... OK")
MIN_H = int(config.getPropertie("CameraMotors","MIN_H"))
MAX_H = int(config.getPropertie("CameraMotors","MAX_H"))
MIN_V = int(config.getPropertie("CameraMotors","MIN_V"))
MAX_V = int(config.getPropertie("CameraMotors","MAX_V"))
cameraMotors = cameraMotors.CameraMotors(MIN_H, MAX_H, MIN_V, MAX_V)

#configurando comportamientos
print("WanderAvoidGoToCan::starting motors ... OK")
wheelMotors = butiaAPI.robot()
print("WanderAvoidGoToCan::starting wheelMotors ... OK")
imageVS = virtualSensor.VirtualSensor(camera)
print("WanderAvoidGoToCan::starting virtual camera sensor ... OK")



safeZoneMinHSV = ast.literal_eval(config.getPropertie("Vision","ARENA_HSV_MIN"))
safeZoneMaxHSV = ast.literal_eval(config.getPropertie("Vision","ARENA_HSV_MAX"))
canMinHSV = ast.literal_eval(config.getPropertie("Vision","LATA_HSV_MIN"))
canMaxHSV = ast.literal_eval(config.getPropertie("Vision","LATA_HSV_MAX"))
canMinSize = int(config.getPropertie("Vision","LATA_MIN_SIZE"))
nearObjectPixelThreshold = int(config.getPropertie("Latas", "NEAR_CAN_SIZE"))
collectedCansVS = virtualSensor.VirtualSensor(0)
wanderAvoidGoToCan = wanderAvoidGoToCan.WanderAvoidGoToCan(wheelMotors, virtualSensor.VirtualSensor(None), imageVS,cameraMotors,canMinHSV,canMaxHSV,safeZoneMinHSV,safeZoneMaxHSV , canMinSize , nearObjectPixelThreshold , collectedCansVS)

print("WanderAvoidGoToCan::starting behavior ... OK")

while True:
	wanderAvoidGoToCan.behave()
	#time.sleep(1)
	#print("behavior", wanderAvoidGoToCan.getName())
