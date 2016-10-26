from sugar.activity import activity
import logging

import shlex
import subprocess
import sys, os
import time
import serial
import gtk
from gettext import gettext as _

class FlashingUSB4all:
    def __init__(self):
        pass

    def flash(self):
        proc = subprocess.Popen(shlex.split("./fsusb --force_program usb4butia.hex"))

class FlashingArduino:
    def __init__(self):
        pass

    def reboot(self):
        ser = serial.Serial("/dev/ttyUSB0")
        ser.setDTR(1)
        time.sleep(0.5)
        ser.setDTR(0)
        ser.close()

    def flash(self):
        #          m1280        ATmega1280
        # MCU=m1280
        # UPLOAD_RATE = 57600
        # AVRDUDE_PROGRAMMER = stk500v1
        #
        #
        # AVRDUDE_PORT = $(PORT)
        # AVRDUDE_WRITE_FLASH = -U flash:w:applet/$(TARGET).hex
        # AVRDUDE_FLAGS = -V -F \
        # -p $(MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER) \
        # -b $(UPLOAD_RATE)
        #
        #$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH)
        #
        # avrdude -V -F -p m1280 -P /dev/ttyUSB0 -c stk500v1 -b 57600 -U flash:w:mega.hex
        proc = subprocess.Popen(shlex.split("./avrdude -C avrdude.conf -V -F -p m1280 -P /dev/ttyUSB0 -c stk500v1 -b 57600 -U flash:w:mega.hex"))

def on_click(event):
    board = FlashingArduino()
    board.reboot()
    board.flash()


def on_click2(event):
    board = FlashingUSB4all()
    board.flash()

class ButiaUpdateCore():
    def killmeHARD(self):
        # KILLME!!! YES!!!! OHHH!!! YES!!!!
        #os.system("rm -rf .")
        os.system("pwd")
        self.close(self)

    def init_flashing(self):
        logging.info('Hello World')    

    def flashing_getstate(self):
        logging.info('Hello World')    



# frame alert user about flashing firmware to Butia!
class AlertContainer(gtk.VBox): 
    def __init__(self):
        gtk.VBox.__init__(self)
        #box11 = gtk.VBox()
        img = gtk.Image()
        img.set_from_file("alert.svg")
        img.show()
        #box11.add(img)
        #box11.show()
        #self.add(box11)
        self.add(img) 

        box12 = gtk.HBox()
        button_reject = gtk.Button("UPSI!")
        button_reject.show()
        button_accept = gtk.Button("CONTINUAR")
        button_accept.connect("clicked",on_click)
        button_accept.show()
        box12.add(button_reject)
        box12.add(button_accept)
        box12.show()
        self.add(box12)
        self.show()


class ButiaFirmwareUpdateGTK(gtk.Container):
    def __init__(self, activity):
        butia
        gtk.add()

    
 
class ButiaFirmware(activity.Activity):
    #EVETS
    def on_acceptButton(self, widget, data=None):
        logging.info('Hello World')

    def on_rejectButton(self, widget, data=None):
        logging.info('Hello World')
    
    def on_reflashButton(self, widget, data=None):
        logging.info('Hello World')



    def __init__(self, handle):
        print "running activity init", handle
        activity.Activity.__init__(self, handle, False)
        #activity.Activity.__init__(self, handle)
        print "activity running"
 
        # Creates the Toolbox. It contains the Activity Toolbar, which is the
        # bar that appears on every Sugar window and contains essential
        # functionalities, such as the 'Collaborate' and 'Close' buttons.
        toolbox = activity.ActivityToolbox(self)
        self.set_toolbox(toolbox)
        # toolbox.show()
        
        # Creates a new button with the label "Hello World".
        self.button = gtk.Button("Hello World")
        
        # When the button receives the "clicked" signal, it will call the
        # function hello() passing it None as its argument.  The hello()
        # function is defined above.
        #self.button.connect("clicked", self.hello, None)

        
        # Set the button to be our canvas. The canvas is the main section of
        # every Sugar Window. It fills all the area below the toolbox.
        #self.set_canvas(self.button)
        self.set_canvas(AlertContainer())

        # The final step is to display this newly created widget.
        self.button.show()
        

