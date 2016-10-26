import wraperSerialcomm
import time
comando = chr(0xff) + chr(0x01) + chr(0x02) + chr(0x02)
send_res = wraperSerialcomm.sendData(comando, "/dev/ttyUSB0")
print send_res
#send_res = wraperSerialcomm.sendData(0x01, "/dev/ttyUSB0")
#print send_res
#send_res = wraperSerialcomm.sendData(0x02, "/dev/ttyUSB0")
#print send_res
#send_res = wraperSerialcomm.sendData(0x02, "/dev/ttyUSB0")
#print send_res
#time.sleep(3)


