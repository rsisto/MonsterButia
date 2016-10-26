import behavior
import cv2.cv as cv, random, math, time, threading
from actuator import butiaAPI
from sensor import virtualSensor , camera , visionDetection
from config import configProperties

""" Este comportamiento evita de manera simple obstaculos presentes. Si se 
  detecta un obstaculo gira en un sentido fijo.
"""

class AvoidObstacles(behavior.Behavior, threading.Thread):
  config = configProperties.ConfigProperties()
  FORWARD_MOTOR = config.getPropertie("Motors","FORWARD_MOTOR")
  BACKWARD_MOTOR = config.getPropertie("Motors","BACKWARD_MOTOR")
  PX_IMAGE_WIDTH = int(config.getPropertie("Camera","PX_WIDTH"))
  PX_IMAGE_HEIGTH = int(config.getPropertie("Camera","PX_HEIGTH"))
  FPS = int(config.getPropertie("Camera","FPS"))

  # velocidad de las ruedas
  #MIN_WHEEL_SPEED = 0
  #MAX_WHEEL_SPEED = 600
  #MIN_SHAPE_AREA = 100
  MAX_WATER_SIZE = int(config.getPropertie("Behaviors","AVOID_MAX_WATER_SIZE"))
  TURN_SPEED = int(config.getPropertie("Behaviors","AVOID_TURN_SPEED"))
  MIN_DIST_LEFT = int(config.getPropertie("Distance","MIN_DIST_LEFT"))
  MIN_DIST_RIGHT = int(config.getPropertie("Distance","MIN_DIST_RIGHT"))
  PORT_RIGHT = config.getPropertie("Distance","PORT_RIGHT")
  PORT_LEFT = config.getPropertie("Distance","PORT_LEFT")

  #params de kinect:
  KINECT_DISTANCE_MIN = int(config.getPropertie("Vision","KINECT_DISTANCE_MIN"))
  KINECT_DISTANCE_MAX  = int(config.getPropertie("Vision","KINECT_DISTANCE_MAX"))
  KINECT_MAX_PIXEL_COUNT_FIRST_ROW = int(config.getPropertie("Vision","KINECT_MAX_PIXEL_COUNT_FIRST_ROW"))
  KINECT_FIRST_ROW_NUM = int(config.getPropertie("Vision","KINECT_FIRST_ROW_NUM"))
  
  #Freno para dejar coherente para el proximo comportamiento
  def reset(self):
    self.usb4butia.set2MotorSpeed(self.FORWARD_MOTOR, "0", self.FORWARD_MOTOR, "0")
  
  #obstaclePos es el kinect
  def __init__(self, motors, obstaclePos, cameraVS  , safeZoneMinHSV, safeZoneMaxHSV):
    behavior.Behavior.__init__(self)
    self.behaviorName = "AvoidObstacles"
    self.avoid = True
    #Rango HSV de zona segura (arena)
    self.safeZoneMinHSV = safeZoneMinHSV
    self.safeZoneMaxHSV = safeZoneMaxHSV
    self.usb4butia=motors
    self.obstaclePos = obstaclePos
    self.cameraVS = cameraVS
    self.avoid = True
    threading.Thread.__init__(self)
    self.start()

  # el thread para actualizar la variable avoid todo el tiempo (necesario para canActivate)
  def run(self): 
    while True:
      #Si no hay chances de ejecutar, salteo el procesamiento
      if(not self.shouldRun):
        time.sleep(0.5)
        continue
      time.sleep(1.0/self.FPS)
      
      waterSize = self.waterSize()
      obstaclePixels = self.obstacleInRange()
      
      #self.avoid = not (self.obstaclePos.getValue() is None) or (waterSize>self.MAX_WATER_SIZE) or (distLeft> self.MIN_DIST_LEFT) or (distRight> self.MIN_DIST_RIGHT)
      self.avoid = (obstaclePixels >= self.KINECT_MAX_PIXEL_COUNT_FIRST_ROW) or (waterSize>self.MAX_WATER_SIZE) 
      #print((waterSize>self.MAX_WATER_SIZE)  ,  (distRight> self.MIN_DIST_RIGHT) , distRight)
      
      
    
  
  def canActivate(self):
    return self.avoid

  def motorSchema(self):
    if self.avoid:
      self.usb4butia.set2MotorSpeed(self.FORWARD_MOTOR, str(int(self.TURN_SPEED)), self.BACKWARD_MOTOR, str(int(self.TURN_SPEED)))
    else:
      self.usb4butia.set2MotorSpeed(self.FORWARD_MOTOR, "0", self.FORWARD_MOTOR, "0")
    
  def finish(self):
    return self.avoid
    
  # devuelve la cantidad de pixeles de agua (o zona peligrosa) encontrados en la imagen
  def waterSize(self):
    return self.PX_IMAGE_WIDTH*self.PX_IMAGE_HEIGTH - visionDetection.countPixels(self.cameraVS.getValue().getImage(), self.safeZoneMinHSV[0],self.safeZoneMaxHSV[0],self.safeZoneMinHSV[1],self.safeZoneMaxHSV[1],self.safeZoneMinHSV[2],self.safeZoneMaxHSV[2])

  def obstacleInRange(self):
    return self.obstaclePos.getValue().countPixelsInRow(self.KINECT_DISTANCE_MIN, self.KINECT_DISTANCE_MAX,self.KINECT_MAX_PIXEL_COUNT_FIRST_ROW , self.KINECT_FIRST_ROW_NUM)
