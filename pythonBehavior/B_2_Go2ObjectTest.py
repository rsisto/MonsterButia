"""
  Test de comportamiento:
    Go2Object (Centra la lata, se acerca)
"""

import time , ast
from actuator import butiaAPI , cameraMotors
from sensor import camera , virtualSensor
from behavior import go2Object
from config import configProperties

config = configProperties.ConfigProperties()

#configurando camara
camera = camera.Camera()
print("Go2Object::starting camera ... OK")
MIN_H = int(config.getPropertie("CameraMotors","MIN_H"))
MAX_H = int(config.getPropertie("CameraMotors","MAX_H"))
MIN_V = int(config.getPropertie("CameraMotors","MIN_V"))
MAX_V = int(config.getPropertie("CameraMotors","MAX_V"))
cameraMotors = cameraMotors.CameraMotors(MIN_H, MAX_H, MIN_V, MAX_V)

#configurando comportamientos
print("Go2Object::starting motors ... OK")
wheelMotors = butiaAPI.robot()
print("Go2Object::starting wheelMotors ... OK")
imageVS = virtualSensor.VirtualSensor(camera)
print("Go2Object::starting virtual camera sensor ... OK")

safeZoneMinHSV = ast.literal_eval(config.getPropertie("Vision","ARENA_HSV_MIN"))
safeZoneMaxHSV = ast.literal_eval(config.getPropertie("Vision","ARENA_HSV_MAX"))
canMinHSV = ast.literal_eval(config.getPropertie("Vision","LATA_HSV_MIN"))
canMaxHSV = ast.literal_eval(config.getPropertie("Vision","LATA_HSV_MAX"))
canMinSize = int(config.getPropertie("Vision","LATA_MIN_SIZE"))
nearObjectPixelThreshold = int(config.getPropertie("Latas", "NEAR_CAN_SIZE"))
behavior = go2Object.Go2Object(wheelMotors, imageVS,cameraMotors,canMinHSV,canMaxHSV,safeZoneMinHSV,safeZoneMaxHSV , canMinSize , nearObjectPixelThreshold)

print("Go2Object::starting behavior ... OK")

while True:
	behavior.behave()
	time.sleep(0.1)
	print("Go2Object::behavior ", behavior.getName())
