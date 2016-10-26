import time
import behavior, wander, go2Can
from actuator import cameraMotors , butiaAPI
from sensor import camera

class WanderAndGo2Can(behavior.Behavior):
  # estados de la maquina de comportamientos
  WANDER_STATE = "WANDER_STATE"
  GO_2_CAN_STATE = "GO_2_CAN_STATE"
  
  
  def __init__(self, camera, cameraMotors, butiaAPI):
    behavior.Behavior.__init__(self)
    self.behaviors = {}
    self.camera = camera
    self.cameraMotors = cameraMotors    
    self.behaviors[self.WANDER_STATE] = wander.Wander(butiaAPI)
    self.behaviors[self.GO_2_CAN_STATE]  = go2Can.Go2Can(camera, cameraMotors)
    self.reset()
    
  def reset(self):
    for behavior in self.behaviors:
        self.behaviors[behavior].reset()
    self.state = self.WANDER_STATE

  def behave(self):
    self.doTransition()
    self.behaviors[self.state].behave()
    
  def doTransition(self):
    image = self.camera.getImage()
    min_y , percentage = sandDetector.findSandHeightAndPercentage(image)
    shapes = lata_detector.find_shapes(image,min_y)
    center, shape = lata_detector.findMostLeft(shapes  )

    if self.state == self.WANDER_STATE:
      if center[0]>0:
        self.setState(self.GO_2_CAN_STATE)
        self.behaviors[self.state].reset()
    elif self.state == self.GO_2_CAN_STATE:
      if center[0]<=0:
        self.setState(self.WANDER_STATE)
        self.behaviors[self.state].reset()
        
  def setState(self, state):
    self.state = state
    
"""
motors = butiaAPI.robot()
camera = camera.Camera()

print("WanderAndGo2Can::starting camera ... OK")
cameraMotors = cameraMotors.CameraMotors(300, 700, 100, 500)
cameraMotors.setMotorMustReachPosition(True)

print("WanderAndGo2Can::starting motors ... OK")
wanderAndGo2Can = WanderAndGo2Can(camera, cameraMotors, motors)
print("WanderAndGo2Can::starting behavior ... OK")

while  not wanderAndGo2Can.finish():
  print("WanderAndGo2Can::state: " + wanderAndGo2Can.state)
  wanderAndGo2Can.behave()
  time.sleep(0.5)
  
print(wanderAndGo2Can.finish())

"""
