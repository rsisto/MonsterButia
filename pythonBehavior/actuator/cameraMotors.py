import butiaAPI
from config import configProperties
from sensor import camera

"""
Esta clase controla los motores de la torre donde va montada la camara.
Si la posicion horizontal se decrementa se mueve hacia la izquierda.
Si la posicion vertical se decrementa se mueve hacia arriba.
"""

class CameraMotors:
  config = configProperties.ConfigProperties()
  MOTOR_V = config.getPropertie("Motors","CAMERA_V")
  MOTOR_H = config.getPropertie("Motors","CAMERA_H")
  MOTOR_ENCODER_NOISE = int(config.getPropertie("Motors","MOTOR_ENCODER_NOISE"))
  
  # constantes de posicionamiento vertical y horizontal de la camara (si no se pasan como parametro)
  MIN_H = 300
  MAX_H = 700
  MIN_V = 150
  MAX_V = 400

  # campo de vision de la camara en unidade del encoder del motor.
  ENC_CAMERA_H_FOV = 135
  ENC_CAMERA_V_FOV = 101 # 135*120/160
  
  # almacena si se esta enviando la misma orden de control de posicion a los motores de la camara
  sameCommand = False
  # almacena la ultima orden de posicion a los motores
  lastPosH = -1
  lastPosV = -1
  
  # indica si se debe espera a que los motores alcancen la posicion (syncrono)
  motorMustReachPosition = False
  
  def __init__(self, initMinH = MIN_H, initMaxH = MAX_H, initMinV = MIN_V, initMaxV = MAX_V):
    self.MIN_H = initMinH
    self.MAX_H = initMaxH
    self.MIN_V = initMinV
    self.MAX_V = initMaxV
    self.butiabot = butiaAPI.robot()
    self.butiabot.joint_mode(self.MOTOR_H, "0", "1023") #1023 is 300 degrees
    self.butiabot.joint_mode(self.MOTOR_V, "0", "1023") #1023 is 300 degrees
    self.centerMotors()
    
  def centerMotors(self):
    self.actualVPosition = self.MIN_V+(self.MAX_V-self.MIN_V)/2
    self.actualHPosition = self.MIN_H+(self.MAX_H-self.MIN_H)/2
    self.butiabot.set_position(self.MOTOR_V,str(self.actualVPosition))
    self.butiabot.set_position(self.MOTOR_H,str(self.actualHPosition))
    self.waitToReachPosition()

    
  def setU(self, increment):
    self.actualVPosition=self.actualVPosition+increment
    if self.actualVPosition>self.MAX_V:
      self.actualVPosition=self.MAX_V
    
  def setD(self, increment):
    self.actualVPosition=self.actualVPosition-increment
    if self.actualVPosition<self.MIN_V:
      self.actualVPosition=self.MIN_V

  def setR(self, increment):
    self.actualHPosition=self.actualHPosition-increment
    if self.actualHPosition<self.MIN_H:
      self.actualHPosition=self.MIN_H

  def setL(self, increment):
    self.actualHPosition=self.actualHPosition+increment
    if self.actualHPosition>self.MAX_H:
      self.actualHPosition=self.MAX_H

  def gotoHome(self):
    self.actualHPosition=self.MIN_H
    self.actualVPosition=self.MIN_V

  def gotoHHome(self):
    self.actualHPosition=self.MIN_H
      
  def doMove(self):
    self.sameCommand = (self.actualHPosition==self.lastPosH) and (self.actualVPosition==self.lastPosV)
    self.butiabot.set_position(self.MOTOR_V,str(self.actualVPosition))
    self.butiabot.set_position(self.MOTOR_H,str(self.actualHPosition))
    self.lastPosV = self.actualVPosition
    self.lastPosH = self.actualHPosition
    self.waitToReachPosition()
  
  def waitToReachPosition(self):
    if (self.motorMustReachPosition):
      while True:
        differenceH = self.actualHPosition-self.butiabot.get_position(self.MOTOR_H)
        differenceV = self.actualVPosition-self.butiabot.get_position(self.MOTOR_V)
        if (abs(differenceH)<self.MOTOR_ENCODER_NOISE) and (abs(differenceH)<self.MOTOR_ENCODER_NOISE):
          break
          
  def reachUL(self):
    return (self.actualHPosition>=self.MAX_H) and (self.actualVPosition>=self.MAX_V)
  
  def setMotorMustReachPosition(self, reach):
    self.motorMustReachPosition = reach
    
  def getSameCommand(self):
    return self.sameCommand  

  def pixel2encoder(self, pixel):
    return self.ENC_CAMERA_H_FOV * pixel/camera.Camera.PX_IMAGE_WIDTH
    
