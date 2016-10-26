import wraperSerialcomm
import time

comandoLedOn = 'A'
comandoLedOff = 'B'
datos = ""
count = 0
while count < 9:	
	send_res = wraperSerialcomm.sendData(comandoLedOn, "/dev/ttyUSB0")
	print send_res
	time.sleep(3)
	send_res = wraperSerialcomm.sendData(comandoLedOff, "/dev/ttyUSB0")
	time.sleep(3)
	print send_res
	count = count + 1


