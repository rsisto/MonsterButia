import time
import behavior, pan, centerObject
from actuator import cameraMotors
from sensor import camera


class ConcurrentsBehaviors(behavior.Behavior):
  
  
  def __init__(self):
    behavior.Behavior.__init__(self)
    self.behaviors = {}
  
  def reset(self):
    for behavior in self.behaviors:
        self.behaviors[behavior].reset()

  def behave(self):
    for behavior in self.behaviors:
      self.behaviors[behavior].behave()
        
  def finish(self):
    result = True

    for behavior in self.behaviors:
      if behavior.finish():
        result = False
        break
        
    return result

  def setShouldRun(self,var):
    self.shouldRun = var
    for behavior in self.behaviors:
        self.behaviors[behavior].setShouldRun(var)
