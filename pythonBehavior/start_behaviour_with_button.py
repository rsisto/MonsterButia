from actuator import butiaAPI
#import time


monster_butia = butiaAPI.robot()
button_pressed = False
print "Esperando boton"
while not button_pressed :
  button_pressed = not bool(monster_butia.getButton(6))
  #print "estado del boton: " + str( button_pressed)
  
  #time.sleep(1)
print "Boton presionado"
#import B_4_wanderTakeCanAvoidTest
#import C_4_wanderThrowCanTest
import D_1_FindAndThrowCansTest

