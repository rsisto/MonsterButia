"""
  Test de comportamiento:
    FindTakeCan (Centra la lata, se acerca y la agarra)
    Problema: Cuando no ve la lata toma el control el movimiento de la pinza (solucion: usar comportamiento con wander)
"""

import time , ast
from actuator import butiaAPI , cameraMotors
from sensor import camera , virtualSensor
from behavior import findTakeCan
from config import configProperties

config = configProperties.ConfigProperties()

#configurando camara
camera = camera.Camera()
print("FindTakeCan::starting camera ... OK")
MIN_H = int(config.getPropertie("CameraMotors","MIN_H"))
MAX_H = int(config.getPropertie("CameraMotors","MAX_H"))
MIN_V = int(config.getPropertie("CameraMotors","MIN_V"))
MAX_V = int(config.getPropertie("CameraMotors","MAX_V"))
cameraMotors = cameraMotors.CameraMotors(MIN_H, MAX_H, MIN_V, MAX_V)

#configurando comportamientos
print("FindTakeCan::starting motors ... OK")
wheelMotors = butiaAPI.robot()
print("FindTakeCan::starting wheelMotors ... OK")
imageVS = virtualSensor.VirtualSensor(camera)
print("FindTakeCan::starting virtual camera sensor ... OK")

safeZoneMinHSV = ast.literal_eval(config.getPropertie("Vision","ARENA_HSV_MIN"))
safeZoneMaxHSV = ast.literal_eval(config.getPropertie("Vision","ARENA_HSV_MAX"))
canMinHSV = ast.literal_eval(config.getPropertie("Vision","LATA_HSV_MIN"))
canMaxHSV = ast.literal_eval(config.getPropertie("Vision","LATA_HSV_MAX"))
canMinSize = int(config.getPropertie("Vision","LATA_MIN_SIZE"))
nearObjectPixelThreshold = int(config.getPropertie("Latas", "NEAR_CAN_SIZE"))
collectedCansVS = virtualSensor.VirtualSensor(0)

behavior = findTakeCan.FindTakeCan(wheelMotors, imageVS,cameraMotors,canMinHSV,canMaxHSV,safeZoneMinHSV,safeZoneMaxHSV , canMinSize ,  nearObjectPixelThreshold , collectedCansVS)

print("FindTakeCan::starting behavior ... OK")

while True:
	behavior.behave()
	#time.sleep(1)
	print("FindTakeCan::behavior ", behavior.getName())
