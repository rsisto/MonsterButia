"""
  Test de comportamiento centrar objeto con camara
    
"""

import time , ast
from actuator import butiaAPI , cameraMotors
from sensor import camera , virtualSensor
from behavior import centerObject
from config import configProperties

config = configProperties.ConfigProperties()

#configurando camara
camera = camera.Camera()
print("CenterObject::starting camera ... OK")
MIN_H = int(config.getPropertie("CameraMotors","MIN_H"))
MAX_H = int(config.getPropertie("CameraMotors","MAX_H"))
MIN_V = int(config.getPropertie("CameraMotors","MIN_V"))
MAX_V = int(config.getPropertie("CameraMotors","MAX_V"))
cameraMotors = cameraMotors.CameraMotors(MIN_H, MAX_H, MIN_V, MAX_V)

#configurando comportamientos
print("CenterObject::starting motors ... OK")
wheelMotors = butiaAPI.robot()
print("CenterObject::starting wheelMotors ... OK")
imageVS = virtualSensor.VirtualSensor(camera)
print("CenterObject::starting virtual camera sensor ... OK")



safeZoneMinHSV = ast.literal_eval(config.getPropertie("Vision","ARENA_HSV_MIN"))
safeZoneMaxHSV = ast.literal_eval(config.getPropertie("Vision","ARENA_HSV_MAX"))
canMinHSV = ast.literal_eval(config.getPropertie("Vision","LATA_HSV_MIN"))
canMaxHSV = ast.literal_eval(config.getPropertie("Vision","LATA_HSV_MAX"))
canMinSize = int(config.getPropertie("Vision","LATA_MIN_SIZE"))
nearObjectPixelThreshold = int(config.getPropertie("Latas", "NEAR_CAN_SIZE"))
centerObject = centerObject.CenterObject(imageVS,cameraMotors,virtualSensor.VirtualSensor(None),canMinHSV,canMaxHSV,safeZoneMinHSV,safeZoneMaxHSV , canMinSize )

print("CenterObject::starting behavior ... OK")

while True:
	centerObject.behave()
	time.sleep(0.1)
	print("behavior", centerObject.getName())
