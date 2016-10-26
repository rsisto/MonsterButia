""" Comportamiento que centra la camara en un objeto
  NOTA: Si se eject
"""
import behavior, time, cv2.cv as cv
import threading, ast
from actuator import  cameraMotors
from sensor import virtualSensor , camera, visionDetection
from config import configProperties

class CenterObject(behavior.Behavior, threading.Thread):
  # incrementos cuando se encontro un objeto y se trata de centrar en la imagen
  config = configProperties.ConfigProperties()
  INCREMENT_H_CENTER_OBJECT = int(config.getPropertie("CameraMotors","INCREMENT_H_CENTER_OBJECT"))
  INCREMENT_V_CENTER_OBJECT = int(config.getPropertie("CameraMotors","INCREMENT_V_CENTER_OBJECT"))
  FPS = int(config.getPropertie("Camera","FPS"))
  
  def __init__(self, camera, cameraMotors, sensorObjectVS , minHSVObject, maxHSVObject , minHSVFloor, maxHSVFloor , minObjectArea):
    behavior.Behavior.__init__(self)
    self.behaviorName = "CenterObject"
    self.lastShape = None
    self.camera = camera
    self.cameraMotors = cameraMotors
    self.sensorObjectVS = sensorObjectVS
    self.reset()
    self.minHSVObject = minHSVObject
    self.maxHSVObject = maxHSVObject
    self.minHSVFloor = minHSVFloor
    self.maxHSVFloor = maxHSVFloor
    self.minArea = minObjectArea
    threading.Thread.__init__(self)
    self.lastCaptureId = 0
    self.lastCaptureIdUsed = 0
    self.start()
    
  def reset(self):
    self.end = False;
    self.countMoveL=0
    self.countMoveR=0
    self.countMoveU=0
    self.countMoveD=0
    self.cameraMotors.centerMotors()


    
  def finish(self):
    return self.end
    
  # tiene sentido activarlo si veo una objeto
  def canActivate(self):
    return not (self.lastShape == None)
    
  # el thread para actualizar la variable seeCan todo el tiempo (necesario para canActivate)
  def run(self): 
    showCapture = ast.literal_eval(self.config.getPropertie("Vision","SHOW_CENTEROBJECT_DETECTION_IMAGE"))
    while True:
      #Si no hay chances de ejecutar, salteo el procesamiento
      if(not self.shouldRun):
        time.sleep(0.5)
        continue
      image = self.camera.getValue().getImage()
      min_y , percentage, sandShape = visionDetection.findBiggestShapeHeightAndPercentage(image,self.minHSVFloor,self.maxHSVFloor)
      #Si encuentro arena, sigo ejecutando
      if sandShape:
        #buscar poligono de arena y encontrar latas ahi adentro (poligono convexo, convex hull)
        storage = cv.CreateMemStorage(0)
        hull = cv.ConvexHull2 (sandShape, storage, cv.CV_CLOCKWISE, 1)
        shapes = visionDetection.find_shapes(image, min_y , self.minHSVObject[0],self.maxHSVObject[0],self.minHSVObject[1],self.maxHSVObject[1],self.minHSVObject[2],self.maxHSVObject[2] , minArea = self.minArea)
        
        #Buscar objeto mas cercano
        closestObject = None 
        y_object = 0
        center = (0,0)
        
        if shapes:
          for shape in shapes:
              success, center , radius = cv.MinEnclosingCircle(shape)
              for point in shape:
                if cv.PointPolygonTest(hull, point, False ) >= 0 and center[1]> y_object:
                  closestObject = shape
                  y_object = center[1]
                  break
              
        
        
        if(showCapture):
          #Dibujo safe zone en verde
          cv.PolyLine(image, [hull], True, (0,255,0), 3, cv.CV_AA, 0)
          if closestObject:
            #Dibujo closest object en rojo
            cv.PolyLine(image, [closestObject], True, (0,0,255), 3, cv.CV_AA, 0)        
          cv.ShowImage("CenterObjectDetection" + str(self), image)    
          cv.WaitKey(1000/self.FPS)  
        self.sensorObjectVS.setValue(closestObject)
        self.lastShape = closestObject #para el canActivate, se setea el objeto que esta viendo
        
      if (not showCapture):
        time.sleep(1.0/self.FPS)
      self.lastCaptureId = self.lastCaptureId + 1
      
    
  def motorSchema(self):
    #Para no ejecutar el comportamiento con la misma imagen que la vez anterior.
    if self.lastCaptureId == self.lastCaptureIdUsed:
      return
    self.lastCaptureIdUsed = self.lastCaptureId
    
    # determina la cantidad maxima de movimientos para ambos lados necesarios para asumir que estoy oscilando y detener la busqueda
    MAX_MOVES_TO_STOP = 3
    shape = self.sensorObjectVS.getValue()
    if shape<>None:
      success, center , radius = cv.MinEnclosingCircle(shape)

      # evaluo posicion de la marca y muevo la camara en la horizontal si corresponde
      difference = center[0]-self.camera.getValue().PX_IMAGE_WIDTH/2
      if (difference<0):
        self.countMoveL=self.countMoveL+1
        self.cameraMotors.setL(self.INCREMENT_H_CENTER_OBJECT)
      else:
        self.countMoveR=self.countMoveR+1
        self.cameraMotors.setR(self.INCREMENT_H_CENTER_OBJECT)
      
      # evaluo posicion de la marca y muevo la camara en la vertical si corresponde
      difference = center[1]-self.camera.getValue().PX_IMAGE_HEIGTH/2
      if (difference<0):
        self.countMoveU=self.countMoveU+1
        self.cameraMotors.setU(self.INCREMENT_V_CENTER_OBJECT)
      else:
        self.countMoveD=self.countMoveD+1
        self.cameraMotors.setD(self.INCREMENT_V_CENTER_OBJECT)
      # si estoy oscilando o la camara no se mueve termine de centrar
      if ((self.countMoveL>MAX_MOVES_TO_STOP) and (self.countMoveR>MAX_MOVES_TO_STOP)) or ((self.countMoveU>MAX_MOVES_TO_STOP) and (self.countMoveD>MAX_MOVES_TO_STOP)) or self.cameraMotors.getSameCommand():
        self.end = True
      else: 
        self.end = False
      self.cameraMotors.doMove()

  def getPercept(self):
    return self.center, cv.BoundingRect(self.lastShape)[0] # tomo del rectangulo que acota la figura el primer elemento (llx, left lower x)

