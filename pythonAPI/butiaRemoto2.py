#butiaRemoto
import butiaAPI
import string

butiabot = butiaAPI.robot()
modulos = butiabot.get_modules_list()
print modulos

a = 'z' #no hace nada
vel = 600

while a != 'q':
    a = raw_input("Choose your action f, v, j, k: ")
	
    if a == 'f':
        butiabot.set2MotorSpeed("0", str(vel), "0", str(vel))	#foreward
    elif a == 'v':
        butiabot.set2MotorSpeed("1",str(vel), "1", str(vel))	#backward
    elif a == 'j':
        butiabot.set2MotorSpeed("0",str(vel), "1", str(vel))	#left
    elif a == 'k':
        butiabot.set2MotorSpeed("1",str(vel), "0", str(vel))	#right	
    elif a == '+':
        if vel < 1023:
            vel = vel + 50
            print vel
    elif a == '-':
        if vel > 50:
            vel = vel - 50
            print vel
	else:	
		butiabot.set2MotorSpeed("0","0", "0", "0")	#stop	
		
		
						
		
