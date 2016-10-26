import time
from behavior import throwCan
from actuator import butiaAPI
#prueba del volcator

print "start..."
robot = butiaAPI.robot()
print robot.get_modules_list()
tc = throwCan.Throwcan(robot)
tc.reset()
time.sleep(2)
tc.motorSchema()

print "fin"




	  

