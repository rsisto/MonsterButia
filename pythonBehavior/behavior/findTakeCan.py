# -*- coding: utf-8 -*-
""" Comportamiento para dirigir robot a un objeto
Se centra la camara en el objeto y se mueven las ruedas del robot para dirigirse al mismo, finalmente junta la lata
"""
import stateMachine, go2Object, pinza_behavior_old
import time, threading
from actuator import cameraMotors, butiaAPI
from sensor import camera , virtualSensor


class FindTakeCan(stateMachine.StateMachine):
  GO_2_OBJECT_BEHAVIOR = "GO_2_OBJECT_BEHAVIOR"
  PINZA_BEHAVIOR = "PINZA_BEHAVIOR"
  INITIAL_STATE = GO_2_OBJECT_BEHAVIOR
  
  def getName(self):
    return "FindTakeCan::" + str(self.state) 
  
  #Params:
  #collectedCansVS: virtualsensor que lleva la cuenta de la cantidad de latas juntadas.  
  def __init__(self, wheelMotors, camera, cameraMotors, minHSVCan, maxHSVCan , minHSVFloor, maxHSVFloor, minCanSize , nearObjectPixelThreshold , collectedCansVS):
    stateMachine.StateMachine.__init__(self)
    self.behaviors = {}
    self.camera = camera
    self.cameraMotors = cameraMotors
    self.collectedCansVS = collectedCansVS
    self.behaviors[self.GO_2_OBJECT_BEHAVIOR]  = go2Object.Go2Object(wheelMotors,camera, cameraMotors,  minHSVCan, maxHSVCan , minHSVFloor, maxHSVFloor , minCanSize , nearObjectPixelThreshold)
    self.behaviors[self.PINZA_BEHAVIOR]  = pinza_behavior_old.PickLata()#(wheelMotors)
    self.reset()

  # tiene sentido activarlo si veo una lata
  def canActivate(self):
    return self.behaviors[self.GO_2_OBJECT_BEHAVIOR].canActivate()
      
  # Termino si levanto la lata
  def finish(self):
    return self.behaviors[self.PINZA_BEHAVIOR].finish()
    
  def doTransition(self):
    # si termine de acercarme a la lata debo recogerla
    #print("CENTER",self.behaviors[self.GO_2_OBJECT_BEHAVIOR].finish())
    #print("PINZA",self.behaviors[self.PINZA_BEHAVIOR].finish())
    if (self.behaviors[self.GO_2_OBJECT_BEHAVIOR].finish()): 
      self.setState(self.PINZA_BEHAVIOR)
    elif (self.behaviors[self.PINZA_BEHAVIOR].finish()): 
      self.setState(self.GO_2_OBJECT_BEHAVIOR)
      #Incremento contador de Latas juntadas (puede haber juntado menos por las veces que falló al juntar)
      self.collectedCansVS.setValue(self.collectedCansVS.getValue()+1)
    
    # TODO: puede quedarse indefinidamente si no logra tomar la lata
