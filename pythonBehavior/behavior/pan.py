import behavior
from actuator import  cameraMotors
from sensor import camera


class Pan(behavior.Behavior):
  
  def __init__(self, cameraMotors):
    behavior.Behavior.__init__(self)
    self.cameraMotors = cameraMotors
    self.reset()
    
  def reset(self):
    self.end = False;
    self.countMoveL=0
    self.countMoveR=0
    self.countMoveU=0
    self.countMoveD=0
    self.cameraMotors.gotoHome()
    self.cameraMotors.doMove()
    self.actualVPosition = self.cameraMotors.actualVPosition
    self.actualHPosition = self.cameraMotors.actualHPosition

  def behave(self):
    self.motorSchema()

  def perceptualSchema(self):
    pass
    
  def finish(self):
    return self.end;
    
  def motorSchema(self):
    # muevo motor de la camara hacia la izquierda y si no se mueve (alcanzo el borde), lo llevo a la derecha y subo un VFOV
    self.actualHPosition = self.actualHPosition + self.cameraMotors.ENC_CAMERA_H_FOV
    if (self.actualHPosition>self.cameraMotors.MAX_H):
      self.actualVPosition = self.actualVPosition + self.cameraMotors.ENC_CAMERA_V_FOV
      if (self.actualVPosition<=self.cameraMotors.MAX_V):
        self.actualHPosition = self.cameraMotors.MIN_H
      
    self.end = self.cameraMotors.reachUL()

    if not self.end:
      self.cameraMotors.actualHPosition = self.actualHPosition
      self.cameraMotors.actualVPosition = self.actualVPosition
      self.cameraMotors.doMove()
    
"""
cameraMotors = cameraMotors.CameraMotors(300, 700, 400, 600)
print("Pan::starting motors ... OK")
pan = Pan(cameraMotors)
print("Pan::starting behavior ... OK")
cameraMotors.setMotorMustReachPosition(True)

while not pan.finish():
  print(cameraMotors.actualHPosition, cameraMotors.actualVPosition)
  pan.behave()
print("Pan:END")
print(cameraMotors.actualHPosition, cameraMotors.actualVPosition)

"""
