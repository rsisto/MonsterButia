import butiaAPI
import time

butiabot = butiaAPI.robot()

modulos = butiabot.listarModulos()

print modulos
butiabot.abrirButia()
carga = butiabot.getCargaBateria()
print carga
ver = butiabot.getVersion()
print ver

butiabot.abrirLback()
datoRet = butiabot.loopBack("hola")
print datoRet

butiabot.abrirMotores()
datoRet2 = butiabot.setVelocidadMotores("0", "1000", "0", "500")
print datoRet2
time.sleep(1)
datoRet3 = butiabot.setVelocidadMotores("0", "0", "0", "0")
print datoRet3
datoRet4 = butiabot.setVelMotor("0","1","1023")
time.sleep(1)
print datoRet4
datoRet4 = butiabot.setVelMotor("0","1","0")
datoRet4 = butiabot.setVelMotor("1","0","452")
time.sleep(1)
print datoRet4
datoRet4 = butiabot.setVelMotor("1","1","0")

butiabot.abrirSensor()
sen1 = butiabot.getValSenAnalog("2")
print sen1
sen2 = butiabot.getValSenAnalog("4")
print sen2
sen3 = butiabot.getValSenDigital("5")
print sen3









butiabot.cerrar()


