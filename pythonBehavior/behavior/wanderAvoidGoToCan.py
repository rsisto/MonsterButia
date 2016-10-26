import time
import wander, avoidObstacles, findTakeCan, priorityBehaviors
from actuator import cameraMotors 
from sensor import camera , virtualSensor

class WanderAvoidGoToCan(priorityBehaviors.PriorityBehaviors):
  # estados de la maquina de comportamientos
  WANDER_STATE = "WANDER_STATE"
  AVOID_STATE = "AVOID_STATE"
  FIND_TAKE_CAN_STATE = "FIND_TAKE_CAN_STATE"
  INITIAL_STATE = AVOID_STATE
  
  def __init__(self, motors, obstaclePos, cameraVS, cameraMotors , canMinHSV,canMaxHSV,safeZoneMinHSV,safeZoneMaxHSV , objectMinSize , nearObjectPixelThreshold, collectedCansVS):
    priorityBehaviors.PriorityBehaviors.__init__(self)
    self.behaviors[self.WANDER_STATE] = wander.Wander(motors)
    self.behaviors[self.WANDER_STATE].setPriority(1)
    self.behaviors[self.FIND_TAKE_CAN_STATE] = findTakeCan.FindTakeCan(motors, cameraVS, cameraMotors, canMinHSV,canMaxHSV,safeZoneMinHSV,safeZoneMaxHSV, objectMinSize , nearObjectPixelThreshold , collectedCansVS)
    self.behaviors[self.FIND_TAKE_CAN_STATE].setPriority(0.5)
    self.behaviors[self.AVOID_STATE] = avoidObstacles.AvoidObstacles(motors, obstaclePos, cameraVS , safeZoneMinHSV, safeZoneMaxHSV)
    self.behaviors[self.AVOID_STATE].setPriority(0)
    self.reset()
    
  
    
