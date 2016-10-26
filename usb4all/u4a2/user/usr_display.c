/* Author             					  Date        Comment			Versión
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Santiago Reyes Gonzalez				10/07/09   Original.			0.02	
 * Andrés Aguirre Dorelo				21/12/09   Cambios al open y al close	0.04
 *********************************************************************************************************/

/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include <usart.h>
#include <delays.h>
#include <stdio.h>
#include "system\typedefs.h"
#include "system\usb\usb.h"
#include "usr_display.h"
#include "io_cfg.h"              // I/O pin mapping
#include "user\handlerManager.h"
#include "user\dynamicPolling.h"                              

#define FALSE 0
#define TRUE  1

/** V A R I A B L E S ********************************************************/
#pragma udata 

byte  usrDisplayHandler;	 // Handler number asigned to the module
byte* sendBufferUsrDisplay;  	 // buffer to send data
unsigned char display_init = FALSE;

/** P R I V A T E  P R O T O T Y P E S ************* *************************/
void UserDisplayProcessIO(void);
void UserDisplayInit(byte i);
void UserDisplayReceived(byte*, byte);
void UserDisplayRelease(byte i);
void UserDisplayConfigure(void);
 
// Table used by te framework to get a fixed reference point to the user module functions defined by the framework 
/** USER MODULE REFERENCE*****************************************************/
#pragma romdata user
uTab userDisplayModuleTable = {&UserDisplayInit,&UserDisplayRelease,&UserDisplayConfigure,"display"}; //modName must be less or equal 8 characters
#pragma code

/** D E C L A R A T I O N S **************************************************/
#pragma code module

void DelayFor18TCY( void ){
Nop(); Nop(); Nop(); 
Nop(); Nop(); Nop(); 
Nop(); Nop(); Nop(); 
Nop(); Nop(); Nop();
Nop(); Nop(); Nop(); 
Nop(); Nop(); Nop();
}

void DelayPORXLCD( void ){
	Delay10KTCYx(30); //Delay of 15ms
	return;
}

void DelayXLCD( void ){
	Delay10KTCYx(10); //Delay of 5ms
	return;
}

/********************************************************************
*       Function Name:  BusyXLCD                                    *
*       Return Value:   char: busy status of LCD controller         *
*       Parameters:     void                                        *
*       Description:    This routine reads the busy status of the   *
*                       Hitachi HD44780 LCD controller.             *
********************************************************************/
unsigned char BusyXLCD(void)
{
        RW_PIN = 1;                     // Set the control bits for read
        RS_PIN = 0;
        DelayFor18TCY();
        E_PIN = 1;                      // Clock in the command
        DelayFor18TCY();
#ifdef BIT8                             // 8-bit interface
        if(DATA_PORT&0x80)                      // Read bit 7 (busy bit)
        {                               // If high
                E_PIN = 0;              // Reset clock line
                RW_PIN = 0;             // Reset control line
                return 1;               // Return TRUE
        }
        else                            // Bit 7 low
        {
                E_PIN = 0;              // Reset clock line
                RW_PIN = 0;             // Reset control line
                return 0;               // Return FALSE
        }
#else                                   // 4-bit interface
#ifdef UPPER                            // Upper nibble interface
        if(DATA_PORT&0x80)
#else                                   // Lower nibble interface
        if(DATA_PORT&0x08)
#endif
        {
                E_PIN = 0;              // Reset clock line
                DelayFor18TCY();
                E_PIN = 1;              // Clock out other nibble
                DelayFor18TCY();
                E_PIN = 0;
                RW_PIN = 0;             // Reset control line
                return 1;               // Return TRUE
        }
        else                            // Busy bit is low
        {
                E_PIN = 0;              // Reset clock line
                DelayFor18TCY();
                E_PIN = 1;              // Clock out other nibble
                DelayFor18TCY();
                E_PIN = 0;
                RW_PIN = 0;             // Reset control line
                return 0;               // Return FALSE
        }
#endif
}


/********************************************************************
*       Function Name:  WriteCmdXLCD                                *
*       Return Value:   void                                        *
*       Parameters:     cmd: command to send to LCD                 *
*       Description:    This routine writes a command to the Hitachi*
*                       HD44780 LCD controller. The user must check *
*                       to see if the LCD controller is busy before *
*                       calling this routine.                       *
********************************************************************/
void WriteCmdXLCD(unsigned char cmd)
{
#ifdef BIT8                             // 8-bit interface
        TRIS_DATA_PORT = 0;             // Data port output
        DATA_PORT = cmd;                // Write command to data port
        RW_PIN = 0;                     // Set the control signals
        RS_PIN = 0;                     // for sending a command
        DelayFor18TCY();
        E_PIN = 1;                      // Clock the command in
        DelayFor18TCY();
        E_PIN = 0;
        DelayFor18TCY();
        TRIS_DATA_PORT = 0xff;          // Data port input
#else                                   // 4-bit interface
#ifdef UPPER                            // Upper nibble interface
        TRIS_DATA_PORT &= 0x0f;
        DATA_PORT &= 0x0f;
        DATA_PORT |= cmd&0xf0;
#else                                   // Lower nibble interface
        TRIS_DATA_PORT &= 0xf0;
        DATA_PORT &= 0xf0;
        DATA_PORT |= (cmd>>4)&0x0f;
#endif
        RW_PIN = 0;                     // Set control signals for command
        RS_PIN = 0;
        DelayFor18TCY();
        E_PIN = 1;                      // Clock command in
        DelayFor18TCY();
        E_PIN = 0;
#ifdef UPPER                            // Upper nibble interface
        DATA_PORT &= 0x0f;
        DATA_PORT |= (cmd<<4)&0xf0;
#else                                   // Lower nibble interface
        DATA_PORT &= 0xf0;
        DATA_PORT |= cmd&0x0f;
#endif
        DelayFor18TCY();
        E_PIN = 1;                      // Clock command in
        DelayFor18TCY();
        E_PIN = 0;
#ifdef UPPER                            // Make data nibble input
        TRIS_DATA_PORT |= 0xf0;
#else
        TRIS_DATA_PORT |= 0x0f;
#endif
#endif
        return;
}

/********************************************************************
*       Function Name:  SetDDRamAddr                                *
*       Return Value:   void                                        *
*       Parameters:     CGaddr: display data address                *
*       Description:    This routine sets the display data address  *
*                       of the Hitachi HD44780 LCD controller. The  *
*                       user must check to see if the LCD controller*
*                       is busy before calling this routine.        *
********************************************************************/
void SetDDRamAddr(unsigned char DDaddr)
{
#ifdef BIT8                                     // 8-bit interface
        TRIS_DATA_PORT = 0;                     // Make port output
        DATA_PORT = DDaddr | 0b10000000;        // Write cmd and address to port
        RW_PIN = 0;                             // Set the control bits
        RS_PIN = 0;
        DelayFor18TCY();
        E_PIN = 1;                              // Clock the cmd and address in
        DelayFor18TCY();
        E_PIN = 0;
        DelayFor18TCY();
        TRIS_DATA_PORT = 0xff;                  // Make port input
#else                                           // 4-bit interface
#ifdef UPPER                                    // Upper nibble  interface
        TRIS_DATA_PORT &= 0x0f;                 // Make port output
        DATA_PORT &= 0x0f;                      // and write upper nibble
        DATA_PORT |= ((DDaddr | 0b10000000) & 0xf0);
#else                                           // Lower nibble interface
        TRIS_DATA_PORT &= 0xf0;                 // Make port output
        DATA_PORT &= 0xf0;                      // and write upper nibble
        DATA_PORT |= (((DDaddr | 0b10000000)>>4) & 0x0f);
#endif
        RW_PIN = 0;                             // Set control bits
        RS_PIN = 0;
        DelayFor18TCY();
        E_PIN = 1;                              // Clock the cmd and address in
        DelayFor18TCY();
        E_PIN = 0;
#ifdef UPPER                                    // Upper nibble interface
        DATA_PORT &= 0x0f;                      // Write lower nibble
        DATA_PORT |= ((DDaddr<<4)&0xf0);
#else                                           // Lower nibble interface
        DATA_PORT &= 0xf0;                      // Write lower nibble
        DATA_PORT |= (DDaddr&0x0f);
#endif
        DelayFor18TCY();
        E_PIN = 1;                              // Clock the cmd and address in
        DelayFor18TCY();
        E_PIN = 0;
#ifdef UPPER                                    // Upper nibble interface
        TRIS_DATA_PORT |= 0xf0;                 // Make port input
#else                                           // Lower nibble interface
        TRIS_DATA_PORT |= 0x0f;                 // Make port input
#endif
#endif
        return;
}

/********************************************************************
*       Function Name:  OpenXLCD                                    *
*       Return Value:   void                                        *
*       Parameters:     lcdtype: sets the type of LCD (lines)       *
*       Description:    This routine configures the LCD. Based on   *
*                       the Hitachi HD44780 LCD controller. The     *
*                       routine will configure the I/O pins of the  *
*                       microcontroller, setup the LCD for 4- or    *
*                       8-bit mode and clear the display. The user  *
*                       must provide three delay routines:          *
*                       DelayFor18TCY() provides a 18 Tcy delay     *
*                       DelayPORXLCD() provides at least 15ms delay *
*                       DelayXLCD() provides at least 5ms delay     *
********************************************************************/
void OpenXLCD(unsigned char lcdtype)
{
        // The data bits must be either a 8-bit port or the upper or
        // lower 4-bits of a port. These pins are made into inputs
#ifdef BIT8                             // 8-bit mode, use whole port
        DATA_PORT = 0;
        TRIS_DATA_PORT = 0xff;
#else                                   // 4-bit mode
#ifdef UPPER                            // Upper 4-bits of the port
        DATA_PORT &= 0x0f;
        TRIS_DATA_PORT |= 0xf0;
#else                                   // Lower 4-bits of the port
        DATA_PORT &= 0xf0;
        TRIS_DATA_PORT |= 0x0f;
#endif
#endif
        TRIS_RW = 0;                    // All control signals made outputs
        TRIS_RS = 0;
        TRIS_E = 0;
        RW_PIN = 0;                     // R/W pin made low
        RS_PIN = 0;                     // Register select pin made low
        E_PIN = 0;                      // Clock pin made low


        // Delay for 15ms to allow for LCD Power on reset
        DelayPORXLCD();
        
        // Setup interface to LCD
#ifdef BIT8                             // 8-bit mode interface
        TRIS_DATA_PORT = 0;             // Data port output
        DATA_PORT = 0b00110000;         // Function set cmd(8-bit interface)
#else                                   // 4-bit mode interface
#ifdef UPPER                            // Upper nibble interface
        TRIS_DATA_PORT &= 0x0f;
        DATA_PORT &= 0x0f;
        DATA_PORT |= 0b00100000;        // Function set cmd(4-bit interface)
#else                                   // Lower nibble interface
        TRIS_DATA_PORT &= 0xf0;
        DATA_PORT &= 0xf0;
        DATA_PORT |= 0b00000010;        // Function set cmd(4-bit interface)
#endif
#endif
        E_PIN = 1;                      // Clock the cmd in
        DelayFor18TCY();
        E_PIN = 0;
        
        // Delay for at least 4.1ms
        DelayXLCD();

        // Setup interface to LCD
#ifdef BIT8                             // 8-bit interface
        DATA_PORT = 0b00110000;         // Function set cmd(8-bit interface)
#else                                   // 4-bit interface
#ifdef UPPER                            // Upper nibble interface
        DATA_PORT &= 0x0f;              // Function set cmd(4-bit interface)
        DATA_PORT |= 0b00100000;
#else                                   // Lower nibble interface
        DATA_PORT &= 0xf0;              // Function set cmd(4-bit interface)
        DATA_PORT |= 0b00000010;
#endif
#endif
        E_PIN = 1;                      // Clock the cmd in
        DelayFor18TCY();
        E_PIN = 0;

        // Delay for at least 100us
        DelayXLCD();

        // Setup interface to LCD
#ifdef BIT8                             // 8-bit interface
        DATA_PORT = 0b00110000;         // Function set cmd(8-bit interface)
#else                                   // 4-bit interface
#ifdef UPPER                            // Upper nibble interface
        DATA_PORT &= 0x0f;              // Function set cmd(4-bit interface)
        DATA_PORT |= 0b00100000;
#else                                   // Lower nibble interface
        DATA_PORT &= 0xf0;              // Function set cmd(4-bit interface)
        DATA_PORT |= 0b00000010;
#endif
#endif
        E_PIN = 1;                      // Clock cmd in
        DelayFor18TCY();
        E_PIN = 0;

#ifdef BIT8                             // 8-bit interface
        TRIS_DATA_PORT = 0xff;          // Make data port input
#else                                   // 4-bit interface
#ifdef UPPER                            // Upper nibble interface
        TRIS_DATA_PORT |= 0xf0;         // Make data nibble input
#else                                   // Lower nibble interface
        TRIS_DATA_PORT |= 0x0f;         // Make data nibble input
#endif
#endif

        // Set data interface width, # lines, font
        while(BusyXLCD());              // Wait if LCD busy
        WriteCmdXLCD(lcdtype);          // Function set cmd

        // Turn the display on then off
        while(BusyXLCD());              // Wait if LCD busy
        WriteCmdXLCD(DOFF&CURSOR_OFF&BLINK_OFF);        // Display OFF/Blink OFF
        while(BusyXLCD());              // Wait if LCD busy
        WriteCmdXLCD(DON&CURSOR_ON&BLINK_ON);           // Display ON/Blink ON

        // Clear display
        while(BusyXLCD());              // Wait if LCD busy
        WriteCmdXLCD(0x01);             // Clear display

        // Set entry mode inc, no shift
        while(BusyXLCD());              // Wait if LCD busy
        WriteCmdXLCD(SHIFT_CUR_LEFT);   // Entry Mode

        // Set DD Ram address to 0
        while(BusyXLCD());              // Wait if LCD busy
        SetDDRamAddr(0);                // Set Display data ram address to 0

        return;
}


/********************************************************************
*       Function Name:  WriteDataXLCD                               *
*       Return Value:   void                                        *
*       Parameters:     data: data byte to be written to LCD        *
*       Description:    This routine writes a data byte to the      *
*                       Hitachi HD44780 LCD controller. The user    *
*                       must check to see if the LCD controller is  *
*                       busy before calling this routine. The data  *
*                       is written to the character generator RAM or*
*                       the display data RAM depending on what the  *
*                       previous SetxxRamAddr routine was called.   *
********************************************************************/
void WriteDataXLCD(char data)
{
#ifdef BIT8                             // 8-bit interface
        TRIS_DATA_PORT = 0;             // Make port output
        DATA_PORT = data;               // Write data to port
        RS_PIN = 1;                     // Set control bits
        RW_PIN = 0;
        DelayFor18TCY();
        E_PIN = 1;                      // Clock data into LCD
        DelayFor18TCY();
        E_PIN = 0;
        RS_PIN = 0;                     // Reset control bits
        TRIS_DATA_PORT = 0xff;          // Make port input
#else                                   // 4-bit interface
#ifdef UPPER                            // Upper nibble interface
        TRIS_DATA_PORT &= 0x0f;
        DATA_PORT &= 0x0f;
        DATA_PORT |= data&0xf0;
#else                                   // Lower nibble interface
        TRIS_DATA_PORT &= 0xf0;
        DATA_PORT &= 0xf0;
        DATA_PORT |= ((data>>4)&0x0f);
#endif
        RS_PIN = 1;                     // Set control bits
        RW_PIN = 0;
        DelayFor18TCY();
        E_PIN = 1;                      // Clock nibble into LCD
        DelayFor18TCY();
        E_PIN = 0;
#ifdef UPPER                            // Upper nibble interface
        DATA_PORT &= 0x0f;
        DATA_PORT |= ((data<<4)&0xf0);
#else                                   // Lower nibble interface
        DATA_PORT &= 0xf0;
        DATA_PORT |= (data&0x0f);
#endif
        DelayFor18TCY();
        E_PIN = 1;                      // Clock nibble into LCD
        DelayFor18TCY();
        E_PIN = 0;
#ifdef UPPER                            // Upper nibble interface
        TRIS_DATA_PORT |= 0xf0;
#else                                   // Lower nibble interface
        TRIS_DATA_PORT |= 0x0f;
#endif
#endif
        return;
}

/********************************************************************
*       Function Name:  putsXLCD
*       Return Value:   void
*       Parameters:     buffer: pointer to string
*       Description:    This routine writes a string of bytes to the
*                       Hitachi HD44780 LCD controller. The user
*                       must check to see if the LCD controller is
*                       busy before calling this routine. The data
*                       is written to the character generator RAM or
*                       the display data RAM depending on what the
*                       previous SetxxRamAddr routine was called.
********************************************************************/

void putsXLCD(char *buffer)
{
        while(*buffer)                  // Write data to LCD up to null
        {
                while(BusyXLCD());      // Wait while LCD is busy
                WriteDataXLCD(*buffer); // Write character to LCD
                buffer++;               // Increment buffer
        }
        return;
}


/******************************************************************************
 * Function:        commandXLCD(unsigned char a)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Esta funcion manda comandos al display y comprueba si esta  ocupado.
 *						
 *
 * Note:            None
 *****************************************************************************/

void commandXLCD (unsigned char a) { // Envia comando al LCD
	while(BusyXLCD()); 
	WriteCmdXLCD(a);
}


/******************************************************************************
 * Function:        displayInit(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is initialices the resources that the user module needs to work, it is called by the framework 
 *					when the module is opened	
 *
 * Note:            None
 *****************************************************************************/

void displayInit(void){
	DelayPORXLCD();			/* Espero 15ms @20MHz*/
	commandXLCD(COMANDO_1);		/* Primera palabara de inicializacion*/
	Delay10KTCYx(8); 		/* Espero 4ms @20MHz*/
	Delay1KTCYx(2); 		/* Espero 100us @20MHz*/
	commandXLCD(COMANDO_1);		/* Primera palabara de inicializacion*/
	Delay1KTCYx(2); 		/* Espero 100us @20MHz*/
	commandXLCD(COMANDO_1);		/* Primera palabara de inicializacion*/
	commandXLCD(COMANDO_2);		/* Segunda palabara de inicializacion*/
	commandXLCD(COMANDO_2);		/* Segunda palabara de inicializacion*/
	commandXLCD(FUNCION_INI);	/* Configuracion de funciones*/
	commandXLCD(FUNCION_INI);	/* Configuracion de funciones*/
	commandXLCD(COMANDO_3);		/* Tercera palabara de inicializacion*/
	commandXLCD(COMANDO_3);		/* Tercera palabara de inicializacion*/
	commandXLCD(LIMPIAR);		/* Display On, Cursor On, Cursor Blink*/
	commandXLCD(LIMPIAR);		/* Display On, Cursor On, Cursor Blink*/
	commandXLCD(MODO_ENT);		/* Modo de entrada*/
	commandXLCD(MODO_ENT);		/* Modo de entrada*/
	commandXLCD(COMANDO_4);		/* Cuarta palabara de inicializacion*/
	commandXLCD(COMANDO_4);		/* Cuarta palabara de inicializacion*/
	commandXLCD(DON);		/* Limpio el display*/
	commandXLCD(DON);		/* Limpio el display*/
	commandXLCD(CASA);		/* Voy al inicio del display*/
	commandXLCD(CASA);		/* Voy al inicio del display*/
}//end UserDisplayInit
/******************************************************************************
 * Function:        UserDisplayInit(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is initialices the resources that the user module needs to work, it is called by the framework 
 *					when the module is opened	
 *
 * Note:            None
 *****************************************************************************/

void UserDisplayInit(byte i){
	BOOL res;
	char string_1[17]; 	/* caracteres a mostrar en el display en la linea 1*/
	char string_2[17]; 	/* caracteres a mostrar en el display en la linea 2	*/
	usrDisplayHandler = i;
	// add my receive function to the handler module, to be called automatically when the pc sends data to the user module
	setHandlerReceiveFunction(usrDisplayHandler,&UserDisplayReceived);
	// add my receive pooling function to the dynamic pooling module, to be called periodically 
	//res = addPollingFunction(&UserDisplayProcessIO);
	// initialize the send buffer, used to send data to the PC
	sendBufferUsrDisplay = getSharedBuffer(usrDisplayHandler);
	//TODO return res value 
	if (display_init == FALSE){ 
		display_init = TRUE;
		mInitDisplay(); 			/* Inicio Display en TRISD */
		mInitBKL();				/*Incio el puerto del backlight*/
	}else{
		commandXLCD(LIMPIAR);
		commandXLCD(LINEA_1);
		string_1[0]='I'; string_1[1]='M'; string_1[2]='M'; string_1[3]=' '; string_1[4]= 'E'; string_1[5]='l';
		string_1[6]='e';
		string_1[7]='c'; string_1[8]='t'; string_1[9]='r'; string_1[10]='o'; string_1[11]='n'; string_1[12]='i';
		string_1[13]='c'; string_1[14]='a';
		string_1[15]= 0; /*Termino el string*/
		putsXLCD(string_1);
		
		string_2[0]='D';string_2[1]='I';string_2[2]='S';string_2[3]='P';string_2[4]=' ';string_2[5]='I';
		string_2[6]='N';string_2[7]='I';string_2[8]='T';
		string_2[9]=' ';string_2[10]=' ';string_2[11]=' ';string_2[12]=' ';string_2[13]=' ';
		string_2[14]=0; /*Termino el string*/
		commandXLCD(LINEA_2); /*Paso a la segunda linea*/
		putsXLCD(string_2);
	}
}//end UserDisplayInit

/******************************************************************************
 * Function:        UserDisplayConfigure(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function sets the specific configuration for the user module, it is called by the framework 
 *						
 *
 * Note:            None
 *****************************************************************************/
void UserDisplayConfigure(void) {
// Do the configuration
}

/******************************************************************************
 * Function:        UserDisplayProcessIO(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is registered in the dinamic polling, who call ir periodically to process the IO interaction
 *					int the PIC, also it can comunicate things to the pc by the USB	
 *
 * Note:            None
 *****************************************************************************/
void UserDisplayProcessIO(void){  
	if((usb_device_state < CONFIGURED_STATE)||(UCONbits.SUSPND==1)) return;
	// here enter the code that want to be called periodically, per example interaction with buttons and leds.	
} //end ProcessIO


/******************************************************************************
 * Function:        UserDesplayRelease(byte i)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function release all the resources that the user module used, it is called by the framework 
 *					when the module is close	
 *
 * Note:            None
 *****************************************************************************/

void UserDisplayRelease(byte i){
	unsetHandlerReceiveBuffer(i);
	unsetHandlerReceiveFunction(i);
//	BusyXLCD();					/* Espero a que se libere el display*/ /*Esto causa una colgada! ojo*/
	commandXLCD(LIMPIAR);				/*Limpio el display*/
	commandXLCD(COMANDO_2); 			/*Limpio la memoria DDRAM*/
	commandXLCD(DOFF); 				/* Apago el display*/
//	BusyXLCD();					/* Espero a que se libere el display*/
//	TODO esta bien que este el release 
//	mReleaseDisplay(); 				/* Libero el TRISD*/
}


/******************************************************************************
 * Function:        UserDisplayReceived(byte* recBuffPtr, byte len)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function manages the comunication with the pc
 *
 * Note:            None
 *****************************************************************************/

void UserDisplayReceived(byte* recBuffPtr, byte len){
	  byte index;
	  char mens[9] = "Display";
	  char string_1[17]; 	/* caracteres a mostrar en el display en la linea 1*/
	  char string_2[17]; 	/* caracteres a mostrar en el display en la linea 2	*/
	  int i=0;
	  byte userDisplayCounter = 0;
      
	switch(((DISPLAY_DATA_PACKET*)recBuffPtr)->CMD){
		case READ_VERSION:
			displayInit();
			//dataPacket._byte[1] is len
			((DISPLAY_DATA_PACKET*)sendBufferUsrDisplay)->_byte[0] = ((DISPLAY_DATA_PACKET*)recBuffPtr)->_byte[0]; 
			((DISPLAY_DATA_PACKET*)sendBufferUsrDisplay)->_byte[1] = ((DISPLAY_DATA_PACKET*)recBuffPtr)->_byte[1];
			((DISPLAY_DATA_PACKET*)sendBufferUsrDisplay)->_byte[2] = DISPLAY_MINOR_VERSION;
			((DISPLAY_DATA_PACKET*)sendBufferUsrDisplay)->_byte[3] = DISPLAY_MAJOR_VERSION;
			userDisplayCounter=0x04;
		break;
		case ESCRIBIR:
			displayInit();
			((DISPLAY_DATA_PACKET*)sendBufferUsrDisplay)->_byte[0] = ((DISPLAY_DATA_PACKET*)recBuffPtr)->_byte[0]; 
			((DISPLAY_DATA_PACKET*)sendBufferUsrDisplay)->_byte[1] = ((DISPLAY_DATA_PACKET*)recBuffPtr)->_byte[1];
			for(i=1;i<33;i++){
				if(i<17){
					string_1[i-1]= ((DISPLAY_DATA_PACKET*)recBuffPtr)->_byte[i];
					if (string_1[i-1] == '_'){
						string_1[i-1] = ' ';
					}
				}else{
					string_2[i-17]= ((DISPLAY_DATA_PACKET*)recBuffPtr)->_byte[i];
					if (string_2[i-17] == '_'){
						string_2[i-17] = ' ';
					}
				}
			}
			commandXLCD(LIMPIAR);
			commandXLCD(CASA);
			commandXLCD(LINEA_1);  /*Inicio primera linea*/
			putsXLCD(string_1);
			commandXLCD(LINEA_2); /*Paso a la segunda linea*/
			putsXLCD(string_2);
			userDisplayCounter=0x02;
		break;  
		case PRUEBA:	
			displayInit();
			commandXLCD(LIMPIAR);
			commandXLCD(LINEA_1);
			((DISPLAY_DATA_PACKET*)sendBufferUsrDisplay)->_byte[0] =((DISPLAY_DATA_PACKET*)recBuffPtr)->_byte[0];
			((DISPLAY_DATA_PACKET*)sendBufferUsrDisplay)->_byte[1] = ((DISPLAY_DATA_PACKET*)recBuffPtr)->_byte[1]; 
			string_1[0]='I'; string_1[1]='M'; string_1[2]='M'; string_1[3]=' '; string_1[4]= 'E';
			string_1[5]='l'; string_1[6]='e';
			string_1[7]='c'; string_1[8]='t'; string_1[9]='r'; string_1[10]='o'; string_1[11]='n'; string_1[12]='i';
			string_1[13]='c';
			string_1[14]='a';
			string_1[15]= 0; /*Termino el string*/
			putsXLCD(string_1);
			string_2[0]='A';string_2[1]='.';string_2[2]='F';string_2[3]='.';string_2[4]='E';string_2[5]='.';string_2[6]=' ';
			string_2[7]=' ';
			string_2[8]=' ';string_2[9]=' ';string_2[10]=' ';string_2[11]=' ';string_2[12]=' ';string_2[13]=' ';string_2[14]=0;
			/*Termino el string*/
			commandXLCD(LINEA_2); /*Paso a la segunda linea*/
			putsXLCD(string_2);
			userDisplayCounter=0x02;
		break; 
		case BORRAR:
			displayInit();
			commandXLCD(LIMPIAR);
			commandXLCD(COMANDO_2); 	/*Limpio la memoria DDRAM*/
			((DISPLAY_DATA_PACKET*)sendBufferUsrDisplay)->_byte[0] = ((DISPLAY_DATA_PACKET*)recBuffPtr)->_byte[0]; 
			((DISPLAY_DATA_PACKET*)sendBufferUsrDisplay)->_byte[1] = ((DISPLAY_DATA_PACKET*)recBuffPtr)->_byte[1]; 
			userDisplayCounter=0x02;
		break;
		case INICIAR:
			displayInit();
			((DISPLAY_DATA_PACKET*)sendBufferUsrDisplay)->_byte[0] = ((DISPLAY_DATA_PACKET*)recBuffPtr)->_byte[0]; 
			((DISPLAY_DATA_PACKET*)sendBufferUsrDisplay)->_byte[1] = ((DISPLAY_DATA_PACKET*)recBuffPtr)->_byte[1]; 
			userDisplayCounter=0x02;
		break;
		case PRENDER_BKL:
			displayInit();
			BACK_LIGHT_ON();
			((DISPLAY_DATA_PACKET*)sendBufferUsrDisplay)->_byte[0] = ((DISPLAY_DATA_PACKET*)recBuffPtr)->_byte[0]; 
			((DISPLAY_DATA_PACKET*)sendBufferUsrDisplay)->_byte[1] = ((DISPLAY_DATA_PACKET*)recBuffPtr)->_byte[1]; 
			userDisplayCounter=0x02;
		break;
		case APAGAR_BKL:
			displayInit();
			BACK_LIGHT_OFF();
			((DISPLAY_DATA_PACKET*)sendBufferUsrDisplay)->_byte[0] = ((DISPLAY_DATA_PACKET*)recBuffPtr)->_byte[0]; 
			((DISPLAY_DATA_PACKET*)sendBufferUsrDisplay)->_byte[1] = ((DISPLAY_DATA_PACKET*)recBuffPtr)->_byte[1]; 
			userDisplayCounter=0x02;
		break;
		case RESET:
			Reset();
		break;
		case MESS:
			sendMes(mens, sizeof(mens));
		break;
	}//end switch(s)
	if(userDisplayCounter != 0){
		if(!mUSBGenTxIsBusy()){
			USBGenWrite2(usrDisplayHandler, userDisplayCounter);
		}
	}//end if	  	
} //end UserDisplayReceived


/** EOF usr_display.c ***************************************************************/
