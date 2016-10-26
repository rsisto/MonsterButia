/* Author             									  Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Rafael Fernandez									    14/06/07    Original.
 *****************************************************************************/

#ifndef TEMP_ISR_H
#define TEMP_ISR_H

/** I N C L U D E S **********************************************************/
#include "system\typedefs.h"
#include "user\adminModule.h"
#include "user\loaderModule.h"
#include "usb4all\dynamicISR\dynamicISR.h"

/** D E F I N I T I O N S ****************************************************/

#define TESTISR_MINOR_VERSION   0x35    //testISR version 0.35
#define TESTISR_MAJOR_VERSION   0x00

/** S T R U C T U R E S ******************************************************/
typedef union TESTISR_DATA_PACKET
{
    byte _byte[USBGEN_EP_SIZE];  //For byte access
    word _word[USBGEN_EP_SIZE/2];//For word access(USBGEN_EP_SIZE msut be even)
    struct
    {
        enum
        { 
            READ_VERSION    = 0x00,
			MESS			= 0x05,
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
} TESTISR_DATA_PACKET;

/** P U B L I C  P R O T O T Y P E S *****************************************/



#endif //TEST_ISR_H
