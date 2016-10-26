import behavior, time
from actuator import  butiaAPI

from config import configProperties


#algoritmo desarrollado por fede mientras escuchaba al cuarteto
class PickLata(behavior.Behavior):
  config = configProperties.ConfigProperties()
  PINZA_IZQ_CHASIS = config.getPropertie("Pinza","PINZA_IZQ_CHASIS")
  PINZA_IZQ_PZA = config.getPropertie("Pinza","PINZA_IZQ_PZA")
  PINZA_DER_CHASIS = config.getPropertie("Pinza","PINZA_DER_CHASIS")
  PINZA_DER_PZA = config.getPropertie("Pinza","PINZA_DER_PZA")
  OPEN_DER = int(config.getPropertie("Pinza","OPEN_DER"))
  CLOSE_DER = int(config.getPropertie("Pinza","CLOSE_DER"))
  ALT_MAX_DER =  int(config.getPropertie("Pinza","ALT_MAX_DER"))
  ALT_MIN_DER =  int(config.getPropertie("Pinza","ALT_MIN_DER"))
  OPEN_IZQ =  int(config.getPropertie("Pinza","OPEN_IZQ"))
  CLOSE_IZQ =  int(config.getPropertie("Pinza","CLOSE_IZQ"))
  ALT_MAX_IZQ =  int(config.getPropertie("Pinza","ALT_MAX_IZQ"))
  ALT_MIN_IZQ =  int(config.getPropertie("Pinza","ALT_MIN_IZQ"))
  INC =  int(config.getPropertie("Pinza","INC"))
  PAUSA = float(config.getPropertie("Pinza","PAUSA"))
  MOTOR_ENCODER_NOISE = int(config.getPropertie("Motors","MOTOR_ENCODER_NOISE"))

  posDerChasis = ALT_MAX_DER
  posDerPinza = OPEN_DER
  posIzqChasis = ALT_MAX_IZQ
  posIzqPinza = OPEN_IZQ
  
  def __init__(self, wheelMotors = butiaAPI.robot()):
    behavior.Behavior.__init__(self)
    self.behaviorName = "PickLata"
    self.butiabot = wheelMotors
    self.butiabot.joint_mode(self.PINZA_IZQ_CHASIS, "0", "1023") #1023 is 300 degrees
    self.butiabot.joint_mode(self.PINZA_IZQ_PZA, "0", "1023") #1023 is 300 degrees
    self.butiabot.joint_mode(self.PINZA_DER_CHASIS, "0", "1023") #1023 is 300 degrees
    self.butiabot.joint_mode(self.PINZA_DER_PZA, "0", "1023") #1023 is 300 degrees
    self.reset()
    
  def reset(self):
    print ("reset ... ")
    self.goToPositionPaused(self.ALT_MAX_DER, self.ALT_MAX_IZQ, self.OPEN_DER, self.OPEN_IZQ)

  def goToPositionPaused(self, reachDerChasis, reachIzqChasis, reachDerPinza, reachIzqPinza):
    posDerChasis=self.butiabot.get_position(self.PINZA_DER_CHASIS);
    posIzqChasis=self.butiabot.get_position(self.PINZA_IZQ_CHASIS)
    posDerPinza=self.butiabot.get_position(self.PINZA_DER_PZA)
    posIzqPinza=self.butiabot.get_position(self.PINZA_IZQ_PZA)
    lastDerChasis=-1
    lastIzqChasis=-1
    lastDerPinza=-1
    lastIzqPinza=-1
    
    # mientras cambien los valores de posicion dejo ejecutar, si no cambian posible error y vuelvo, tambien sale (break) si logra alcanzar la posicion solicitada
    while (posDerChasis==lastDerChasis) and (posIzqChasis==lastIzqChasis) and (posDerPinza==lastDerPinza) and (posIzqPinza==lastIzqPinza):
      differenceDerChasis = reachDerChasis-posDerChasis
      differenceIzqChasis = reachIzqChasis-posIzqChasis
      differenceDerPinza = reachDerPinza-posDerPinza
      differenceIzqPinza = reachIzqPinza-posIzqPinza
      
      if (abs(differenceDerChasis)<=self.INC) and (abs(differenceIzqChasis)<=self.INC) and (abs(differenceDerPinza)<=self.INC) and (abs(differenceIzqPinza)<=self.INC):
        break
      if differenceDerChasis<0:
        posDerChasis=posDerChasis-self.INC;
      else: 
        posDerChasis=posDerChasis+self.INC;  

      if differenceIzqChasis<0:
        posIzqChasis=posIzqChasis-self.INC;
      else: 
        posIzqChasis=posIzqChasis+self.INC;
        
      if differenceDerPinza<0:
        posDerPinza=posDerPinza-self.INC;
      else: 
        posDerPinza=posDerPinza+self.INC;
        
      if differenceIzqPinza<0:
        posIzqPinza=posIzqPinza-self.INC;
      else: 
        posIzqPinza=posIzqPinza+self.INC;
        
      self.butiabot.set_position(self.PINZA_DER_CHASIS, str(posDerChasis))
      self.butiabot.set_position(self.PINZA_IZQ_CHASIS, str(posIzqChasis))
      self.butiabot.set_position(self.PINZA_DER_PZA, str(posDerPinza))
      self.butiabot.set_position(self.PINZA_IZQ_PZA, str(posIzqPinza))
      time.sleep(self.PAUSA)
    
  def motorSchema(self):  
    #bajo pinza  
    print ("abajo .... ")
    self.goToPositionPaused(self.ALT_MIN_DER,self.ALT_MIN_IZQ, self.OPEN_DER, self.OPEN_IZQ)
    #cierro
    print ("cierro .... ")
    self.goToPositionPaused(self.ALT_MIN_DER,self.ALT_MIN_IZQ, self.CLOSE_DER, self.CLOSE_IZQ)
    #subo pinza  
    print ("subo .... ")
    self.goToPositionPaused(self.ALT_MAX_DER,self.ALT_MAX_IZQ, self.CLOSE_DER, self.CLOSE_IZQ)
    #abro
    print ("abro .... ")
    self.goToPositionPaused(self.ALT_MAX_DER,self.ALT_MAX_IZQ, self.OPEN_DER, self.OPEN_IZQ)
