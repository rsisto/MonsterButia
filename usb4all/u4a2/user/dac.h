/* Author             									  Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Andrés Aguirre, Rafael Fernandez				         03/11/07    Original.
 *****************************************************************************/

#ifndef DAC_H
#define DAC_H

/** I N C L U D E S **********************************************************/
#include "system\typedefs.h"
#include "user\adminModule.h"
#include "user\loaderModule.h"

/** D E F I N I T I O N S ****************************************************/

#define DAC_MINOR_VERSION   0x01    //DAC Version 0.01
#define DAC_MAJOR_VERSION   0x00

#define DACPortA  LATB
#define DACPortBLow   LATC
#define DACPortBHigh  LATD
#define mInitDAC_A() TRISB &= 0x00;
#define mReleaseDAC_A() TRISB |= 0xFF;
#define mInitDAC_BLow() TRISC &= 0x00;
#define mReleaseDAC_BLow() TRISC |= 0xFF;
#define mInitDAC_BHigh TRISD &= 0x0F;
#define mReleaseDAC_BHigh() TRISD |= 0xF0;



/** S T R U C T U R E S ******************************************************/
typedef union DAC_DATA_PACKET
{
    byte _byte[USBGEN_EP_SIZE];  //For byte access
    word _word[USBGEN_EP_SIZE/2];//For word access(USBGEN_EP_SIZE msut be even)
    struct
    {
        enum
        { 
            READ_VERSION    = 0x00,
			MESS			= 0x05,
			SEND_DAC		= 0x04,
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
} DAC_DATA_PACKET;

/** P U B L I C  P R O T O T Y P E S *****************************************/



#endif //USER_DAC_H
