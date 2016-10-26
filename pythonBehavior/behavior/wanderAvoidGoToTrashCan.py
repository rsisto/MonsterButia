import time
import wander, avoidObstacles, priorityBehaviors , papelerRelease
from actuator import cameraMotors 
from sensor import camera , virtualSensor

"""
  Comportamiento compuesto por, en orden de prioridad descendente:
    Wander
    FindThrowTrash, compuesto por
      Acercarse a objeto
      tirar latas
    Avoid
    
"""

class WanderAvoidGoToTrashCan(priorityBehaviors.PriorityBehaviors):
  # estados de la maquina de comportamientos
  WANDER_STATE = "WANDER_STATE"
  AVOID_STATE = "AVOID_STATE"
  FIND_THROW_TRASH_STATE = "FIND_THROW_TRASH_STATE"
  INITIAL_STATE = WANDER_STATE
  
  def __init__(self, motors, obstaclePos, cameraVS, cameraMotors , trashCanMinHSV,trashCanMaxHSV,safeZoneMinHSV,safeZoneMaxHSV , objectMinSize , nearObjectPixelThreshold):
    priorityBehaviors.PriorityBehaviors.__init__(self)
    self.behaviors[self.WANDER_STATE] = wander.Wander(motors)
    self.behaviors[self.WANDER_STATE].setPriority(1)
    self.behaviors[self.FIND_THROW_TRASH_STATE] = papelerRelease.GoAndThrowTrash(motors, cameraVS, cameraMotors, trashCanMinHSV,trashCanMaxHSV,safeZoneMinHSV,safeZoneMaxHSV, objectMinSize , nearObjectPixelThreshold)
    self.behaviors[self.FIND_THROW_TRASH_STATE].setPriority(0.5)
    self.behaviors[self.AVOID_STATE] = avoidObstacles.AvoidObstacles(motors, obstaclePos, cameraVS , safeZoneMinHSV, safeZoneMaxHSV)
    self.behaviors[self.AVOID_STATE].setPriority(0)
    self.reset()
  
  def finish(self):
    return self.behaviors[self.FIND_THROW_TRASH_STATE].finish()
