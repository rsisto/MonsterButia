#! /usr/bin/env python
# -*- coding: utf-8 -*-
# 
# ButiaAPI
# Copyright (c) 2009, 2010, 2011, 2012 Butiá Team butia@fing.edu.uy 
# Butia is a free open plataform for robotics projects
# www.fing.edu.uy/inco/proyectos/butia
# Facultad de Ingenieria - Universidad de la República - Uruguay
#
# Implements abstractions for the comunications with the bobot-server
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

import socket
import string
import math
from config import configProperties
import threading, commands, time, subprocess

ERROR_SENSOR_READ = -1

BUTIA_1 = 20

BOBOT_HOST = 'localhost'
BOBOT_PORT = 2009

class robot:
    

    def __init__(self, host = BOBOT_HOST, port = BOBOT_PORT):
        """
        init the robot class
        """
        #self.bobot_launch()
        self.lock = threading.Lock()
        self.host = host
        self.port = port
        self.client = None
        self.fclient = None
        self.ver = ERROR_SENSOR_READ
        self.reconnect()

       
    def doCommand(self, msg):
        """
        Executes a command in butia.
        @param msg message to be executed
        """
        msg = msg +'\n'
        ret = ERROR_SENSOR_READ
        self.lock.acquire()
        try:     
            self.client.send(msg) 
            ret = self.fclient.readline()
            ret = ret[:-1]
        except:
            ret = ERROR_SENSOR_READ # Doesn't return here to release the lock
        self.lock.release()
        
        if ((ret == 'nil value') or (ret == None) or (ret == 'fail') or (ret == 'missing driver')):
            ret = ERROR_SENSOR_READ
        return ret
          
    # connect o reconnect the bobot
    def reconnect(self):
        self.close()
        try:
            self.client = socket.socket()
            self.client.connect((self.host, self.port))  
            self.fclient = self.client.makefile()
            msg = 'INIT'
            #bobot server instance is running, but we have to check for new or remove hardware
            self.doCommand(msg)
        except:
            return ERROR_SENSOR_READ
        return 0

    # ask bobot for refresh is state of devices connected
    def refresh(self):
        if (self.ver == BUTIA_1) or (self.ver == ERROR_SENSOR_READ):
            msg = 'INIT'
        else:
            msg = 'REFRESH'
        return self.doCommand(msg)

    # close the comunication with the bobot
    def close(self):
        try:
            if self.fclient != None:
                self.fclient.close()
                self.fclient = None
            if self.client != None:
                self.client.close()
                self.client = None
        except:
            return ERROR_SENSOR_READ
        return 0

    #######################################################################
    ### Operations to the principal module
    #######################################################################


    # call the module 'modulename'
    def callModule(self, modulename, function , params = ''):
        msg = 'CALL ' + modulename + ' ' + function
        if params != '' :
            msg += ' ' + params
        ret = self.doCommand(msg)
        try:
            ret = int(ret)
        except:
            ret = ERROR_SENSOR_READ
        return ret

    # Close bobot service
    def closeService(self):
        msg = 'QUIT'
        return self.doCommand(msg)

    #######################################################################
    ### Useful functions 
    #######################################################################

    # returns if the module_name is present
    def isPresent(self, module_name):
        module_list = self.get_modules_list()
        return (module_name in module_list)

    # returns a list of modules
    def get_modules_list(self):
        msg = 'LIST'
        l = []
        ret = self.doCommand(msg)
        if not (ret == '' or ret == ERROR_SENSOR_READ):
            l = ret.split(',')
        return l

    # loopBack: send a message to butia and wait to recibe the same
    def loopBack(self, data):
        msg = 'lback send ' + data
        ret = self.doCommand(msg)
        if ret != -1 :
            msg = 'CALL lback read'
            return self.doCommand(msg)
        else:
            return ERROR_SENSOR_READ
            

    #######################################################################
    ### Operations for motores.lua driver
    #######################################################################

    def set2MotorSpeed(self, leftSense = '0', leftSpeed = '0', rightSense = '0', rightSpeed = '0'):
        msg = leftSense + ' ' + leftSpeed + ' ' + rightSense + ' ' + rightSpeed
        if self.ver == BUTIA_1:
            return self.callModule('motores', 'setvel2mtr', msg)
        else:
            return self.callModule('motors', 'setvel2mtr', msg)
     
    def setMotorSpeed(self, idMotor = '0', sense = '0', speed = '0'):
        msg = idMotor + ' ' + sense + ' ' + speed
        if self.ver == BUTIA_1:
            return self.callModule('motores', 'setvelmtr', msg)
        else:
            return self.callModule('motors', 'setvelmtr', msg)

    #######################################################################
    ### Operations for ax.lua driver
    #######################################################################

    def wheel_mode(self, idMotor = '0'):
        msg = idMotor
        if self.ver == BUTIA_1:
            return self.callModule('ax', 'wheel_mode', msg) ##TODO implement
        else:
            return self.callModule('ax', 'wheel_mode', msg)
     
    def joint_mode(self, idMotor = '0', min = '0', max = '1023'):
        msg = idMotor + ' ' + min + ' ' + max
        if self.ver == BUTIA_1:
            return self.callModule('ax', 'joint_mode', msg) ##TODO implement
        else:
            return self.callModule('ax', 'joint_mode', msg)

    def set_position(self, idMotor = '0', pos = '0'):
        msg = idMotor + ' ' + pos
        if self.ver == BUTIA_1:
            return self.callModule('ax', 'set_position', msg) ##TODO implement
        else:
            return self.callModule('ax', 'set_position', msg)

    def get_position(self, idMotor = '0'):
        msg = idMotor
        if self.ver == BUTIA_1:
            return self.callModule('ax', 'get_position', msg) ##TODO implement
        else:
            return self.callModule('ax', 'get_position', msg)



    #######################################################################
    ### Operations for butia.lua driver
    #######################################################################

    def ping(self):
        return self.callModule('placa', 'ping')

    # returns the approximate charge of the battery        
    def getBatteryCharge(self):
        return self.callModule('butia', 'get_volt')

    # returns the firmware version 
    def getVersion(self):
        self.ver = self.callModule('butia', 'read_ver')
        return self.ver
    
    # set de motor idMotor on determinate angle
    def setPosition(self, idMotor = 0, angle = 0):
        msg = str(idMotor) + ' ' + str(angle)
        return self.callModule('placa', 'setPosicion' , msg )
    
    # return the value of button: 1 if pressed, 0 otherwise
    def getButton(self, number=''):
        if self.ver == BUTIA_1:
            return self.callModule('boton' + str(number), 'getValue')
        else:
            return self.callModule('button:' + str(number), 'getValue')

    # return the value en ambient light sensor
    def getAmbientLight(self, number=''):
        if self.ver == BUTIA_1:
            return self.callModule('luz' + str(number), 'getValue')
        else:
            return self.callModule('light:' + str(number), 'getValue')

    # return the value of the distance sensor
    def getDistance(self, number=''):
        if self.ver == BUTIA_1:
            return self.callModule('dist' + str(number), 'getValue')
        else:
            return self.callModule('distanc:' + str(number), 'getValue')
    
    # return the value of the grayscale sensor
    def getGrayScale(self, number=''):
        if self.ver == BUTIA_1:
            return self.callModule('grises' + str(number), 'getValue')
        else:
            return self.callModule('grey:' + str(number), 'getValue')

    # return the value of the temperature sensor
    def getTemperature(self, number=''):
        if self.ver == BUTIA_1:
            return self.callModule('temp' + str(number), 'getValue')
        else:
            return self.callModule('temp:' + str(number), 'getValue')

    # return the value of the vibration sensor
    def getVibration(self, number=''):
        if self.ver == BUTIA_1:
            return self.callModule('vibra' + str(number), 'getValue')
        else:
            return self.callModule('vibra:' + str(number), 'getValue')

    # return the value of the tilt sensor
    def getTilt(self, number=''):
        if self.ver == BUTIA_1:
            return self.callModule('tilt' + str(number), 'getValue')
        else:
            return self.callModule('tilt:' + str(number), 'getValue')

    # FIXME: the name of the module and the function...
    # return the value of the capacitive touch sensor
    def getCapacitive(self, number=''):
        if self.ver == BUTIA_1:
            return self.callModule('capacitive' + str(number), 'getValue')
        else:
            return self.callModule('capacitive:' + str(number), 'getValue')

    # return the value of the magnetic induction sensor
    def getMagneticInduction(self, number=''):
        if self.ver == BUTIA_1:
            return self.callModule('magnet' + self.aux + str(number), 'getValue')
        else:
            return self.callModule('magnet:' + self.aux + str(number), 'getValue')

    # set the led intensity
    def setLed(self, nivel = 255, number= ''):
        if self.ver == BUTIA_1:
            return self.callModule('led' + self.aux + str(number), 'setLight', str(math.trunc(nivel)))
        else:
            return self.callModule('led:' + self.aux + str(number), 'setLight', str(math.trunc(nivel)))

    # FIXME: check the lenght of text?
    # write a text in LCD display
    def writeLCD(self, text):
        text = str(text)
        text = text.replace(' ', '_')
        self.callModule('display', 'escribir' , text)
        
    def bobot_launch(self):
        """
        launch bobot-server.lua with a lua virtual machine modified to locally
        resolve library dependences located in the bin directory of tortugarte.
        And without libreadline and libhistory dependency
        """
        output = commands.getoutput('ps -ax | grep lua')
        if 'bobot-server' in output:
            print('Bobot is alive!')
        else:
            config = configProperties.ConfigProperties()
            BOBOT_DIR = config.getPropertie("Butia","BOBOT_DIR")
            print('creating Bobot')
            self.bobot = subprocess.Popen(['lua', 'bobot-server.lua'], cwd=BOBOT_DIR)

        # Sure that bobot is running
        time.sleep(3)

