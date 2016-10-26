/* Author             									  Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Andrés Aguirre								         27/11/07    Original.
 *****************************************************************************/

#ifndef IOR_H
#define IOR_H

/** I N C L U D E S **********************************************************/
#include "system\typedefs.h"
#include "user\adminModule.h"
#include "user\loaderModule.h"

/** D E F I N I T I O N S ****************************************************/

#define IOR_MINOR_VERSION   0x01    //IOR Version 0.01
#define IOR_MAJOR_VERSION   0x00

#define IORPortA  LATB
#define IORPortBLow   LATC
#define IORPortBHigh  LATD
#define mInitIOR_A() TRISB &= 0x00;
#define mReleaseIOR_A() TRISB |= 0xFF;
#define mInitIOR_BLow() TRISC &= 0x00;
#define mReleaseIOR_BLow() TRISC |= 0xFF;
#define mInitIOR_BHigh TRISD &= 0x0F;
#define mReleaseIOR_BHigh() TRISD |= 0xF0;



/** S T R U C T U R E S ******************************************************/
typedef union IOR_DATA_PACKET
{
    byte _byte[USBGEN_EP_SIZE];  //For byte access
    word _word[USBGEN_EP_SIZE/2];//For word access(USBGEN_EP_SIZE msut be even)
    struct
    {
        enum
        { 
            READ_VERSION    = 0x00,
			MESS			= 0x05,
			SEND_IOR		= 0x04,
			INTE			= 0x06,
			TEST			= 0x07,
			RESET			= 0xFF //backward compatibility
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
} IOR_DATA_PACKET;

/** P U B L I C  P R O T O T Y P E S *****************************************/



#endif //USER_IOR_H
