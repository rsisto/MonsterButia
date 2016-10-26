""" Comportamiento para dirigir robot a un objeto
Se centra la camara en el tacho y se mueven las ruedas del robot para dirigirse al mismo, finalmente tira la basura
"""
import stateMachine, go2Object, throwCan
import time, threading
from actuator import butiaAPI , cameraMotors
from sensor import virtualSensor, camera


class GoAndThrowTrash(stateMachine.StateMachine):
  GO_2_OBJECT_BEHAVIOR = "GO_2_OBJECT_BEHAVIOR"
  THROWCAN_BEHAVIOR = "THROWCAN_BEHAVIOR"
  INITIAL_STATE = GO_2_OBJECT_BEHAVIOR
  
  def getName(self):
    return "GoAndThrowTrash::" + str(self.state )
    
  def __init__(self, wheelMotors, camera, cameraMotors, minHSVCan, maxHSVCan , minHSVFloor, maxHSVFloor, minCanSize , nearObjectPixelThreshold):
    stateMachine.StateMachine.__init__(self)
    self.behaviors = {}
    self.camera = camera
    self.cameraMotors = cameraMotors
    self.behaviors[self.GO_2_OBJECT_BEHAVIOR]  = go2Object.Go2Object(wheelMotors,camera, cameraMotors,  minHSVCan, maxHSVCan , minHSVFloor, maxHSVFloor , minCanSize , nearObjectPixelThreshold)
    self.behaviors[self.THROWCAN_BEHAVIOR]  = throwCan.Throwcan(wheelMotors)
    print("haciendo reset")
    self.end = False
    self.reset()
  
  #Tiene sentido activarlo si se ve el tacho    
  def canActivate(self):
    return self.behaviors[self.GO_2_OBJECT_BEHAVIOR].canActivate()
    
  # Termino si tiro las latas
  def finish(self):
    return self.end
  
  def reset(self):
    stateMachine.StateMachine.reset(self)
    self.end = False
    
    
  def doTransition(self):
    # si termine de acercarme a la lata debo recogerla
    #print("CENTER",self.behaviors[self.GO_2_OBJECT_BEHAVIOR].finish())
    #print("THROWCAN",self.behaviors[self.THROWCAN_BEHAVIOR].finish())
    if (self.behaviors[self.GO_2_OBJECT_BEHAVIOR].finish()): 
      self.end = False
      self.setState(self.THROWCAN_BEHAVIOR)
    elif (self.behaviors[self.THROWCAN_BEHAVIOR].finish()): 
      self.end = True
      self.setState(self.GO_2_OBJECT_BEHAVIOR)


