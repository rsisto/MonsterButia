# -*- coding: utf-8 -*-
# Copyright (c) 2011 Butiá Team butia@fing.edu.uy 
# Butia is a free open plataform for robotics projects
# www.fing.edu.uy/inco/proyectos/butia
# Universidad de la República del Uruguay
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


import apiSumoUY
import math
import gobject

from TurtleArt.tapalette import make_palette, palette_name_to_index
from TurtleArt.talogo import primitive_dictionary
from TurtleArt.taconstants import BLACK, WHITE, CONSTANTS, BOX_COLORS
from TurtleArt.tautils import debug_output

from gettext import gettext as _

from plugins.plugin import Plugin
   
class Sumtia(Plugin):
    
    def __init__(self, parent):
        self.tw = parent
        self.vel = 10

        self.api = apiSumoUY.apiSumoUY()
        self.api.setPuertos()
        self.api.conectarse() 

    def setup(self):        

        """ Setup is called once, when the Turtle Window is created. """     

        palette = make_palette('sumtia', colors=["#00FF00","#008000"], help_string=_('SumBot'))

        #add block about movement of butia, this blocks don't allow multiple instances

        primitive_dictionary['sendVelocities'] = self.sendVelocities
        palette.add_block('sendVelocities',  # the name of your block
                     style='basic-style-2arg',  # the block style
                     label=_('speed SumBot'),  # the label for the block
                     prim_name='sendVelocities',  # code reference (see below)
                     default=[10,10],
                     help_string=_('submit the speed to the SumBot'))
        self.tw.lc.def_prim('sendVelocities', 2, lambda self, x, y: primitive_dictionary['sendVelocities'](x, y))

        primitive_dictionary['setVel'] = self.setVel
        palette.add_block('setVel',  # the name of your block
                     style='basic-style-1arg',  # the block style
                     label=_('speed SumBot'),  # the label for the block
                     prim_name='setVel',  # code reference (see below)
                     default=[10],
                     help_string=_('set the default speed for the movement commands'))
        self.tw.lc.def_prim('setVel', 1, lambda self, x: primitive_dictionary['setVel'](x))

        primitive_dictionary['forwardSumtia'] = self.forwardSumtia
        palette.add_block('forwardSumtia',  # the name of your block
                     style='basic-style',  # the block style
                     label=_('forward SumBot'),  # the label for the block
                     prim_name='forwardSumtia',  # code reference (see below)
                     help_string=_('move SumBot forward'))
        self.tw.lc.def_prim('forwardSumtia', 0, lambda self: primitive_dictionary['forwardSumtia']())

        primitive_dictionary['backward'] = self.backward
        palette.add_block('backward',  # the name of your block
                     style='basic-style',  # the block style
                     label=_('backward SumBot'),  # the label for the block
                     prim_name='backward',  # code reference (see below)
                     help_string=_('move SumBot backward'))
        self.tw.lc.def_prim('backward', 0, lambda self: primitive_dictionary['backward']())
        
        primitive_dictionary['stopSumtia'] = self.stopSumtia
        palette.add_block('stopSumtia',  # the name of your block
                     style='basic-style',  # the block style
                     label=_('stop SumBot'),  # the label for the block
                     prim_name='stopSumtia',  # code reference (see below)
                     help_string=_('stop the SumBot'))
        self.tw.lc.def_prim('stopSumtia', 0, lambda self: primitive_dictionary['stopSumtia']())

        primitive_dictionary['turnLeft'] = self.turnLeft
        palette.add_block('turnLeft',  # the name of your block
                     style='basic-style',  # the block style
                     label=_('left SumBot'),  # the label for the block
                     prim_name='turnLeft',  # code reference (see below)
                     help_string=_('turn left the SumBot'))
        self.tw.lc.def_prim('turnLeft', 0, lambda self: primitive_dictionary['turnLeft']())

        primitive_dictionary['turnRight'] = self.turnRight
        palette.add_block('turnRight',  # the name of your block
                     style='basic-style',  # the block style
                     label=_('right SumBot'),  # the label for the block
                     prim_name='turnRight',  # code reference (see below)
                     help_string=_('turn right the SumBot'))
        self.tw.lc.def_prim('turnRight', 0, lambda self: primitive_dictionary['turnRight']())

        primitive_dictionary['angleToCenter'] = self.angleToCenter
        palette.add_block('angleToCenter',  # the name of your block
                     style='box-style',  # the block style
                     label=_('angle to center'),  # the label for the block
                     prim_name='angleToCenter',  # code reference (see below)
                     help_string=_('get the angle to the center of the dohyo'))
        self.tw.lc.def_prim('angleToCenter', 0, lambda self: primitive_dictionary['angleToCenter']())

        primitive_dictionary['angleToOpponent'] = self.angleToOpponent
        palette.add_block('angleToOpponent',  # the name of your block
                     style='box-style',  # the block style
                     label=_('angle to Enemy'),  # the label for the block
                     prim_name='angleToOpponent',  # code reference (see below)
                     help_string=_('get the angle to the Enemy'))
        self.tw.lc.def_prim('angleToOpponent', 0, lambda self: primitive_dictionary['angleToOpponent']())
        
        primitive_dictionary['getX'] = self.getX
        palette.add_block('getX',  # the name of your block
                     style='box-style',  # the block style
                     label=_('x coor. SumBot'),  # the label for the block
                     prim_name='getX',  # code reference (see below)
                     help_string=_('get the x coordinate of the SumBot'))
        self.tw.lc.def_prim('getX', 0, lambda self: primitive_dictionary['getX']())
        
        primitive_dictionary['getY'] = self.getY
        palette.add_block('getY',  # the name of your block
                     style='box-style',  # the block style
                     label=_('y coor. SumBot'),  # the label for the block
                     prim_name='getY',  # code reference (see below)
                     help_string=_('get the y coordinate of the SumBot'))
        self.tw.lc.def_prim('getY', 0, lambda self: primitive_dictionary['getY']())
        
        primitive_dictionary['getOpX'] = self.getOpX
        palette.add_block('getOpX',  # the name of your block
                     style='box-style',  # the block style
                     label=_('x coor. Enemy'),  # the label for the block
                     prim_name='getOpX',  # code reference (see below)
                     help_string=_('get the x coordinate of the Enemy'))
        self.tw.lc.def_prim('getOpX', 0, lambda self: primitive_dictionary['getOpX']())
        
        primitive_dictionary['getOpY'] = self.getOpY
        palette.add_block('getOpY',  # the name of your block
                     style='box-style',  # the block style
                     label=_('y coor. Enemy'),  # the label for the block
                     prim_name='getOpY',  # code reference (see below)
                     help_string=_('get the y coordinate of the Enemy'))
        self.tw.lc.def_prim('getOpY', 0, lambda self: primitive_dictionary['getOpY']())
        
        primitive_dictionary['getRot'] = self.getRot
        palette.add_block('getRot',  # the name of your block
                     style='box-style',  # the block style
                     label=_('rotation SumBot'),  # the label for the block
                     prim_name='getRot',  # code reference (see below)
                     help_string=_('get the rotation of the Sumbot'))
        self.tw.lc.def_prim('getRot', 0, lambda self: primitive_dictionary['getRot']())
        
        primitive_dictionary['getOpRot'] = self.getOpRot
        palette.add_block('getOpRot',  # the name of your block
                     style='box-style',  # the block style
                     label=_('rotation Enemy'),  # the label for the block
                     prim_name='getOpRot',  # code reference (see below)
                     help_string=_('get the rotation of the Enemy'))
        self.tw.lc.def_prim('getOpRot', 0, lambda self: primitive_dictionary['getOpRot']())
        
        primitive_dictionary['getDistCenter'] = self.getDistCenter
        palette.add_block('getDistCenter',  # the name of your block
                     style='box-style',  # the block style
                     label=_('distance to center'),  # the label for the block
                     prim_name='getDistCenter',  # code reference (see below)
                     help_string=_('get the distance to the center of the dohyo'))
        self.tw.lc.def_prim('getDistCenter', 0, lambda self: primitive_dictionary['getDistCenter']())
        
        primitive_dictionary['getDistOp'] = self.getDistOp
        palette.add_block('getDistOp',  # the name of your block
                     style='box-style',  # the block style
                     label=_('distance to Enemy'),  # the label for the block
                     prim_name='getDistOp',  # code reference (see below)
                     help_string=_('get the distance to the Enemy'))
        self.tw.lc.def_prim('getDistOp', 0, lambda self: primitive_dictionary['getDistOp']())
        
        primitive_dictionary['updateState'] = self.updateState
        palette.add_block('updateState',  # the name of your block
                     style='basic-style',  # the block style
                     label=_('update information'),  # the label for the block
                     prim_name='updateState',  # code reference (see below)
                     help_string=_('update information from the server'))
        self.tw.lc.def_prim('updateState', 0, lambda self: primitive_dictionary['updateState']())

    def start(self):
        """ start is called when run button is pressed. """
        pass
 
    def stop(self):
        """ stop is called when stop button is pressed. """
        pass

    def goto_background(self):
        """ goto_background is called when the activity is sent to the
        background. """
        pass

    def return_to_foreground(self):
        """ return_to_foreground is called when the activity returns to
        the foreground. """
        pass

    def quit(self):
        """ cleanup is called when the activity is exiting. """
        self.api.liberarRecursos()

    # Sumtia helper functions for apiSumoUY.py interaction

    def sendVelocities(self,vel_izq = 0, vel_der = 0):
        self.api.enviarVelocidades(vel_izq, vel_der)
        
    def setVel(self,vel = 0):
        self.vel = int(vel)

    def forwardSumtia(self):
        self.api.enviarVelocidades(self.vel, self.vel)

    def backward(self):
        self.api.enviarVelocidades(-self.vel, -self.vel)
        
    def stopSumtia(self):
        self.api.enviarVelocidades(0,0)

    def turnLeft(self):
        self.api.enviarVelocidades(-self.vel, self.vel)


    def turnRight(self):
        self.api.enviarVelocidades(self.vel, -self.vel)
        
    def getX(self):
        return self.api.getCoorX()
    
    def getY(self):
        return self.api.getCoorY()
    
    def getOpX(self):
        return self.api.getCoorXOp()
    
    def getOpY(self):
        return self.api.getCoorYOp()
    
    def getRot(self):
        return self.api.getRot()
    
    def getOpRot(self):
        return self.api.getRotOp()

    def angleToCenter(self):
        rot = math.degrees(math.atan2(self.api.getCoorY(), self.api.getCoorX())) + (180 - self.getRot())
        return (rot - 360) if abs(rot) > 180 else rot 

    def angleToOpponent(self):
        x = self.getX() - self.getOpX()
        y = self.getY() - self.getOpY()
        rot = math.degrees(math.atan2(y, x)) + (180 - self.getRot())
        return (rot - 360) if abs(rot) > 180 else rot 
    
    def getDistCenter(self):
        return math.sqrt(math.pow(self.getX(), 2) + math.pow(self.getY(), 2))
    
    def getDistOp(self):
        return math.sqrt(math.pow(self.getX() - self.getOpX(), 2) +
                        math.pow(self.getY() - self.getOpY(), 2))
    
    def updateState(self):
        err = self.api.getInformacion()
        if err == -1:
            print "Error getting information"


