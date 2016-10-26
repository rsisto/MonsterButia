import behavior,  random, math, time
from actuator import butiaAPI
from config import configProperties

""" Este comportamiento cuenta unicamente de un motor esquema que implementa 
  una sencilla exploracion del entorno (wander). Para esto sortea un numero 
  entre MIN_TIME_OUT y MAX_TIME_OUT que determina la cantidad de segundos a 
  esperar antes de cambiar de estado.
  Se implementan tres estados, avanzar, girar izquierda y girar derecha. 
  No existen transiciones entre los girar, por lo que se obliga siempre a pasar
  por el estado avanzar.
"""

class Wander(behavior.Behavior):
  config = configProperties.ConfigProperties()
  FORWARD_MOTOR = config.getPropertie("Motors","FORWARD_MOTOR")
  BACKWARD_MOTOR = config.getPropertie("Motors","BACKWARD_MOTOR")

  # velocidad de las ruedas
  WHEEL_SPEED = int(config.getPropertie("Behaviors","WANDER_MOTOR_SPEED"))
  
  # estados de la maquina
  FORWARD_STATE = 0
  LEFT_STATE = 1
  RIGHT_STATE = 2
  
  # tiempos en segundos (real) para el time out
  
  MIN_TIME_OUT = 1
  MAX_TIME_OUT = 5
  
  
  def __init__(self, butiaAPI = butiaAPI.robot()):
    behavior.Behavior.__init__(self)
    self.behaviorName = "wander"
    self.currentWaitTime = 0
    lastTimeOut = time.time()
    self.wheelMotors = butiaAPI
    self.reset()

  def reset(self):
    self.state = self.FORWARD_STATE
    self.initTimeOut()
    
  def timeOut(self):
    if (time.time()-self.lastTimeOut)>self.currentWaitTime:
      self.initTimeOut()
      return True
      
    return False
      
  def initTimeOut(self):
    self.lastTimeOut = time.time()
    self.currentWaitTime = self.MIN_TIME_OUT + (self.MAX_TIME_OUT-self.MIN_TIME_OUT)*random.random()

  def motorSchema(self):
    if self.timeOut():
      if self.state == self.FORWARD_STATE:
        if random.random()>=0.5:
          self.state = self.LEFT_STATE
          self.wheelMotors.set2MotorSpeed(self.BACKWARD_MOTOR, str(self.WHEEL_SPEED), self.FORWARD_MOTOR, str(self.WHEEL_SPEED))
        else:
          self.state = self.RIGHT_STATE
          self.wheelMotors.set2MotorSpeed(self.FORWARD_MOTOR, str(self.WHEEL_SPEED), self.BACKWARD_MOTOR, str(self.WHEEL_SPEED))
      else:
          self.state = self.FORWARD_STATE
          self.wheelMotors.set2MotorSpeed(self.FORWARD_MOTOR, str(self.WHEEL_SPEED), self.FORWARD_MOTOR, str(self.WHEEL_SPEED))

