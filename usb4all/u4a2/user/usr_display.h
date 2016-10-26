	/* Author             									  Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Santiago Reyes Gonzalez								         10/07/09    Original.
 * Andres Aguirre Dorelo										
 * Edgardo Vaz		 	
 *****************************************************************************/

#ifndef display_H
#define display_H
/* Interface type 8-bit or 4-bit
+
 * For 8-bit operation uncomment the #define BIT8
 */
/* #define BIT8 */

/* When in 4-bit interface define if the data is in the upper
 * or lower nibble.  For lower nibble, comment the #define UPPER
 */
/* #define UPPER */
 
/* DATA_PORT defines the port to which the LCD data lines are connected */
#define DATA_PORT      PORTD
#define TRIS_DATA_PORT TRISD
/*#define FALSE 			0x00*/
/*#define TRUE 			0xFF*/
/* CTRL_PORT defines the port where the control lines are connected.
 * These are just samples, change to match your application.
 */
#define RW_PIN   LATDbits.LATD6  	/* PORT for RW */ 
#define TRIS_RW  TRISDbits.TRISD6   /* TRIS for RW */ 
#define RS_PIN   LATDbits.LATD5   	/* PORT for RS */ 
#define TRIS_RS  TRISDbits.TRISD5   /* TRIS for RS */ 
#define E_PIN    LATDbits.LATD4   	/* PORT for E  */ 
#define TRIS_E   TRISDbits.TRISD4   /* TRIS for E  */

/* Display ON/OFF Control defines */
#define DON         0x0C  		/* Display on      */
#define DOFF        0x08  		/* Display off     */
#define CURSOR_ON   0b00001111  /* Cursor on       */
#define CURSOR_OFF  0b00001101  /* Cursor off      */
#define BLINK_ON    0b00001111  /* Cursor Blink    */
#define BLINK_OFF   0b00001110  /* Cursor No Blink */
#define LIMPIAR  	0x01 		/* Limpia el LCD*/
#define LINEA_1  	0x02 		/* Inicio del LCD linea 1*/
#define LINEA_2  	0xC0 		/* Inicio del LCD linea 2*/
#define COMANDO_1 	0x03 		/* Palabra cable inicial*/
#define COMANDO_2	0x02 		/* Segunda palabra de inicializacion borrado DDRAM*/
#define COMANDO_3	0x08 		/* Tercera palabra de inicializacion*/
#define COMANDO_4	0x06 		/* Cuarta palabra de inicializacion*/
#define FUNCION_INI	0x28 		/* Seteo de funcion*/
#define	MODO_ENT	0x06 		/* Modo de entrada*/
#define	CASA		0x80 		/* Modo de entrada*/


/* Cursor or Display Shift defines */
#define SHIFT_CUR_LEFT    0b00010011  /* Cursor shifts to the left   */
#define SHIFT_CUR_RIGHT   0b00010111  /* Cursor shifts to the right  */
#define SHIFT_DISP_LEFT   0b00011011  /* Display shifts to the left  */
#define SHIFT_DISP_RIGHT  0b00011111  /* Display shifts to the right */

/* Function Set defines */
#define FOUR_BIT   0b00101111  /* 4-bit Interface               */
#define EIGHT_BIT  0b00111111  /* 8-bit Interface               */
#define LINE_5X7   0b00110011  /* 5x7 characters, single line   */
#define LINE_5X10  0b00110111  /* 5x10 characters               */
#define LINES_5X7  0b00111011  /* 5x7 characters, multiple line */

#define PARAM_SCLASS auto
#define MEM_MODEL far  /* Change this to near for small memory model */
/** I N C L U D E S **********************************************************/
#include "system\typedefs.h"
#include "user\adminModule.h"
#include "user\loaderModule.h"
/*#include <stdio.h>*/
/** D E F I N I T I O N S ****************************************************/

#define DISPLAY_MINOR_VERSION   0x04    //Display Version 0.04
#define DISPLAY_MAJOR_VERSION   0x00

#define displayPort	LATD

#define mInitDisplay()     TRISD &= 0x00
#define mReleaseDisplay()  TRISD |= 0xFF



/** S T R U C T U R E S ******************************************************/
typedef union DISPLAY_DATA_PACKET
{
    byte _byte[USBGEN_EP_SIZE];  //For byte access
    word _word[USBGEN_EP_SIZE/2];//For word access(USBGEN_EP_SIZE msut be even)
    struct
    {
        enum
        { 
            READ_VERSION    = 0x00,
			ESCRIBIR		= 0x01,
			PRUEBA			= 0x02,
			BORRAR			= 0x03,
			INICIAR			= 0x04,
			MESS			= 0x05,
			INTE			= 0x06,
			TEST			= 0x07,
			PRENDER_BKL 		= 0x08,
			APAGAR_BKL		= 0x09,
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
} DISPLAY_DATA_PACKET;

void comandXLCD (unsigned char a);
void DelayFor18TCY( void );
void DelayPORXLCD( void );
void DelayXLCD( void );

/** P U B L I C  P R O T O T Y P E S *****************************************/



#endif //USER_DISPLAY_H
