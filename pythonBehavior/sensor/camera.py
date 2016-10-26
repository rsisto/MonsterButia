import threading, ast, cv2.cv as cv , time
from config import configProperties

class Camera(threading.Thread):
  config = configProperties.ConfigProperties()
  CAMERA_ID = int(config.getPropertie("Camera","cameraId"))
  FPS = int(config.getPropertie("Camera","FPS"))
  SHOW_CAMERA_IMAGE = config.getPropertie("Camera","SHOW_CAMERA_IMAGE")
  FRAME_NAME = "Capture"
  STATE_ON = 0
  STATE_OFF = 1
  state = STATE_ON
  
  PX_IMAGE_WIDTH = int(config.getPropertie("Camera","PX_WIDTH"))
  PX_IMAGE_HEIGTH = int(config.getPropertie("Camera","PX_HEIGTH"))
  
  def __init__(self, cameraId=CAMERA_ID,showImages = SHOW_CAMERA_IMAGE):
    self.CAMERA_ID = cameraId
    self.SHOW_CAMERA_IMAGE = showImages 
    self.capture = cv.CaptureFromCAM(cameraId)
    cv.SetCaptureProperty(self.capture, cv.CV_CAP_PROP_FRAME_HEIGHT, self.PX_IMAGE_HEIGTH)
    cv.SetCaptureProperty(self.capture, cv.CV_CAP_PROP_FRAME_WIDTH, self.PX_IMAGE_WIDTH)
    threading.Thread.__init__(self)
    self.start()
  
  def run(self): 
    while True:
      
      self.image = cv.QueryFrame(self.capture)
      if ast.literal_eval(self.SHOW_CAMERA_IMAGE):
        cv.ShowImage(self.FRAME_NAME, self.image)
        cv.WaitKey(1000/self.FPS)
      else:
        time.sleep(1.0/self.FPS)
      
  def getImage(self):
    #return self.image
    return cv.CloneImage(self.image)

