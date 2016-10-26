/* Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Andres Aguirre       01/04/07    Original. 
 ********************************************************************/

#ifndef USER_TEMP_2_H
#define USER_TEMP_2_H

/** I N C L U D E S **********************************************************/
#include "system\typedefs.h"
#include "user\adminModule.h"
#include "user\userTemp.h"
#include "user\loaderModule.h"

/** D E F I N I T I O N S ****************************************************/

/* Temperature Mode */
#define TEMP_REAL_TIME  0x00
#define TEMP_LOGGING    0x01
#define MINOR_VERSION2   0x0D    //Demo Version 0.13
#define MAJOR_VERSION2   0x00

/** S T R U C T U R E S ******************************************************/
typedef union TEMP_DATA_PACKET2
{
    byte _byte[USBGEN_EP_SIZE];  //For byte access
    word _word[USBGEN_EP_SIZE/2];//For word access(USBGEN_EP_SIZE msut be even)
    struct
    {
        enum
        { 
            READ_VERSION    = 0x00,
			SET_TEMP_REAL   = 0x01,
            RD_TEMP         = 0x34, //backward compatibility
            SET_TEMP_LOGGING= 0x03,
            RD_TEMP_LOGGING = 0x04,
			MESS			= 0x05,
			RESET			= 0xFF
        }CMD;
        byte len;
    };
    struct
    {
        unsigned :8;
        byte ID;
    };
    struct
    {
        unsigned :8;
        byte led_num;
        byte led_status;
    };
    struct
    {
        unsigned :8;
        word word_data;
    };
} DATA_PACKET2;

/** P U B L I C  P R O T O T Y P E S *****************************************/
//void TempProcessIO(void);
void tempUserInit2(byte i);
void userTempInit2(byte handler);
void userTempReceived2(byte* recBuffPtr,byte len);
void userTempRelease2(byte i);
BOOL Switch3IsPressed2();
//void userReceive(byte* data,byte len);
#endif //USER_TEMP_2_H
