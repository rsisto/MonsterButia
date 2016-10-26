import behavior
import cv2.cv as cv, random, math, time
from actuator import butiaAPI
from sensor import virtualSensor
from config import configProperties


# Algritmo propuesto en el articulo Hasselmo2007 pp1265
class GoTo(behavior.Behavior):
  config = configProperties.ConfigProperties()
  FORWARD_MOTOR = config.getPropertie("Motors","FORWARD_MOTOR")
  BACKWARD_MOTOR = config.getPropertie("Motors","BACKWARD_MOTOR")

  # velocidad de las ruedas
  MIN_WHEEL_SPEED = -int(config.getPropertie("Behaviors","GOTO_MOTOR_SPEED"))
  MAX_WHEEL_SPEED = -MIN_WHEEL_SPEED
  
  
  def __init__(self, wheelMotors, cameraMotors, shape, minH, maxH, minV, maxV , nearObjectPixelThreshold):
    behavior.Behavior.__init__(self)
    self.minH = minH
    self.maxH = maxH
    self.minV = minV
    self.maxV = maxV
    self.wheelMotors = wheelMotors
    self.cameraMotors = cameraMotors
    self.shape = shape
    self.end = False
    self.MIN_SHAPE_AREA = nearObjectPixelThreshold
    

  def finish(self):
    return self.end
    
  def reset(self):
    self.end = False
    
  def motorSchema(self):
    shape = self.shape.getValue()
    #if (shape is None) or (cv.ContourArea(shape)<self.MIN_SHAPE_AREA) or (cv.ContourArea(shape)>self.MAX_SHAPE_AREA):
    if shape:
      area = cv.ContourArea(shape)
      #print("area",area)
      #print("minshapearea",self.MIN_SHAPE_AREA)
      
    self.end = (shape == 0) or (shape is None) or (area>self.MIN_SHAPE_AREA) 
    if self.end:
      self.wheelMotors.set2MotorSpeed(self.FORWARD_MOTOR, "0", self.FORWARD_MOTOR, "0")
      #print("goTo::stop")
    else:
      velL, velR = self.scaleDir2Vel(shape)
      if (velR>0): dirR=self.FORWARD_MOTOR
      else: dirR=self.BACKWARD_MOTOR
      if (velL>0): dirL=self.FORWARD_MOTOR
      else: dirL=self.BACKWARD_MOTOR
      
      self.wheelMotors.set2MotorSpeed(dirL, str(int(abs(velL))), dirR, str(abs(int(velR))))
  
  def scaleDir2Vel(self,shape):
    success, center , radius = cv.MinEnclosingCircle(shape)
    ceroUno = (self.cameraMotors.actualHPosition-self.minH)/float(self.maxH-self.minH)
    velR = self.MIN_WHEEL_SPEED+ceroUno*(self.MAX_WHEEL_SPEED-self.MIN_WHEEL_SPEED)
    velL = self.MIN_WHEEL_SPEED+(1-ceroUno)*(self.MAX_WHEEL_SPEED-self.MIN_WHEEL_SPEED)
    if (abs(velL)<300):
      velL = self.MAX_WHEEL_SPEED
      velR = self.MAX_WHEEL_SPEED
    return velL, velR


"""
centerCanVS = virtualSensor.VirtualSensor()

goTo = GoTo(centerCanVS, 0, 10, 0, 10)

for i in range(0,10):
  centerCanVS.setValue((i,0))
  goTo.behave()
  print("goTo", )
  
print("goTo:END")
"""
