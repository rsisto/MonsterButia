/* Author             									  Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Andrés Aguirre, Rafael Fernandez, Carlos Grossy       20/05/07    Original.
 *****************************************************************************/

#ifndef MANCHESTER_H
#define MANCHESTER_H

/** I N C L U D E S **********************************************************/
#include "system\typedefs.h"
#include "user\adminModule.h"
#include "user\loaderModule.h"
#include "user\defines.h"


/** D E F I N I T I O N S ****************************************************/

#define MANCHESTER_MINOR_VERSION   0x0D    //Skeleton Version 0.13
#define MANCHESTER_MAJOR_VERSION   0x00
#define MANCHESTER_PORT LATD
#define MANCHESTER_BIT  0x10 // bit 5

#define MAINCLOCK .48000000 // 48 megahertz
#define TIMERLOAD .65500
#define TIMER0L_INIT = (.65535 -TIMERLOAD)%.256  
#define TIMER0H_INIT = (.65535 -TIMERLOAD)/.256 


/** S T R U C T U R E S ******************************************************/
typedef union MANCHESTER_DATA_PACKET
{
    byte _byte[USBGEN_EP_SIZE];  //For byte access
    word _word[USBGEN_EP_SIZE/2];//For word access(USBGEN_EP_SIZE msut be even)
    struct
    {
        enum
        { 
            READ_VERSION    = 0x00,
			MESS			= 0x05,
			SENDBYTE		= 0x10, //Manchester segun IEE 802.3
			SENDBYTE_T		= 0x11, //Manchester segun G.E Thomas 
			DEBUG			= 0x20, 
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
} MANCHESTER_DATA_PACKET;

typedef union MANCHESTER_DATA_TO_SEND
{
    BYTE mbyte[2];  //For byte access
    unsigned short m_data;
    struct
    {
        unsigned b0:1;
        unsigned b1:1;
        unsigned b2:1;
        unsigned b3:1;
        unsigned b4:1;
        unsigned b5:1;
        unsigned b6:1;
        unsigned b7:1;
        unsigned b8:1;
        unsigned b9:1;
        unsigned b10:1;
        unsigned b11:1;
        unsigned b12:1;
        unsigned b13:1;
        unsigned b14:1;
        unsigned b15:1;
    };
} MANCHESTER_DATA_TO_SEND;

/** P U B L I C  P R O T O T Y P E S *****************************************/

void sendCurrentBit();
void termineDeMandar();
void manchesterOut(byte valor);
void prendoTimer(void);
void apagoTimer(void);

#endif //USER_MANCHESTER_H
