#####################################################
# * Butiá Arduino Firmware - Makefile
# * version:     0.2
# * date:        19-8-2010
# * Authors:	Andrés Aguirre, Pablo Gindel, Jorge Visca, Gonzalo Tejera,
# *             Santiago Margni, Federico Andrade, Guillermo Reisch
# *
# * (c) MINA Group, Facultad de Ingeniería, Universidad de la República, Uruguay. 
# This software is copyrigth .....//FIXME
#####################################################

OBJS       = ax12.o butia_firmware_mega_0_2.o conector.o info.o perifericos.pde comunicacion.o  modulos.o PnP.o
SALIDA	   = blablabla

ARDUINO = /home/romina/Documents/arduino/arduino-0018/
ARDUINO_SRC = $(ARDUINO)/hardware/arduino/cores/arduino
ARDUINO_LIBS = $(ARDUINO)/libraries/
# Arduino Libs 
# EEPROM  Ethernet  Firmata  LiquidCrystal  Matrix  NewSoftSerial  Servo  SoftwareSerial  Sprite  Stepper  Wire
ARDUINO_ILIBS = -I$(ARDUINO_LIBS)EEPROM -I$(ARDUINO_LIBS)Ethernet -I$(ARDUINO_LIBS)Firmata -I$(ARDUINO_LIBS)Wire \
		-I$(ARDUINO_LIBS)LiquidCrystal -I$(ARDUINO_LIBS)Matrix -I$(ARDUINO_LIBS)NewSoftSerial \
		-I$(ARDUINO_LIBS)Servo -I$(ARDUINO_LIBS)SoftwareSerial -I$(ARDUINO_LIBS)Sprite -I$(ARDUINO_LIBS)Stepper

#LIBUSB_SRC = ../libs/libusb-0.1.12  #change with your libusb sources folder
#LUA_SRC = ../libs/lua-5.1.4/src/    #change with your lua sources folder

OPTS       = -Wall -o3
CFLAGS     = -mmcu=avr5 -D__AVR_ATmega1280__ -x c++  $(OPTS) -I$(ARDUINO_SRC) $(ARDUINO_ILIBS)
# -mmcu=avr5
LDFLAGS    = -lstdc

CC = avr-gcc
LD = avr-gcc


all: $(OBJS) $(SALIDA)



linker: compile $(SALIDA)
	$(LD) $(OBJS) $(LDFLAGS) -o $(SALIDA)

compile: $(OBJS) gcc_avr_is_installed

upload: $(SALIDA)



SALIDA:
	echo ejejeje > blablabla



clean:
	rm -f $(OBJS) $(SALIDA)


gcc_avr_is_installed:
	VAR=`dpkg-query -l | grep gcc-avr | grep ii`
	if [ "$VAR" == "" ] ; then echo NO ESTA INSTALADO EL AVR-GCC ; exit -1; fi


%.o : %.cpp
	echo pepep
	$(CC) $(CFLAGS) -c $<

%.o : %.pde
	echo allala
	$(CC) $(CFLAGS) -c $<


