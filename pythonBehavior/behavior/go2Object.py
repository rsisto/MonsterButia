""" Comportamiento para dirigir robot a un objeto
Se centra la camara en el objeto y se mueven las ruedas del robot para dirigirse al mismo.
"""
import concurrentsBehaviors, centerObject, goTo
import time, threading , cv2.cv as cv
from actuator import  cameraMotors  , butiaAPI
from sensor import camera , virtualSensor

class Go2Object(concurrentsBehaviors.ConcurrentsBehaviors):
  CENTER_OBJECT_BEHAVIOR = "CENTER_OBJECT_BEHAVIOR"
  GO_TO_BEHAVIOR = "GO_TO_BEHAVIOR"
  
  def getName(self):
    return "Go2Object"

  def __init__(self, wheelMotors, camera, cameraMotors, minHSVObject, maxHSVObject , minHSVFloor, maxHSVFloor, minObjectArea , nearObjectPixelThreshold):
    concurrentsBehaviors.ConcurrentsBehaviors.__init__(self)
    self.behaviors = {}
    self.camera = camera
    self.cameraMotors = cameraMotors
    self.centerObjectVS = virtualSensor.VirtualSensor(None)
    self.behaviors[self.CENTER_OBJECT_BEHAVIOR]  = centerObject.CenterObject(camera, cameraMotors, self.centerObjectVS , minHSVObject,maxHSVObject , minHSVFloor, maxHSVFloor, minObjectArea )
    self.behaviors[self.GO_TO_BEHAVIOR]  = goTo.GoTo(wheelMotors, cameraMotors, self.centerObjectVS, cameraMotors.MIN_H, cameraMotors.MAX_H, cameraMotors.MIN_V, cameraMotors.MAX_V , nearObjectPixelThreshold)
    self.reset()
    
  
  def canActivate(self):
    return self.behaviors[self.CENTER_OBJECT_BEHAVIOR].canActivate()

  # Termino si alcanzo el objeto
  def finish(self):
     return self.behaviors[self.GO_TO_BEHAVIOR].finish()

  # tiene sentido activarlo si veo un objeto
  def canActivate(self):
    return self.behaviors[self.CENTER_OBJECT_BEHAVIOR].canActivate()
  
  
