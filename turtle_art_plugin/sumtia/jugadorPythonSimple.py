import apiSumoUY

error = False

try:
	api = apiSumoUY.apiSumoUY()
	api.setPuertos(7001, "127.0.0.1", 8001)
	err = api.conectarse()	
except:
	error = True
	print "Error trying to connect..."	
	
			
if err == -1:
	error = True
	
api.enviarVelocidades(1,-1)

while not error:
	api.getInformacion()
	print "X: " + str(api.getCoorX())
	print "Rot: " + str(api.getRot())
	print "Ang to C: " + str(api.getAngleToCenter())