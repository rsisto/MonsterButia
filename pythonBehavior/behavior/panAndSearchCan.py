import time
import behavior, pan, centerObject
from actuator import cameraMotors
from sensor import camera

class PanAndSearchCan(behavior.Behavior):
  # estados de la maquina de comportamientos
  PAN_STATE = "PAN_STATE"
  CENTER_STATE = "CENTER_STATE"
  
  
  def __init__(self, camera, cameraMotors):
    self.behavior.Behavior.__init__(self)
    self.behaviors = {}
    self.camera = camera
    self.cameraMotors = cameraMotors    
    self.behaviors[self.PAN_STATE] = pan.Pan(cameraMotors)
    self.behaviors[self.CENTER_STATE]  = centerCan.CenterCan(camera, cameraMotors)
    self.reset()
    
  def reset(self):
    for behavior in self.behaviors:
        self.behaviors[behavior].reset()
    self.state = self.CENTER_STATE

  def finish(self):
    return self.behaviors[self.PAN_STATE].finish()  

  def behave(self):
    self.doTransition()
    self.behaviors[self.state].behave()
    
  def doTransition(self):
    if self.state == self.PAN_STATE:
      image = self.camera.getImage()
      min_y , percentage = sandDetector.findSandHeightAndPercentage(image)
      shapes = lata_detector.find_shapes(image,min_y)
      center, shape = lata_detector.findMostLeft(shapes)
      
      if (self.cameraMotors.actualHPosition<self.cameraMotors.MAX_H) and (center[0] > 0): # si no llegue al borde izquierdo y hay una lata
        self.setState(self.CENTER_STATE)
        self.behaviors[self.state].reset()
    elif self.state == self.CENTER_STATE:
      if self.behaviors[self.state].finish():
        self.cameraMotors.doMove()
        self.setState(self.PAN_STATE)
        
  
  def setState(self, state):
    self.state = state
    
