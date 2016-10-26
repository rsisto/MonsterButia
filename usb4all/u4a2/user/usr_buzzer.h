/* Author             									  Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Santiago Reyes      									21/07/09    Original.
 *****************************************************************************/

#ifndef USER_BUZZER_H
#define USER_BUZZER_H

/** I N C L U D E S **********************************************************/
#include "system\typedefs.h"
#include "user\adminModule.h"
#include "user\loaderModule.h"

/** D E F I N I T I O N S ****************************************************/

#define BUZZER_MINOR_VERSION   0x02    //Skeleton Version 0.13
#define BUZZER_MAJOR_VERSION   0x00
 
/*
#define mInitIOR_D() TRISD &= 0x00;
#define mReleaseIOR_D() TRISD |= 0xFF;
*/
/** S T R U C T U R E S ******************************************************/
typedef union BUZZER_DATA_PACKET
{
    byte _byte[USBGEN_EP_SIZE];  //For byte access
    word _word[USBGEN_EP_SIZE/2];//For word access(USBGEN_EP_SIZE msut be even)
    struct
    {
        enum
        { 
            READ_VERSION    = 0x00,
            PRENDER			= 0x01,
            APAGAR			= 0x02,
            BUZZER_CORTO    = 0x03,
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
        byte buzzer_num;
        byte buzzer_status;
    };
    struct
    {
        unsigned :8;
        word word_data;
    };
} BUZZER_DATA_PACKET;

/** P U B L I C  P R O T O T Y P E S *****************************************/



#endif //USER_BUZZER_H
