#!/usr/bin/env python
# Copyright (c) 2011 Alan Aguiar, <alanjas@hotmail.com>
# Copyright (c) 2011 Aylen Ricca, <ar18_90@hotmail.com>
# Copyright (c) 2011 Rodrigo Dearmas, <piegrande46@hotmail.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

import gtk
import sys, os
import logging
from gettext import gettext as _
from plugins.plugin import Plugin
from TurtleArt.tapalette import make_palette
from TurtleArt.talogo import primitive_dictionary, logoerror
from TurtleArt.tapalette import special_block_colors
from TurtleArt.tautils import convert
from TurtleArt.tawindow import block_names
pycam = None
try:
    import pygame
    import pygame.camera as pycam
except ImportError:
    print _('Error importing Pygame. This plugin require Pygame 1.9')

COLOR_NOTPRESENT = ["#A0A0A0","#808080"]
_logger = logging.getLogger('turtleart-activity followme plugin')


class Followme(Plugin):

    def __init__(self, parent):
        self.parent = parent
        self.cam_on = False
        self.cam_present = False
        self.tamanioc = (320, 240)
        self.colorc = (255, 255, 255)
        self.threshold = (25, 25, 25)
        self.pixels_min = 10
        self.pixels = 0
        self.brightness = 128
        self.use_average = True
        self.calibrations = {}
        self.cam = None
        self.mask = None
        self.connected = None
        self.capture = None
        self.lcamaras = []
        pygame.init()
        if pycam:
            pycam.init()
            self.get_camera('RGB')

    def get_camera(self, mode):
        self.stop_camera()
        self.cam_present = False
        self.lcamaras = pygame.camera.list_cameras()
        if self.lcamaras:
            self.cam = pygame.camera.Camera(self.lcamaras[0], self.tamanioc, mode)
            try:
                self.cam.start()
                self.set_camera_flags()
                self.cam.stop()
                self.capture = pygame.surface.Surface(self.tamanioc)
                self.capture_aux = pygame.surface.Surface(self.tamanioc)
                self.cam_present = True
            except:
                print _('Error on initialization of the camera')
        else:
            print _('No cameras was found')

    def set_camera_flags(self):
        if (self.brightness == -1):
            self.cam.set_controls(True, False)
        else:
            self.cam.set_controls(True, False, self.brightness)
        res = self.cam.get_controls()
        self.flip = res[0]
        self.tamanioc = self.cam.get_size()
        self.x_offset = int(self.tamanioc[0] / 2.0 - 5)
        self.y_offset = int(self.tamanioc[1] / 2.0 - 5)

    def stop_camera(self):
        if (self.cam_present and self.cam_on):
            try:
                self.cam.stop()
                self.cam_on = False
            except:
                print _('Error in stop camera')

    def start_camera(self):
        if (self.cam_present and not(self.cam_on)):
            try:
                self.cam.start()
                self.set_camera_flags()
                self.cam_on = True
            except:
                print _('Error in start camera')

    def get_mask(self):
        if self.cam_on:
            try:
                self.capture = self.cam.get_image(self.capture)
                pygame.transform.threshold(self.capture_aux, self.capture, self.colorc, 
                            (self.threshold[0],self.threshold[1], self.threshold[2]), (0,0,0), 2)
                self.mask = pygame.mask.from_threshold(self.capture_aux, self.colorc, self.threshold)
            except:
                print _('Error in get mask')
        return self.mask

    def calc_luminance(self):
        self.start_camera()
        if self.cam_on:
            self.capture = self.cam.get_image(self.capture)
            # Average the 100 pixels in the center of the screen
            r, g, b = 0, 0, 0
            for y in range(10):
                s = self.y_offset + y
                for x in range(10):
                    color = self.capture.get_at((self.x_offset + x, s))
                    r += color[0]
                    g += color[1]
                    b += color[2]

            return int((r * 0.3 + g * 0.6 + b * 0.1) / 100)

        else:
            return -1


    def dynamicLoadBlockColors(self):
        if not(self.cam_present):
            special_block_colors['threshold'] = COLOR_NOTPRESENT
            special_block_colors['savecalibrationN'] = COLOR_NOTPRESENT
            special_block_colors['calibrationN'] = COLOR_NOTPRESENT
            special_block_colors['brightness_f'] = COLOR_NOTPRESENT
            special_block_colors['xposition'] = COLOR_NOTPRESENT
            special_block_colors['yposition'] = COLOR_NOTPRESENT
            special_block_colors['pixels'] = COLOR_NOTPRESENT
            special_block_colors['follow'] = COLOR_NOTPRESENT
            special_block_colors['pixels_min'] = COLOR_NOTPRESENT
            special_block_colors['camera_mode'] = COLOR_NOTPRESENT
            special_block_colors['mode_rgb'] = COLOR_NOTPRESENT
            special_block_colors['mode_yuv'] = COLOR_NOTPRESENT
            special_block_colors['mode_hsv'] = COLOR_NOTPRESENT
            special_block_colors['brightness_w'] = COLOR_NOTPRESENT
            special_block_colors['average_color'] = COLOR_NOTPRESENT

    def setup(self):

        self.dynamicLoadBlockColors()

        palette = make_palette('FollowMe', colors=["#00FF00","#008000"],
                                help_string=_('FollowMe'))


        primitive_dictionary['threshold'] = self.prim_threshold
        palette.add_block('threshold',
                        style='basic-style-3arg',
                        label=[(_('threshold') + '  ' + _('G')), _('R'), _('B')],
                        default=[25, 25, 25],
                        help_string=_('set a threshold for a RGB color'),
                        prim_name='threshold')
        self.parent.lc.def_prim('threshold', 3, lambda self, x, y, z:
                        primitive_dictionary['threshold'](x, y, z))

        primitive_dictionary['savecalibration'] = self._prim_savecalibration
        palette.add_block('savecalibrationN',
                          style='basic-style-1arg',
                          label=_('calibration'),
                          prim_name='savecalibrationN',
                          string_or_number=True,
                          default='1',
                          help_string=_('stores a personalized calibration'))
        self.parent.lc.def_prim('savecalibrationN', 1,
                             lambda self, x: primitive_dictionary['savecalibration'](
                'calibration', x))

        primitive_dictionary['calibration'] = self._prim_calibration
        palette.add_block('calibrationN',
                          style='number-style-1strarg',
                          label=_('calibration'),
                          prim_name='calibrationN',
                          string_or_number=True,
                          default='1',
                          help_string=_('return a personalized calibration'))
        self.parent.lc.def_prim('calibrationN', 1,
                             lambda self, x: primitive_dictionary['calibration']('calibration', x))

        primitive_dictionary['follow'] = self.prim_follow
        palette.add_block('follow',
                        style='basic-style-1arg',
                        label=_('follow'),
                        help_string=_('follow a color or calibration'),
                        prim_name='follow')
        self.parent.lc.def_prim('follow', 1, lambda self, x:
                        primitive_dictionary['follow'](x))

        primitive_dictionary['xposition'] = self.prim_xposition
        palette.add_block('xposition',
                        style='box-style',
                        label=_('x position'),
                        help_string=_('return x position'),
                        value_block=True,
                        prim_name='xposition')
        self.parent.lc.def_prim('xposition', 0, lambda self:
                        primitive_dictionary['xposition']())

        primitive_dictionary['yposition'] = self.prim_yposition
        palette.add_block('yposition',
                        style='box-style',
                        label=_('y position'),
                        help_string=_('return y position'),
                        value_block=True,
                        prim_name='yposition')
        self.parent.lc.def_prim('yposition', 0, lambda self:
                        primitive_dictionary['yposition']())

        primitive_dictionary['pixels'] = self.prim_pixels
        palette.add_block('pixels',
                        style='box-style',
                        label=_('pixels'),
                        help_string=_('return the number of pixels of the biggest blob'),
                        value_block=True,
                        prim_name='pixels')
        self.parent.lc.def_prim('pixels', 0, lambda self:
                        primitive_dictionary['pixels']())

        primitive_dictionary['brightness_f'] = self.prim_brightness
        palette.add_block('brightness_f',
                        style='basic-style-1arg',
                        label=_('brightness'),
                        default=128,
                        help_string=_('set the camera brightness as a value between 0 to 255. Use -1 to enable the auto-brightness'),
                        prim_name='brightness_f')
        self.parent.lc.def_prim('brightness_f', 1, lambda self, x:
                        primitive_dictionary['brightness_f'](x))

        primitive_dictionary['pixels_min'] = self.prim_pixels_min
        palette.add_block('pixels_min',
                        style='basic-style-1arg',
                        label=_('minimum pixels'),
                        default=10,
                        help_string=_('set the minimal number of pixels to follow'),
                        prim_name='pixels_min')
        self.parent.lc.def_prim('pixels_min', 1, lambda self, x:
                        primitive_dictionary['pixels_min'](x))

        primitive_dictionary['camera_mode'] = self.prim_camera_mode
        palette.add_block('camera_mode',
                        style='basic-style-1arg',
                        label=_('camera mode'),
                        default='RGB',
                        help_string=_('set the color mode of the camera: RGB; YUV or HSV'),
                        prim_name='camera_mode')
        self.parent.lc.def_prim('camera_mode', 1, lambda self, x:
                        primitive_dictionary['camera_mode'](x))

        primitive_dictionary['mode_rgb'] = self.prim_mode_rgb
        palette.add_block('mode_rgb',
                        style='box-style',
                        label=_('RGB'),
                        help_string=_('set the color mode of the camera to RGB'),
                        value_block=True,
                        prim_name='mode_rgb')
        self.parent.lc.def_prim('mode_rgb', 0, lambda self:
                        primitive_dictionary['mode_rgb']())

        primitive_dictionary['mode_yuv'] = self.prim_mode_yuv
        palette.add_block('mode_yuv',
                        style='box-style',
                        label=_('YUV'),
                        help_string=_('set the color mode of the camera to YUV'),
                        value_block=True,
                        prim_name='mode_yuv')
        self.parent.lc.def_prim('mode_yuv', 0, lambda self:
                        primitive_dictionary['mode_yuv']())

        primitive_dictionary['mode_hsv'] = self.prim_mode_hsv
        palette.add_block('mode_hsv',
                        style='box-style',
                        label=_('HSV'),
                        help_string=_('set the color mode of the camera to HSV'),
                        value_block=True,
                        prim_name='mode_hsv')
        self.parent.lc.def_prim('mode_hsv', 0, lambda self:
                        primitive_dictionary['mode_hsv']())

        primitive_dictionary['brightness_w'] = self.calc_luminance
        palette.add_block('brightness_w',
                        style='box-style',
                        label=_('get brightness'),
                        help_string=_('get the brightness of the ambient'),
                        prim_name='brightness_w')
        self.parent.lc.def_prim('brightness_w', 0, lambda self:
                        primitive_dictionary['brightness_w']())

        primitive_dictionary['average_color'] = self.prim_average_color
        palette.add_block('average_color',
                        style='basic-style-1arg',
                        label=_('average color'),
                        default=1,
                        help_string=_('if 0: average color is off when calibrates; for other values is on'),
                        prim_name='average_color')
        self.parent.lc.def_prim('average_color', 1, lambda self, x:
                        primitive_dictionary['average_color'](x))

    def stop(self):
        self.stop_camera()

    def quit(self):
        self.stop_camera()
            
    def clear(self):
        pass

    def prim_camera_mode(self, mode):
        m = 'RGB'
        try:
            m = str(mode)
        except:
            pass
        m = m.upper()
        if (m == 'RGB') or (m == 'YUV') or (m == 'HSV'):
            self.get_camera(m)

            if (m == 'RGB'):
                label_0 = _('threshold') + '  ' + _('G')
                label_1 = _('R')
                label_2 = _('B')
            elif (m == 'YUV'):
                label_0 = _('threshold') + '  ' + _('U')
                label_1 = _('Y')
                label_2 = _('V')
            elif (m == 'HSV'):
                label_0 = _('threshold') + '  ' + _('S')
                label_1 = _('H')
                label_2 = _('V')

            #repaints program area blocks (proto) and palette blocks (block)
            for blk in self.parent.block_list.list:
                #NOTE: blocks types: proto, block, trash, deleted
                if blk.type in ['proto', 'block']:
                    if (blk.name == 'threshold'):
                        blk.spr.set_label(label_0, 0)
                        blk.spr.set_label(label_1, 1)
                        blk.spr.set_label(label_2, 2)
                        block_names[blk.name][0] = label_0
                        block_names[blk.name][1] = label_1
                        block_names[blk.name][2] = label_2
                        blk.refresh()

                
    def prim_mode_rgb(self):
        return 'RGB'

    def prim_mode_yuv(self):
        return 'YUV'

    def prim_mode_hsv(self):
        return 'HSV'

    def prim_follow(self, x):
        if type(x) == str:
            self.colorc = self.str_to_tuple(x)
        else:
            self.colorc = (255, 255, 255)

    def prim_brightness(self, x):
        self.brightness = int(x)
        self.set_camera_flags()
            
    def prim_threshold(self, R, G, B):
        R = int(R)
        G = int(G)
        B = int(B)

        if (R < 0) or (R > 255):
            R = 25
        if (G < 0) or (G > 255):
            G = 25
        if (B < 0) or (B > 255):
            B = 25

        self.threshold = (R, G, B)
    
    def prim_pixels_min(self, x):
        if type(x) == float:
            x = int(x)
        if x < 0:
            x = 1
        self.pixels_min = x

    def prim_average_color(self, x):
        if x == 0:
            self.use_average = False
        else:
            self.use_average = True

    def calibrate(self):
        self.colorc = (255, 255, 255)
        self.start_camera()
        x = int((self.tamanioc[0] - 50) / 2.0)
        y = int((self.tamanioc[1] - 50) / 2.0)
        x_m = int(self.tamanioc[0] / 2.0)
        y_m = int(self.tamanioc[1] / 2.0)
        if self.cam_on:
            self.screen = pygame.display.set_mode((1200,900))
            self.clock = pygame.time.Clock()
            self.clock.tick(10)
            self.run = True
            while self.run:
                while gtk.events_pending():
                    gtk.main_iteration()
                for event in pygame.event.get():
                    if event.type == pygame.QUIT:
                        self.run = False
                    # click o tecla
                    elif event.type == 3:
                        self.run = False
                self.capture = self.cam.get_image(self.capture)
                if not(self.flip):
                    self.capture = pygame.transform.flip(self.capture, True, False)
                self.screen.blit(self.capture, (0,0))
                rect = pygame.draw.rect(self.screen, (255,0,0), (x,y,50,50), 4)
                if self.use_average:
                    self.colorc = pygame.transform.average_color(self.capture, rect)
                else:
                    self.colorc = self.capture.get_at((x_m, y_m))
                self.screen.fill(self.colorc, (self.tamanioc[0],self.tamanioc[1],100,100))
                pygame.display.flip()
            self.screen = pygame.display.quit()
    
        return (self.colorc[0], self.colorc[1], self.colorc[2])

    def prim_xposition(self):
        res = -1
        self.start_camera()
        mask = self.get_mask()            
        self.connected = mask.connected_component()
        if (self.connected.count() > self.pixels):
            centroid = self.mask.centroid()
            if self.flip:
                res = centroid[0]
            else:
                res = self.tamanioc[0] - centroid[0]
        return res

    def prim_yposition(self):
        res = -1
        self.start_camera()
        mask = self.get_mask()            
        self.connected = mask.connected_component()
        if (self.connected.count() > self.pixels):
            centroid = self.mask.centroid()
            res = self.tamanioc[1] - centroid[1]
        return res

    def prim_pixels(self):
        res = -1
        self.start_camera()
        mask = self.get_mask()            
        self.connected = mask.connected_component()
        res = self.connected.count()
        return res

    def _prim_savecalibration(self, name, x):
        c = self.calibrate()
        if x is not None:
            if type(convert(x, float, False)) == float:
                if int(float(x)) == x:
                    x = int(x)
            name = name + str(x)
        s = str(c[0]) + ', ' + str(c[1]) + ', ' + str(c[2])
        self.calibrations[name] = s
        #self.tw.lc.update_label_value(name, val)


    def _prim_calibration(self, name, x):
        if x is not None:
            if type(convert(x, float, False)) == float:
                if int(float(x)) == x:
                    x = int(x)
            name = name + str(x)
        if self.calibrations.has_key(name):
            return self.calibrations[name]
        else:
            raise logoerror(_('empty calibration'))

    def str_to_tuple(self, x):
        try:
            t = x.split(',')
            return (int(t[0]), int(t[1]), int(t[2]))
        except:
            raise logoerror(_('error in string conversion'))

