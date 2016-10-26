import time
import wander, avoidObstacles, priorityBehaviors
from actuator import butiaAPI
from sensor import camera , virtualSensor

class WanderAndAvoid(priorityBehaviors.PriorityBehaviors):
  # estados de la maquina de comportamientos
  WANDER_STATE = "WANDER_STATE"
  AVOID_STATE = "AVOID_STATE"
  INITIAL_STATE = AVOID_STATE
  
  def __init__(self, motors, obstaclePos, cameraVS, safeZoneMinHSV, safeZoneMaxHSV):
    priorityBehaviors.PriorityBehaviors.__init__(self)
    self.behaviors[self.WANDER_STATE] = wander.Wander(motors)
    self.behaviors[self.WANDER_STATE].setPriority(1)
    self.behaviors[self.AVOID_STATE] = avoidObstacles.AvoidObstacles(motors, obstaclePos, cameraVS , safeZoneMinHSV, safeZoneMaxHSV)
    self.behaviors[self.AVOID_STATE].setPriority(0)
    self.reset()
    
