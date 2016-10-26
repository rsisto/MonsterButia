#ifndef OF_SERIAL_H
#define OF_SERIAL_H

#include "ofConstants.h"

#include <termios.h>
#include <sys/ioctl.h>
#include <getopt.h>
#include <dirent.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>


//----------------------------------------------------------------------
class ofSerial{

	public:
			ofSerial();
			virtual ~ofSerial();

			void 			enumerateDevices();

			void 			close();
			bool			setup();	// use default port, baud (0,9600)
			bool			setup(string portName, int baudrate);
			bool			setup(int deviceNumber, int baudrate);


			int 			readBytes(unsigned char * buffer, int length);
			int 			writeBytes(unsigned char * buffer, int length);
			bool			writeByte(unsigned char singleByte);
			int             readByte();  // returns -1 on no read or error...
			void			flush(bool flushIn = true, bool flushOut = true);
			int				available();

			bool 			bVerbose;
			void 			setVerbose(bool bLoudmouth) { bVerbose = bLoudmouth; };
			void            ofLog(int logLevel, string message);
            void            ofLog(int logLevel, const char* format, ...);



	protected:

			bool 	bInited;


				int 		fd;			// the handle to the serial port mac
				struct 	termios oldoptions;


};

// notes:
// ----------------------------
// when calling setup("....") you need to pass in
// the name of the com port the device is attached to
// for example, on a mac, it might look like:
//
// 		setup("/dev/tty.usbserial-3B1", 9600)
//
// and on a pc, it might look like:
//
// 		setup("COM4", 9600)
///**/

#endif
