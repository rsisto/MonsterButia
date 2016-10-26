"""
  Test de contar pixeles de kinect, por fila o matriz
    
"""

import time , ast
from sensor import kinectCamera
from config import configProperties

config = configProperties.ConfigProperties()

#configurando camara
camera = kinectCamera.KinectCamera()

KINECT_MIN_Y = int(config.getPropertie("Vision","KINECT_MIN_Y"))
KINECT_MAX_Y = int(config.getPropertie("Vision","KINECT_MAX_Y"))

KINECT_DISTANCE_MIN = int(config.getPropertie("Vision","KINECT_DISTANCE_MIN"))
KINECT_DISTANCE_MAX = int(config.getPropertie("Vision","KINECT_DISTANCE_MAX"))
KINECT_MAX_PIXEL_COUNT = int(config.getPropertie("Vision","KINECT_MAX_PIXEL_COUNT"))

KINECT_MAX_PIXEL_COUNT_FIRST_ROW = int(config.getPropertie("Vision","KINECT_MAX_PIXEL_COUNT_FIRST_ROW"))
KINECT_FIRST_ROW_NUM = int(config.getPropertie("Vision","KINECT_FIRST_ROW_NUM"))
while True:
  #count = camera.countPixelsInDistance(KINECT_MIN_Y,KINECT_MAX_Y,KINECT_DISTANCE_MIN,KINECT_DISTANCE_MAX,KINECT_MAX_PIXEL_COUNT)
  count = camera.countPixelsInRow(KINECT_DISTANCE_MIN,KINECT_DISTANCE_MAX,KINECT_MAX_PIXEL_COUNT_FIRST_ROW ,KINECT_FIRST_ROW_NUM )
  time.sleep(0.1)
  print("count:", count)
