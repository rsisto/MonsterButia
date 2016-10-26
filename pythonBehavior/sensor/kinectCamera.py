import threading , time
from config import configProperties
from openni import *

"""
Clase utilitaria para manejar la camara kinect (se encarga de capturar imagenes)
Se utiliza el API PythonNI OpenNI
"""
class KinectCamera(threading.Thread):
  config = configProperties.ConfigProperties()
  FPS = int(config.getPropertie("Camera","KINECT_FPS"))
  INCREMENT_STEP = int(config.getPropertie("Vision","KINECT_INCREMENT_STEP"))
  depthMap = None
  def __init__(self):
    self.ctx = Context()
    self.ctx.init()

    # Create a depth generator
    self.depth = DepthGenerator()
    self.depth.create(self.ctx)
    
    #Set resolution and fps
    self.depth.set_resolution_preset(RES_VGA)
    self.depth.fps = self.FPS

    # Start generating images (?)
    self.ctx.start_generating_all()
    
    threading.Thread.__init__(self)
    self.start()
  
  def run(self): 
    while True:
      # Update to next frame
      nRetVal = self.ctx.wait_one_update_all(self.depth)
      # Store the depthmap
      self.depthMap = self.depth.map
      time.sleep(1.0/self.FPS)

  """
  returns the amount of pixels between a certain distance (in millimeters), in the area determined by maxy, miny
  if maxPixelCount is passed, the count finishes when maxPixelCount is reached
  """
  def countPixelsInDistance(self,minY, maxY , distanceMin,distanceMax, maxPixelCount):
    originalMaxPix = maxPixelCount
    if self.depthMap == None:
      return 0
    maxPixelCount = maxPixelCount / (self.INCREMENT_STEP*self.INCREMENT_STEP)
    width = self.depth.map.width
    pixelCount = 0
    #Solo buscar entre los Y pasados como parametros
    for j in range(minY,maxY , self.INCREMENT_STEP):
      for i in range(0, width, self.INCREMENT_STEP):
        depth = self.depthMap[i,j]
        if((i > minY) and (i < maxY) ):
          if((depth > distanceMin) and (depth < distanceMax)):
            pixelCount += 1
            if maxPixelCount <> None and pixelCount > maxPixelCount: 
              return originalMaxPix
    return pixelCount * (self.INCREMENT_STEP*self.INCREMENT_STEP)
  
  def countPixelsInRow(self,distanceMin,distanceMax, maxPixelCount,row):
    width = self.depth.map.width
    pixelCount = 0
    for i in range(0, width):
      depth = self.depthMap[i,row]
      if((depth > distanceMin) and (depth < distanceMax)):
        pixelCount += 1
        if maxPixelCount <> None and pixelCount > maxPixelCount: 
          return maxPixelCount 
    return pixelCount
