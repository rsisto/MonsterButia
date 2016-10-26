import wraperSerialcomm
import time
PRENDER = 'A' + 'M'
APAGAR = 'A' + 'N'
send_res = wraperSerialcomm.sendData(PRENDER, "/dev/ttyUSB0")
print send_res
time.sleep(3)
send_res = wraperSerialcomm.sendData(APAGAR, "/dev/ttyUSB0")
print send_res
#send_res = wraperSerialcomm.sendData(0x02, "/dev/ttyUSB0")
#print send_res
#send_res = wraperSerialcomm.sendData(0x02, "/dev/ttyUSB0")
#print send_res
#time.sleep(3)


