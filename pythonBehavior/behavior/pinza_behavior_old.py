import behavior, time
from actuator import butiaAPI
from config import configProperties

#TODO: sacar para el properties.!!!!

PINZA_IZQ_CHASIS = "23"
PINZA_IZQ_PZA = "24"
PINZA_DER_CHASIS = "21"
PINZA_DER_PZA = "22"
OPEN_DER = 340
CLOSE_DER = 666
ALT_MAX_DER = 375 
#ALT_MIN_DER = 823 
ALT_MIN_DER = 755
OPEN_IZQ = 871
CLOSE_IZQ = 511
ALT_MAX_IZQ = 73
#Cuanto baja el brazo:
ALT_MIN_IZQ = 463 #Para superficie plana:
#ALT_MIN_IZQ = 513 #Playa
INC = 10
PAUSA = 0.1

class PickLata(behavior.Behavior):

  
  posDerChasis = ALT_MAX_DER
  posDerPinza = OPEN_DER
  posIzqChasis = ALT_MAX_IZQ
  posIzqPinza = OPEN_IZQ
  
  def __init__(self):
    behavior.Behavior.__init__(self)
    self.behaviorName = "PickLata"
    
    self.reset()
    self.butiabot = butiaAPI.robot()
    self.butiabot.joint_mode(PINZA_IZQ_CHASIS, "0", "1023") #1023 is 300 degrees
    self.butiabot.joint_mode(PINZA_IZQ_PZA, "0", "1023") #1023 is 300 degrees
    self.butiabot.joint_mode(PINZA_DER_CHASIS, "0", "1023") #1023 is 300 degrees
    self.butiabot.joint_mode(PINZA_DER_PZA, "0", "1023") #1023 is 300 degrees

    self.butiabot.set_position(PINZA_DER_CHASIS, str(ALT_MAX_DER))
    self.butiabot.set_position(PINZA_DER_PZA, str(OPEN_DER))
    self.butiabot.set_position(PINZA_IZQ_CHASIS, str(ALT_MAX_IZQ))
    self.butiabot.set_position(PINZA_IZQ_PZA, str(OPEN_IZQ))

    self.posDerChasis = ALT_MAX_DER
    self.posDerPinza = OPEN_DER
    self.posIzqChasis = ALT_MAX_IZQ
    self.posIzqPinza = OPEN_IZQ
    
    config = configProperties.ConfigProperties()

    self.FORWARD_MOTOR = config.getPropertie("Motors","FORWARD_MOTOR")
    self.BACKWARD_MOTOR = config.getPropertie("Motors","BACKWARD_MOTOR")
        
  def reset(self):
    self.end = False
    
  def canActivate(self):
    return True
  
  def finish(self):
    return self.end
    
  def motorSchema(self):  
    #danza de apareamiento
    self.butiabot.set2MotorSpeed(self.FORWARD_MOTOR, "1023", self.FORWARD_MOTOR, "1023")
    time.sleep(1.5) #antes 1.5
    
    self.butiabot.set2MotorSpeed(self.BACKWARD_MOTOR, "1023", self.BACKWARD_MOTOR, "1023")
    time.sleep(0.7)
    self.butiabot.set2MotorSpeed(self.BACKWARD_MOTOR, "0", self.BACKWARD_MOTOR, "0")
    time.sleep(0.5)
    
    #bajo pinza
    self.posDerChasis = ALT_MIN_DER
    self.posIzqChasis = ALT_MIN_IZQ
    self.butiabot.set_position(PINZA_DER_CHASIS, str(self.posDerChasis))
    self.butiabot.set_position(PINZA_IZQ_CHASIS, str(self.posIzqChasis))
    """while self.posDerChasis<ALT_MIN_DER and self.posIzqChasis<ALT_MIN_IZQ:  
      self.butiabot.set_position(PINZA_DER_CHASIS, str(self.posDerChasis))
      self.butiabot.set_position(PINZA_IZQ_CHASIS, str(self.posIzqChasis))
      if self.posDerChasis<ALT_MIN_DER:
        self.posDerChasis+=INC;
      if self.posIzqChasis<ALT_MIN_IZQ:
        self.posIzqChasis+=INC;"""
    time.sleep(PAUSA*2)
    #cierro
    while self.posDerPinza <CLOSE_DER and self.posIzqPinza > CLOSE_IZQ:  
      self.butiabot.set_position(PINZA_DER_PZA, str(self.posDerPinza))
      self.butiabot.set_position(PINZA_IZQ_PZA, str(self.posIzqPinza))
      if self.posDerPinza <CLOSE_DER:
        self.posDerPinza+=INC;
      if self.posIzqPinza > CLOSE_IZQ:
        self.posIzqPinza-=INC;
      time.sleep(PAUSA)
    #subo pinza  
    while self.posDerChasis>ALT_MAX_DER and self.posIzqChasis>ALT_MAX_IZQ:  
      ##TODO: hacer que suba rapido, volver un poco para adelante y recien ahi abrir las pinzas
      self.posDerChasis=ALT_MAX_DER;
      self.posIzqChasis=ALT_MAX_IZQ;
      self.butiabot.set_position(PINZA_DER_CHASIS, str(self.posDerChasis))    
      self.butiabot.set_position(PINZA_IZQ_CHASIS, str(self.posIzqChasis))
      """
      self.butiabot.set_position(PINZA_DER_CHASIS, str(self.posDerChasis))
      self.butiabot.set_position(PINZA_IZQ_CHASIS, str(self.posIzqChasis))
      if self.posDerChasis>ALT_MAX_DER:
        self.posDerChasis-=INC;
      if self.posIzqChasis>ALT_MAX_IZQ:
        self.posIzqChasis-=INC;
      time.sleep(PAUSA)
      """
    time.sleep(PAUSA)
    #abro
    while self.posDerPinza >OPEN_DER and self.posIzqPinza < OPEN_IZQ :  
      self.butiabot.set_position(PINZA_DER_PZA, str(self.posDerPinza))
      self.butiabot.set_position(PINZA_IZQ_PZA, str(self.posIzqPinza))
      if self.posDerPinza >OPEN_DER:
        self.posDerPinza-=INC
      if self.posIzqPinza < OPEN_IZQ:
         self.posIzqPinza+=INC
      time.sleep(PAUSA)
    self.end = True

  def getName(self):
    return self.behaviorName
