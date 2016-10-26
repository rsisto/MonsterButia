import behavior, random, math, time
from actuator import butiaAPI
from config import configProperties

""" Este comportamiento implementa la descarga de latas, moviendo el balde tirando para adelante.
"""

class Throwcan(behavior.Behavior):
  config = configProperties.ConfigProperties()
  BIN_LEFT_ARM = config.getPropertie("Motors","BIN_LEFT_ARM")
  BIN_RIGHT_ARM = config.getPropertie("Motors","BIN_RIGHT_ARM")
  

  PINZA_IZQ_CHASIS = config.getPropertie("Pinza","PINZA_IZQ_CHASIS")
  PINZA_IZQ_PZA = config.getPropertie("Pinza","PINZA_IZQ_PZA")
  PINZA_DER_CHASIS = config.getPropertie("Pinza","PINZA_DER_CHASIS")
  PINZA_DER_PZA = config.getPropertie("Pinza","PINZA_DER_PZA")
  OPEN_DER = int(config.getPropertie("Pinza","OPEN_DER"))
  CLOSE_DER = int(config.getPropertie("Pinza","CLOSE_DER"))
  #Posicion a abrir brazos para tirar.
  OPEN_THROW_DER = int(config.getPropertie("Pinza","OPEN_THROW_DER"))
  OPEN_THROW_IZQ = int(config.getPropertie("Pinza","OPEN_THROW_IZQ"))
  
  ALT_MAX_DER =  int(config.getPropertie("Pinza","ALT_MAX_DER"))
  ALT_MED_DER =  int(config.getPropertie("Pinza","ALT_MED_DER"))
  ALT_MIN_DER =  int(config.getPropertie("Pinza","ALT_MIN_DER"))
  OPEN_IZQ =  int(config.getPropertie("Pinza","OPEN_IZQ"))
  CLOSE_IZQ =  int(config.getPropertie("Pinza","CLOSE_IZQ"))
  ALT_MAX_IZQ =  int(config.getPropertie("Pinza","ALT_MAX_IZQ"))
  ALT_MED_IZQ =  int(config.getPropertie("Pinza","ALT_MED_IZQ"))
  ALT_MIN_IZQ =  int(config.getPropertie("Pinza","ALT_MIN_IZQ"))
    
  POSITION_REST_LEFT_ARM = config.getPropertie("CanBinArm","POSITION_REST_LEFT_ARM")
  POSITION_REST_RIGHT_ARM = config.getPropertie("CanBinArm","POSITION_REST_RIGHT_ARM")
  POSITION_THROW_LEFT_ARM = config.getPropertie("CanBinArm","POSITION_THROW_LEFT_ARM")
  POSITION_THROW_RIGHT_ARM = config.getPropertie("CanBinArm","POSITION_THROW_RIGHT_ARM")
  POSITION_MEDIUM_LEFT_ARM = config.getPropertie("CanBinArm","POSITION_MEDIUM_LEFT_ARM")
  POSITION_MEDIUM_RIGHT_ARM = config.getPropertie("CanBinArm","POSITION_MEDIUM_RIGHT_ARM")
  FORWARD_MOTOR = config.getPropertie("Motors","FORWARD_MOTOR")
  BACKWARD_MOTOR = config.getPropertie("Motors","BACKWARD_MOTOR")
  
  waitForThrowSeconds = config.getPropertie("CanBinArm","waitForThrowSeconds")
  
  
  
  def __init__(self, butiaAPI = butiaAPI.robot()):
    behavior.Behavior.__init__(self)
    self.behaviorName = "throwCan"
    self.wheelMotors = butiaAPI    
    self.butiabot = butiaAPI
    self.butiabot.joint_mode(self.BIN_RIGHT_ARM, "0", "1023") #1023 is 300 degrees
    self.butiabot.joint_mode(self.BIN_LEFT_ARM, "0", "1023") #1023 is 300 degrees
    self.reset()
    
    
  def canActivate(self):
    return True
  
  def finish(self):
    return self.end
    
  def reset(self):
      #llevo los motores a la posicion inicial
    #abro y bajo
    self.butiabot.set_position(self.BIN_LEFT_ARM,str(self.POSITION_REST_LEFT_ARM))
    time.sleep(0.5) 
    self.butiabot.set_position(self.BIN_RIGHT_ARM,str(self.POSITION_REST_RIGHT_ARM))
    self.end = False
    
  def motorSchema(self):
    self.wheelMotors.set2MotorSpeed(self.FORWARD_MOTOR, "1023", self.FORWARD_MOTOR, "1023")
    time.sleep(1) #antes 1.5
    
    self.wheelMotors.set2MotorSpeed(self.FORWARD_MOTOR, "0", self.FORWARD_MOTOR, "0")
    
    #bajo la pinza a la mitad
    self.butiabot.set_position(self.PINZA_IZQ_CHASIS,str(self.ALT_MED_IZQ))
    self.butiabot.set_position(self.PINZA_DER_CHASIS,str(self.ALT_MED_DER))
    time.sleep(int(self.waitForThrowSeconds))
    
    #Abro pinzas
    self.butiabot.set_position(self.PINZA_IZQ_PZA,str(self.OPEN_THROW_IZQ))
    self.butiabot.set_position(self.PINZA_DER_PZA,str(self.OPEN_THROW_DER))
    time.sleep(1)
    
    #Bajo pinza
    self.butiabot.set_position(self.PINZA_IZQ_CHASIS,str(self.ALT_MIN_IZQ))
    self.butiabot.set_position(self.PINZA_DER_CHASIS,str(self.ALT_MIN_DER))
    time.sleep(1)
    
    #mover motor a posicion de tirar latas
    self.butiabot.set_position(self.BIN_LEFT_ARM,str(self.POSITION_THROW_LEFT_ARM))
    self.butiabot.set_position(self.BIN_RIGHT_ARM,str(self.POSITION_THROW_RIGHT_ARM))
    #dormir hasta que tire latas
    time.sleep(int(self.waitForThrowSeconds))
    self.butiabot.set_position(self.BIN_LEFT_ARM,str(self.POSITION_MEDIUM_LEFT_ARM))
    self.butiabot.set_position(self.BIN_RIGHT_ARM,str(self.POSITION_MEDIUM_RIGHT_ARM))
    #Volver a mover brazo de tacho a posicion inicial
    time.sleep(int(self.waitForThrowSeconds))
    self.butiabot.set_position(self.BIN_LEFT_ARM,str(self.POSITION_THROW_LEFT_ARM))
    self.butiabot.set_position(self.BIN_RIGHT_ARM,str(self.POSITION_THROW_RIGHT_ARM))
    time.sleep(int(self.waitForThrowSeconds))
    
    #sube pinza
    self.reset()
    time.sleep(1)
    
    #subir un poco la pinza izquierda
    self.butiabot.set_position(self.PINZA_IZQ_CHASIS,str(self.ALT_MED_IZQ))
    time.sleep(2)
    
    #subo un poco la pinza derecha
    self.butiabot.set_position(self.PINZA_DER_CHASIS,str(self.ALT_MED_DER))
    time.sleep(1)
      
    #acomodo pinzas 
    self.butiabot.set_position(self.PINZA_IZQ_PZA,str(self.OPEN_IZQ))
    self.butiabot.set_position(self.PINZA_DER_PZA,str(self.OPEN_DER))
    time.sleep(1)
    
    #subir pinza devuelta por las dudas
    self.butiabot.set_position(self.PINZA_DER_CHASIS,str(self.ALT_MAX_DER))
    self.butiabot.set_position(self.PINZA_IZQ_CHASIS,str(self.ALT_MAX_IZQ))
    
    #Ir para atras!
    self.wheelMotors.set2MotorSpeed(self.BACKWARD_MOTOR, "1023", self.BACKWARD_MOTOR, "1023")
    time.sleep(2) 
    self.wheelMotors.set2MotorSpeed(self.FORWARD_MOTOR, "1023", self.BACKWARD_MOTOR, "1023")
    time.sleep(2) 
    self.wheelMotors.set2MotorSpeed(self.FORWARD_MOTOR, "0", self.FORWARD_MOTOR, "0")
    self.end = True
    
"""
throwCan = ThrowCan()
print("ThrowCan::starting behavior ... OK")


wander.behave()
  #time.sleep(1)
  print(wander.state)
"""
    
