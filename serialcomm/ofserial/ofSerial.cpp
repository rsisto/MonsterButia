#include "ofSerial.h"

//----------------------------------------------------------------
ofSerial::ofSerial(){

	bVerbose = false;
}

//----------------------------------------------------------------
ofSerial::~ofSerial(){

	close();

	bVerbose = false;
	bInited = false;
}


//----------------------------------------------------------------
void ofSerial::enumerateDevices(){

		//We will find serial devices by listing the directory

		DIR *dir;
		struct dirent *entry;
		dir = opendir("/dev");
		string str			= "";
		string device		= "";
		int deviceCount		= 0;

		if (dir == NULL){
			ofLog(OF_LOG_ERROR,"ofSerial: error listing devices in /dev");
		} else {
			ofLog(OF_LOG_NOTICE,"ofSerial: listing devices");
			while ((entry = readdir(dir)) != NULL){
				str = (char *)entry->d_name;
				if( str.substr(0,3) == "tty" || str.substr(0,3) == "rfc" ){
					ofLog(OF_LOG_NOTICE,"device %i - %s", deviceCount, str.c_str());
					deviceCount++;
				}
			}
		}
}

//----------------------------------------------------------------
void ofSerial::close(){

    	if (bInited){
    		tcsetattr(fd,TCSANOW,&oldoptions);
    		::close(fd);
    	}
    	// [CHECK] -- anything else need to be reset?

}

//----------------------------------------------------------------
bool ofSerial::setup(){
	return setup(0,9600);		// the first one, at 9600 is a good choice...
}

//----------------------------------------------------------------
bool ofSerial::setup(int deviceNumber, int baud){

	int deviceCount = 0;

	string str			= "";
	string device		= "";
	bool deviceFound	= false;


		//We will find serial devices by listing the directory

		DIR *dir;
		struct dirent *entry;
		dir = opendir("/dev");

		if (dir == NULL){
			ofLog(OF_LOG_ERROR,"ofSerial: error listing devices in /dev");
		}

		while ((entry = readdir(dir)) != NULL){
			str = (char *)entry->d_name;
			if( str.substr(0,3) == "cu." ){
				if(deviceCount == deviceNumber){
					device = "/dev/"+str;
					deviceFound = true;
					ofLog(OF_LOG_NOTICE,"ofSerial device %i - /dev/%s  <--selected", deviceCount, str.c_str());
				}else ofLog(OF_LOG_NOTICE,"ofSerial device %i - /dev/%s", deviceCount, str.c_str());
				deviceCount++;
			}
		}

        if(deviceFound){
            return setup(device, baud);
        }else{
            ofLog(OF_LOG_ERROR,"ofSerial: could not find device %i - only %i devices found", deviceNumber, deviceCount);
            return false;
        }

}

//----------------------------------------------------------------
bool ofSerial::setup(string portName, int baud){

	bInited = false;

	    ofLog(OF_LOG_NOTICE,"ofSerialInit: opening port %s @ %d bps", portName.c_str(), baud);
		fd = open(portName.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
		if(fd == -1){
			ofLog(OF_LOG_ERROR,"ofSerial: unable to open port");
			return false;
		}

		struct termios options;
		tcgetattr(fd,&oldoptions);
		options = oldoptions;
		switch(baud){
		   case 300: 	cfsetispeed(&options,B300);
						cfsetospeed(&options,B300);
						break;
		   case 1200: 	cfsetispeed(&options,B1200);
						cfsetospeed(&options,B1200);
						break;
		   case 2400: 	cfsetispeed(&options,B2400);
						cfsetospeed(&options,B2400);
						break;
		   case 4800: 	cfsetispeed(&options,B4800);
						cfsetospeed(&options,B4800);
						break;
		   case 9600: 	cfsetispeed(&options,B9600);
						cfsetospeed(&options,B9600);
						break;
		   case 14400: 	cfsetispeed(&options,B14400);
						cfsetospeed(&options,B14400);
						break;
		   case 19200: 	cfsetispeed(&options,B19200);
						cfsetospeed(&options,B19200);
						break;
		   case 28800: 	cfsetispeed(&options,B28800);
						cfsetospeed(&options,B28800);
						break;
		   case 38400: 	cfsetispeed(&options,B38400);
						cfsetospeed(&options,B38400);
						break;
		   case 57600:  cfsetispeed(&options,B57600);
						cfsetospeed(&options,B57600);
						break;
		   case 115200: cfsetispeed(&options,B115200);
						cfsetospeed(&options,B115200);
						break;

			default:	cfsetispeed(&options,B9600);
						cfsetospeed(&options,B9600);
						ofLog(OF_LOG_ERROR,"ofSerialInit: cannot set %i baud setting baud to 9600\n", baud);
						break;
		}

		options.c_cflag |= (CLOCAL | CREAD);
		options.c_cflag &= ~PARENB;
		options.c_cflag &= ~CSTOPB;
		options.c_cflag &= ~CSIZE;
		options.c_cflag |= CS8;
		tcsetattr(fd,TCSANOW,&options);

		bInited = true;
		ofLog(OF_LOG_NOTICE,"sucess in opening serial connection");

	    return true;

}


//----------------------------------------------------------------
int ofSerial::writeBytes(unsigned char * buffer, int length){

	if (!bInited){
		ofLog(OF_LOG_ERROR,"ofSerial: serial not inited");
		return OF_SERIAL_ERROR;
	}

	    int numWritten = write(fd, buffer, length);
		if(numWritten <= 0){
			ofLog(OF_LOG_ERROR,"ofSerial: Can't write to com port");
			return OF_SERIAL_ERROR;
		}

		ofLog(OF_LOG_VERBOSE,"ofSerial: numWritten %i", numWritten);

	    return numWritten;

}

//----------------------------------------------------------------
int ofSerial::readBytes(unsigned char * buffer, int length){

	if (!bInited){
		ofLog(OF_LOG_ERROR,"ofSerial: serial not inited");
		return OF_SERIAL_ERROR;
	}

		int nRead = read(fd, buffer, length);
		if(nRead < 0){
			ofLog(OF_LOG_ERROR,"ofSerial: trouble reading from port");
			return OF_SERIAL_ERROR;
		}
		return nRead;
}



//----------------------------------------------------------------
bool ofSerial::writeByte(unsigned char singleByte){

	if (!bInited){
		ofLog(OF_LOG_ERROR,"ofSerial: serial not inited");
		return OF_SERIAL_ERROR;
	}

	unsigned char tmpByte[1];
	tmpByte[0] = singleByte;

	    int numWritten = 0;
	    numWritten = write(fd, tmpByte, 1);
		if(numWritten <= 0 ){
			 ofLog(OF_LOG_ERROR,"ofSerial: Can't write to com port");
			 return OF_SERIAL_ERROR;
		}
		ofLog(OF_LOG_VERBOSE,"ofSerial: written byte");


		return (numWritten > 0 ? true : false);
}

//----------------------------------------------------------------
int ofSerial::readByte(){

	if (!bInited){
		ofLog(OF_LOG_ERROR,"ofSerial: serial not inited");
		return OF_SERIAL_ERROR;
	}

	unsigned char tmpByte[1];
	memset(tmpByte, 0, 1);

		int nRead = read(fd, tmpByte, 1);
		if(nRead < 0){
			ofLog(OF_LOG_ERROR,"ofSerial: trouble reading from port");
            return OF_SERIAL_ERROR;
		}
		if(nRead == 0)
			return OF_SERIAL_NO_DATA;

	return (int)(tmpByte[0]);
}


//----------------------------------------------------------------
void ofSerial::flush(bool flushIn, bool flushOut){

	if (!bInited){
		ofLog(OF_LOG_ERROR,"ofSerial: serial not inited");
		return;
	}

	int flushType = 0;

		if( flushIn && flushOut) flushType = TCIOFLUSH;
		else if(flushIn) flushType = TCIFLUSH;
		else if(flushOut) flushType = TCOFLUSH;
		else return;

		tcflush(fd, flushType);
}

//-------------------------------------------------------------
int ofSerial::available(){

	if (!bInited){
		ofLog(OF_LOG_ERROR,"ofSerial: serial not inited");
		return OF_SERIAL_ERROR;
	}

	int numBytes = 0;

		ioctl(fd,FIONREAD,&numBytes);

	return numBytes;
}

void ofLog(int logLevel, string message){
	if(logLevel >= 1){
		if(logLevel == OF_LOG_VERBOSE){
			printf("OF_VERBOSE: ");
		}
		else if(logLevel == OF_LOG_NOTICE){
			printf("OF_NOTICE: ");
		}
		else if(logLevel == OF_LOG_WARNING){
			printf("OF_WARNING: ");
		}
		else if(logLevel == OF_LOG_ERROR){
			printf("OF_ERROR: ");
		}
		else if(logLevel == OF_LOG_FATAL_ERROR){
			printf("OF_FATAL_ERROR: ");
		}
		printf("%s\n",message.c_str());
	}
}

void ofLog(int logLevel, const char* format, ...){
	//thanks stefan!
	//http://www.ozzu.com/cpp-tutorials/tutorial-writing-custom-printf-wrapper-function-t89166.html

	if(logLevel >= 1){
		va_list args;
		va_start( args, format );
		if(logLevel == OF_LOG_VERBOSE){
			printf("OF_VERBOSE: ");
		}
		else if(logLevel == OF_LOG_NOTICE){
			printf("OF_NOTICE: ");
		}
		else if(logLevel == OF_LOG_WARNING){
			printf("OF_WARNING: ");
		}
		else if(logLevel == OF_LOG_ERROR){
			printf("OF_ERROR: ");
		}
		else if(logLevel == OF_LOG_FATAL_ERROR){
			printf("OF_FATAL_ERROR: ");
		}
		vprintf( format, args );
		printf("\n");
		va_end( args );
	}
}
