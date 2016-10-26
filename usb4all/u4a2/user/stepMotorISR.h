/* Author             									  Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Andres Aguirre									    14/06/07    Original.
 *****************************************************************************/

#ifndef MOTOR_ISR_H
#define MOTOR_ISR_H

/** I N C L U D E S **********************************************************/
#include "system\typedefs.h"
#include "user\adminModule.h"
#include "user\loaderModule.h"
#include "usb4all\dynamicISR\dynamicISR.h"

/** D E F I N I T I O N S ****************************************************/

#define MOTOR_ISR_MINOR_VERSION   0x47    //MOTOR_ISR version 0.36
#define MOTOR_ISR_MAJOR_VERSION   0x01

#define STOP  0x00
#define LEFT  0x01
#define RIGHT 0x02
#define MotorPort  LATD
#define mInitMotor() TRISD &= 0x0F;
#define mReleaseMotor() TRISD |= 0xF0;




/** S T R U C T U R E S ******************************************************/
typedef union MOTORISR_DATA_PACKET
{
    byte _byte[USBGEN_EP_SIZE];  //For byte access
    word _word[USBGEN_EP_SIZE/2];//For word access(USBGEN_EP_SIZE msut be even)
    struct
    {
        enum
        { 
            READ_VERSION    = 0x00,
			MOTOR_ON		= 0x01,
			SPEED			= 0x02, 
			STEPS			= 0x03,   
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
} MOTORISR_DATA_PACKET;

/** P U B L I C  P R O T O T Y P E S *****************************************/



#endif //TEST_ISR_H
