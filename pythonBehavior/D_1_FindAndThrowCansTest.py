"""
  Test de comportamiento maquina de estados entre
    WanderAvoidGoToCan
    WanderAvoidGoToTrashCan
    
    Cuando se juntan N latas, se pasa al estado WanderAvoidGoToTrashCan
    Cuando WanderAvoidGoToTrashCan termina, se pasa a WanderAvoidGoToCan
    
"""
#TODO: mejorar comportamiento de avoid (probablemente bajando Y_MAX de camara.

import time , ast
from actuator import butiaAPI , cameraMotors
from sensor import camera , virtualSensor , kinectCamera
from behavior import findAndThrowCan
from config import configProperties

config = configProperties.ConfigProperties()

#configurando camara
camera = camera.Camera()
print("FindAndThrowCan::starting camera ... OK")
MIN_H = int(config.getPropertie("CameraMotors","MIN_H"))
MAX_H = int(config.getPropertie("CameraMotors","MAX_H"))
MIN_V = int(config.getPropertie("CameraMotors","MIN_V"))
MAX_V = int(config.getPropertie("CameraMotors","MAX_V"))
cameraMotors = cameraMotors.CameraMotors(MIN_H, MAX_H, MIN_V, MAX_V)

#configurando comportamientos
print("FindAndThrowCan::starting motors ... OK")
wheelMotors = butiaAPI.robot()
print("FindAndThrowCan::starting wheelMotors ... OK")
imageVS = virtualSensor.VirtualSensor(camera)
print("FindAndThrowCan::starting virtual camera sensor ... OK")

#parametros arena
safeZoneMinHSV = ast.literal_eval(config.getPropertie("Vision","ARENA_HSV_MIN"))
safeZoneMaxHSV = ast.literal_eval(config.getPropertie("Vision","ARENA_HSV_MAX"))
#Parametros tacho
trashCanMinHSV = ast.literal_eval(config.getPropertie("Vision","TACHOBASURA_HSV_MIN"))
trashCanMaxHSV = ast.literal_eval(config.getPropertie("Vision","TACHOBASURA_HSV_MAX"))
trashCanMinSize = int(config.getPropertie("Vision","TACHO_MIN_SIZE"))
trashCanNearSize = int(config.getPropertie("Tacho", "NEAR_TACHO_SIZE"))
#Parametros latas
canMinHSV = ast.literal_eval(config.getPropertie("Vision","LATA_HSV_MIN"))
canMaxHSV = ast.literal_eval(config.getPropertie("Vision","LATA_HSV_MAX"))
canMinSize = int(config.getPropertie("Vision","LATA_MIN_SIZE"))
canNearSize = int(config.getPropertie("Latas", "NEAR_CAN_SIZE"))

canCollectionLimit = int(config.getPropertie("Behaviors", "CAN_COLLECT_LIMIT"))

collectedCansVS = virtualSensor.VirtualSensor(0)

kinect = kinectCamera.KinectCamera()
kinectVS = virtualSensor.VirtualSensor(kinect)

findAndThrowCan = findAndThrowCan.FindAndThrowCan(wheelMotors, kinectVS, imageVS,cameraMotors,canMinHSV,canMaxHSV,canMinSize , canNearSize,trashCanMinHSV,trashCanMaxHSV,trashCanMinSize , trashCanNearSize , safeZoneMinHSV,safeZoneMaxHSV , collectedCansVS , canCollectionLimit)

print("FindAndThrowCan::starting behavior ... OK")

while True:
  findAndThrowCan.behave()
  #print("behavior", findAndThrowCan.getName())
  time.sleep(0.1)


