#ifndef OF_CONSTANTS
#define OF_CONSTANTS

        #define B14400	14400
        #define B28800	28800


enum ofLogLevel{
	OF_LOG_VERBOSE,
	OF_LOG_NOTICE,
	OF_LOG_WARNING,
	OF_LOG_ERROR,
	OF_LOG_FATAL_ERROR,
	OF_LOG_SILENT	//this one is special and should always be last - set ofSetLogLevel to OF_SILENT to not recieve any messages
};

#define OF_DEFAULT_LOG_LEVEL  OF_LOG_WARNING;

// serial error codes
#define OF_SERIAL_NO_DATA 	-2
#define OF_SERIAL_ERROR		-1

#include <stdarg.h>
#include <iomanip>  //for setprecision
using namespace std;


#endif
