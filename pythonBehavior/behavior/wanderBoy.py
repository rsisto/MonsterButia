import behavior,  random, math, time
from actuator import butiaAPI

# Algritmo propuesto en el articulo Hasselmo2007 pp1265
class Wander(behavior.Behavior):
  M = 0.99 # momentum term
  S = 5.0  # average step size
  R = 0.5  #
  L = 35   # separacion entre ruedas
  
  
  def __init__(self):
    behavior.Behavior.__init__(self)
    self.wheelMotors = butiaAPI.robot()
    self.delta = (0,0)


  def reset(self):
    actualPoint = (0,0)

  def behave(self):
    self.motorSchema()

  def motorSchema(self):
    randomX = random.gauss(0, 1)
    randomY = random.gauss(0, 1)
    self.delta = (self.S * (1.0 - self.M) * randomX + self.M * self.delta[0], self.S * (1.0 - self.M) * randomY + self.M * self.delta[1])
    v = math.sqrt(math.pow(self.delta[0],2)+math.pow(self.delta[1],2))
    w = math.atan2(self.delta[1],self.delta[0])
    velL = v - w*self.L/2
    velR = v + w*self.L/2
    self.setVels(10*velL, 10*velR)

  def setVels(self, velL, velR):
    if velL>1023:
      velL = 1023
    
    if velL<0:
      dirL = 0
      velL = -velL
    else:
      dirL = 1

    if velR<0:
      dirR = 0
      velR = -velR
    else:
      dirR = 1
    velL = velL+200
    velR = velR+200

    if velL>1023:
      velL = 1023

    if velR>1023:
      velR = 1023

    self.wheelMotors.set2MotorSpeed(str(dirL), str(velL), str(dirR), str(velR))
    #print("Wander::setVels",str(dirL), str(velL), str(dirR), str(velR))

"""
wander = Wander()
while not wander.finish():
  #print(cameraMotors.actualHPosition, cameraMotors.actualVPosition)
  wander.behave()
  time.sleep(1)
  
print("Pan:END")
"""
