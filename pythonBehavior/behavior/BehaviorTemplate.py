import behavior
from actuator import  cameraMotors
from sensor import camera

class BehaviorName(behavior.Behavior):

  def __init__(self):
    behavior.Behavior.__init__(self)

  def reset(self):
    pass
            
  def getPriority(self):
    pass

  def setPriority(self, priority):
    pass
  
  def canActivate(self):
    pass
  
  def finish(self):
    pass  

  def behave(self):
    self.perceptualSchema()
    self.motorSchema()

  def perceptualSchema(self):
    pass
    
  def motorSchema(self):    
    pass
