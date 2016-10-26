"""
  Test picklata
    
"""

import time , ast
from actuator import butiaAPI , cameraMotors
from sensor import camera , virtualSensor
from behavior import throwCan
from config import configProperties

throwCan = throwCan.Throwcan()

print("CenterObject::starting behavior ... OK")

while True:
  throwCan.behave()
  time.sleep(5)
  print("correr")
