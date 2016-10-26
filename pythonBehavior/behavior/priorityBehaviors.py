import time
import behavior
from sensor import camera
from actuator import cameraMotors


class PriorityBehaviors(behavior.Behavior):
  
  
  def __init__(self):
    behavior.Behavior.__init__(self)
    self.behaviors = {}
    self.actualBehavior = None  
    
  def reset(self):
    for behavior in self.behaviors:
        self.behaviors[behavior].reset()

  def behave(self):
    self.setMinPriorityBehavior()
    if not (self.priorityBehavior is None):
      self.behaviors[self.priorityBehavior].behave()
  
  def setMinPriorityBehavior(self):
    minPriority = None
    self.priorityBehavior = None

    for behavior in self.behaviors:
      if (minPriority is None) or ((self.behaviors[behavior].getPriority()<minPriority) and self.behaviors[behavior].canActivate()):
        minPriority = self.behaviors[behavior].getPriority()
        self.priorityBehavior =  behavior        
    
  def getName(self):
    if (self.priorityBehavior is None):
      return "None"
    else:
      return self.behaviors[self.priorityBehavior].getName()
  
  def setShouldRun(self,var):
    self.shouldRun = var
    for behavior in self.behaviors:
        self.behaviors[behavior].setShouldRun(var)
