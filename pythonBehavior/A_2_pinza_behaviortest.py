"""
  Test picklata
    
"""

import time , ast
from actuator import butiaAPI , cameraMotors
from sensor import camera , virtualSensor
from behavior import pinza_behavior_old
from config import configProperties


pinza_behavior_old = pinza_behavior_old.PickLata()

print("CenterObject::starting behavior ... OK")

while True:
  pinza_behavior_old.behave()
  time.sleep(5)
  print("correr")
