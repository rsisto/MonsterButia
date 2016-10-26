""" Comportamiento compuesto por
  wanderAvoidAnGoToCan
  wanderAvoidGotoTrashcan
  
  Maquina de estados. Cuando se juntan N latas, se pasa al estado wanderAvoidGotoTrashcan hasta que finalice.

"""
import stateMachine, wanderAvoidGoToTrashCan, wanderAvoidGoToCan , wander
import time, threading
from actuator import cameraMotors, butiaAPI
from sensor import camera , virtualSensor


class FindAndThrowCan(stateMachine.StateMachine):
  PICKUP_CANS = "PICKUP_CANS"
  THROW_CANS = "THROW_CANS"
  INITIAL_STATE = PICKUP_CANS
  
  def getName(self):
    return "FindAndThrowCan::" + str(self.state) + "::" +  str(self.behaviors[self.state].getName()) 
  
  #Parametros:
  #canCollectionLimit cantidad de latas a juntar para empezar a tirarlas.
  def __init__(self, motors, obstaclePos, cameraVS, cameraMotors , canMinHSV,canMaxHSV,canMinSize , canNearSize,trashCanMinHSV,trashCanMaxHSV,trashCanMinSize , trashcanNearSize,safeZoneMinHSV,safeZoneMaxHSV  , collectedCansVS , canCollectionLimit):
    stateMachine.StateMachine.__init__(self)
    self.behaviors = {}
    self.camera = camera
    self.cameraMotors = cameraMotors
    self.canCollectionLimit = canCollectionLimit
    self.collectedCansVS = collectedCansVS
    self.behaviors[self.PICKUP_CANS]  = wanderAvoidGoToCan.WanderAvoidGoToCan(motors, obstaclePos, cameraVS, cameraMotors , canMinHSV,canMaxHSV,safeZoneMinHSV,safeZoneMaxHSV , canMinSize , canNearSize ,collectedCansVS)
    self.behaviors[self.THROW_CANS]  = wanderAvoidGoToTrashCan.WanderAvoidGoToTrashCan(motors, obstaclePos, cameraVS, cameraMotors , trashCanMinHSV,trashCanMaxHSV,safeZoneMinHSV,safeZoneMaxHSV , trashCanMinSize , trashcanNearSize)
    self.behaviors[self.THROW_CANS].setShouldRun(False)
    self.behaviors[self.PICKUP_CANS].setShouldRun(True)
    self.reset()
    
  def doTransition(self):
    #Cuando termina de juntar latas... 
    if (self.state == self.PICKUP_CANS and self.collectedCansVS.getValue() >= self.canCollectionLimit): #cuando se juntan N latas
      print("Transicion a TIRAR")
      
      #Seteo should run en false para pickupcans, para liberar un poco de procesador. Espero 1 segundo para permitir que el comportamiento actualice variables y estados
      self.behaviors[self.THROW_CANS].setShouldRun(True)
      self.behaviors[self.PICKUP_CANS].setShouldRun(False)
      time.sleep(1)
      
      self.collectedCansVS.setValue(0) #Reinicio contador de latas juntadas.
      self.setState(self.THROW_CANS)
    elif (self.state == self.THROW_CANS and  self.behaviors[self.THROW_CANS].finish()): #Cuando logra tirar las latas.
      print("Transicion a JUNTAR")
      
      #Seteo should run en false para throwcans, para liberar un poco de procesador. Espero 1 segundo para permitir que el comportamiento actualice variables y estados
      self.behaviors[self.THROW_CANS].setShouldRun(False)
      self.behaviors[self.PICKUP_CANS].setShouldRun(True)
      time.sleep(1)
      
      self.setState(self.PICKUP_CANS)
    

