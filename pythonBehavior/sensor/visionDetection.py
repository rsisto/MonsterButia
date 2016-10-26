# -*- coding: utf-8 -*-
"""
Utilitario para detectar
"""


import math, camera, time , ast
from config import configProperties
import cv2.cv as cv

config = configProperties.ConfigProperties()



'''
  Retorna y_min , percentage
    y_min: coordenada y mínima donde se encuentra la arena 
           (recordar que los pixeles de la imagen se cuentan de arriba hacia abajo)
    percentage: porcentaje de arena en la imagen
  Recordar: opencv maneja rango de hue de 0 a 180 grados
    por ejemplo, para buscar rojos pasar hueMin=-10, hueMax=10
  
  Parámetros:
    img_hsv: imagen de 3 canales hsv
    
  
'''
def countPixels(img, minHue = 0 , maxHue = 180, minSaturation = 0 , maxSaturation = 255, minValue = 0 , maxValue = 255):
  #cv.Smooth(img, img, cv.CV_BLUR, 3); 

  img_hsv = cv.CreateImage( cv.GetSize(img), 8, 3 )
  cv.CvtColor(img,img_hsv,cv.CV_BGR2HSV)

  thresholded_img = thresholdImage(img_hsv, minHue, maxHue, minSaturation, maxSaturation, minValue , maxValue)
  pixel_count = cv.CountNonZero(thresholded_img)

  #Porcentaje
  showImage = False
  if (showImage):
    cv.ShowImage("countPixels", thresholded_img)
    cv.WaitKey(50)

  return pixel_count


def findBiggestShapeHeightAndPercentage(img, minHSV, maxHSV , areaMin = 1000):
  cv.Smooth(img, img, cv.CV_BLUR, 3); 

  img_hsv = cv.CreateImage( cv.GetSize(img), 8, 3 )
  cv.CvtColor(img,img_hsv,cv.CV_BGR2HSV)

  # filtrar por rango:
  thresholded_img = thresholdImage(img_hsv,minHSV[0],maxHSV[0],minHSV[1],maxHSV[1],minHSV[2],maxHSV[2])
  
  #Porcentaje
  sand_count = cv.CountNonZero(thresholded_img)
  percentage = sand_count*100/(img_hsv.height*img_hsv.width)
  showCapture = ast.literal_eval(config.getPropertie("Vision","SHOW_FINDBIGGESTSHAPE_IMAGE"))
  if(showCapture):
    cv.ShowImage("findBiggestShapeHeightAndPercentage", thresholded_img)
  #Encontrar contorno
  contour = find_biggest_shape(thresholded_img,areaMin)
  #Buscar y minimo
  min_y = img_hsv.height
  if contour != None:
    for point in contour:
      if point[1] < min_y:
        min_y = point[1]
  return min_y , percentage , contour
  
#Retorna una imagen binaria con los pixeles que cumplen la condicion de los parametros
def thresholdImage(img_hsv, minHue = 0 , maxHue = 180, minSaturation = 0 , maxSaturation = 255, minValue = 0 , maxValue = 255):
  # filtrar por rango:
  thresholded_img =  cv.CreateImage(cv.GetSize(img_hsv), 8, 1) 
  
  # HSV stands for hue, saturation, and value
  cv.InRangeS(img_hsv, (minHue, minSaturation, minValue), (maxHue, maxSaturation, maxValue), thresholded_img) 
  
  if(minHue < 0 ):
    # Si minHue es menor a cero, se calcula 2 veces, uno para valores de hue entre [minHue,maxHue] y otro entre [180+minHue,180]
    thresholded_img2 =  cv.CreateImage(cv.GetSize(img_hsv), 8, 1) 

    cv.InRangeS(img_hsv, (180 + minHue, minSaturation, minValue), (180, maxSaturation, maxValue), thresholded_img2) 
    cv.Add(thresholded_img,thresholded_img2,thresholded_img) # para imprimir
    
  return thresholded_img
  
def find_biggest_shape( gray , minArea ):
    """
    use contour search to find shapes in binary image
    """
    storage = cv.CreateMemStorage(0)
    contours = cv.FindContours(gray, storage, cv.CV_RETR_TREE, cv.CV_CHAIN_APPROX_SIMPLE, (0,0))  
    storage = cv.CreateMemStorage(0)
    #Temporales para guardar contorno más grande
    max_area = 0
    max_contour = None
    while contours:
        arclength = cv.ArcLength(contours)
        approximationAccuracy = arclength * 0.02
        polygon = cv.ApproxPoly( contours, storage, cv.CV_POLY_APPROX_DP,  approximationAccuracy, 0) 
        area = math.fabs( cv.ContourArea(polygon) )
        if area > minArea and area > max_area: 
          max_contour = polygon[::]
          max_area = area
        contours = contours.h_next()
    return max_contour 

# Encuentra formas a partir de una imagen binaria
def find_shapes_from_binary( gray , minArea , approximationAccuracyFactor , min_y):
    """
    use contour search to find squares in binary image
    returns list of numpy arrays containing 4 points
    """
    squares = []
    storage = cv.CreateMemStorage(0)
    contours = cv.FindContours(gray, storage, cv.CV_RETR_TREE, cv.CV_CHAIN_APPROX_SIMPLE, (0,0))  
    storage = cv.CreateMemStorage(0)
    while contours:
        arclength = cv.ArcLength(contours)
        approximationAccuracy = arclength * approximationAccuracyFactor
        polygon = cv.ApproxPoly( contours, storage, cv.CV_POLY_APPROX_DP,  approximationAccuracy, 0) #TODO: que es arclength
        area = math.fabs( cv.ContourArea(polygon) )
        isBelowY = False
        for point in polygon:
          if point[1] > min_y:
            isBelowY = True
            break
        if area > minArea and isBelowY:
          squares.append(polygon[::])
        contours = contours.h_next()
        

    return squares

"""
 Encuentra poligonos con luminosidad menor a "valueThreshold" y area mayor a minArea
 color_img, imagen en BGR
"""
def find_shapes(color_img , min_y = 0, hueMin=0,hueMax=180,satMin=0,satMax=255,valMin=0,valMax=255,minArea = 50, approximationAccuracyFactor = 0.02):
    #select even sizes only
    width, height = (color_img.width & -2, color_img.height & -2 )
    timg = cv.CloneImage( color_img ) # make a copy of input image
    # select the maximum ROI in the image
    cv.SetImageROI( timg, (0, 0, width, height) )

    # down-scale and upscale the image to filter out the noise
    pyr = cv.CreateImage( (width/2, height/2), 8, 3 )
    cv.PyrDown( timg, pyr, 7 )
    cv.PyrUp( pyr, timg, 7 )

    #Convert image to HSV 
    cv.CvtColor(timg, timg, cv.CV_BGR2HSV)
    #Blur the image :: Gonzalo
    cv.Smooth(timg, timg, cv.CV_GAUSSIAN, 15, 15)
    
    # select the maximum ROI in the image (no se hace porque los rectangulos los devuelve con posicion relativa al nuevo rectangulo.)
    #cv.SetImageROI( timg, (0, min_y, width, height) )
    
    gray = thresholdImage(timg,hueMin,hueMax,satMin,satMax,valMin,valMax)
    
    showCapture = ast.literal_eval(config.getPropertie("Vision","SHOW_SHAPE_DETECTION_IMAGE"))
    if(showCapture):
      cv.ShowImage("find_shapes_thresh", gray)    
      
    squares = []
    squares = find_shapes_from_binary(gray , minArea, approximationAccuracyFactor , min_y)

    
    if(showCapture):
      color, othercolor = (0,0,255), (0,255,0)
      printImg = cv.CloneImage(color_img)
      for square in squares:
        success, center , radius = cv.MinEnclosingCircle(square)
        cv.PolyLine(printImg, [square], True, color, 3, cv.CV_AA, 0)
        cv.Circle(printImg, (int(center[0]),int(center[1])), 10, othercolor)
        color, othercolor = othercolor, color
      cv.ShowImage("find_shapes_contours", printImg) 
    return squares

    
    
"""
Dado un conjunto de poligomnos encuentra primero un circulo que lo inscribe para cada uno de ellos,
y luego devuelve el radio y el centro del mayor.
"""    	
def findMaxCanCircle(polygons):
	maxRadius = 0
	maxCenter = [0,0]

	for polygon in polygons:
		success, center , radius = cv.MinEnclosingCircle(polygon)
		if success and (radius>maxRadius):
			maxRadius=radius
			maxCenter=center
	return maxRadius, maxCenter

"""
Dado un conjunto de poligomnos calcula el area y el centrol circulo que lo inscribe,
y luego devuelve el area y el centro del mayor.
"""    	
def findMaxArea(polygons):
	maxArea = 0
	maxCenter = [0,0]

	for polygon in polygons:
		success, center, radius = cv.MinEnclosingCircle(polygon)
		if success:
			area = cv.ContourArea(polygon)
			if (area>maxArea):
				maxArea=area
				maxCenter=center
	return maxArea, maxCenter

"""
Dado un conjunto de poligomnos calcula el centrol del circulo que lo inscribe,
y luego devuelve el centro de mas a la izquierda.
"""    	
def findMostRight(polygons):
	maxCenter = [0,0]
	maxPolygon = 0 # inicializo con algo :S
	
	for polygon in polygons:
		success, center, radius = cv.MinEnclosingCircle(polygon)
		if success: # and center[1]>min_y:
			if (center[0]>maxCenter[0]):
				maxCenter=center
				maxPolygon = polygon
	return maxCenter, maxPolygon
 
"""
Dado un poligono, conjunto de puntos, encuentra primero un circulo que lo inscribe para cada uno de ellos,
y luego devuelve el radio y el centro del mayor.
"""    	
def findMaxBoundinRect(polygons):
	for polygon in polygons:
		success, center , radius = cv.boundingRect(Mat(polygon))
		if success and (radius>maxRadius):
			maxRadius=radius
			maxCenter=center
	return maxRadius, maxCenter
