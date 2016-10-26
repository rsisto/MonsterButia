/* Author             									  Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *Santiago Reyes					       03/07/09    Original.
 *****************************************************************************/
 
/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include <usart.h>
#include <delays.h>
#include "system\typedefs.h"
#include "system\usb\usb.h"
#include "user\usr_buzzer.h"
#include "io_cfg.h"              // I/O pin mapping
#include "user\handlerManager.h"
#include "dynamicPolling.h"                              

  
/** V A R I A B L E S ********************************************************/
#pragma udata 

byte  usrBuzzerHandler;	 // Handler number asigned to the module
byte* sendBufferUsrBuzzer; // buffer to send data

/** P R I V A T E  P R O T O T Y P E S ***************************************/
void UserBuzzerProcessIO(void);
void UserBuzzerInit(byte i);
void UserBuzzerReceived(byte*, byte);
void UserBuzzerRelease(byte i);
void UserBuzzerConfigure(void);

// Table used by te framework to get a fixed reference point to the user module functions defined by the framework 
/** USER MODULE REFERENCE*****************************************************/
#pragma romdata user
uTab userBuzzerModuleTable = {&UserBuzzerInit,&UserBuzzerRelease,&UserBuzzerConfigure,"buzzer"}; //modName must be less or equal 8 characters
#pragma code

/** D E C L A R A T I O N S **************************************************/
#pragma code module

/******************************************************************************
 * Function:        UseBuzzerInit(void)
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

void UserBuzzerInit(byte i){
	BOOL res;
	usrBuzzerHandler = i;
	// add my receive function to the handler module, to be called automatically when the pc sends data to the user module
	setHandlerReceiveFunction(usrBuzzerHandler,&UserBuzzerReceived);
	// add my receive pooling function to the dynamic pooling module, to be called periodically 
	//res = addPollingFunction(&UserLedAmarilloProcessIO);
	// initialize the send buffer, used to send data to the PC
	sendBufferUsrBuzzer = getSharedBuffer(usrBuzzerHandler);
	//TODO return res value 
	mInitBuzzer(); /*Inicializo el puerto del buzzer*/
}//end UserLedAmarilloInit

/******************************************************************************
/* Function:        UserBuzzerConfigure(void)
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
void UserBuzzerConfigure(void){
// Do the configuration
}

/******************************************************************************
 * Function:        UserBuzzerProcessIO(void)
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

void UserBuzzerProcessIO(void){  

    if((usb_device_state < CONFIGURED_STATE)||(UCONbits.SUSPND==1)) return;
	// here enter the code that want to be called periodically, per example interaction with buttons and leds
}//end ProcessIO



/******************************************************************************
 * Function:        UserBuzzerRelease(byte i)
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

void UserBuzzerRelease(byte i){
	unsetHandlerReceiveBuffer(i);
	unsetHandlerReceiveFunction(i);
	removePoolingFunction(&UserBuzzerProcessIO);
}

void prender_corto(byte i){
	byte tiempo_decimas;
	byte tiempo_unidades;
	switch (i){
		case 0x01:
			tiempo_decimas = 0x32; /* para que lo prenda por 1 seg */
			tiempo_unidades = 0x00; 
		break;

		case 0x02:
			tiempo_decimas = 0x64; /* para que lo prenda por 2 seg */
			tiempo_unidades = 0x00; 
		break;

		case 0x03:
			tiempo_decimas = 0x96; /* para que lo prenda por 3 seg */
			tiempo_unidades = 0x00; 
		break;

		case 0x04:
			tiempo_decimas = 0xC8; /* para que lo prenda por 4 seg */
			tiempo_unidades = 0x00; 
		break;

		case 0x05:
			tiempo_decimas = 0xFA; /* para que lo prenda por 5 seg */
			tiempo_unidades = 0x00; 
		break;

		case 0x06:
			tiempo_decimas = 0xFA; /* para que lo prenda por 6 seg */
			tiempo_unidades = 0x32; 
		break;

		case 0x07:
			tiempo_decimas = 0xFA; /* para que lo prenda por 7 seg */
			tiempo_unidades = 0x64; 
		break;

		case 0x08:
			tiempo_decimas = 0xFA; /* para que lo prenda por 8 seg */
			tiempo_unidades = 0x96; 
		break;

		case 0x09:
			tiempo_decimas = 0xFA; /* para que lo prenda por 9 seg */
			tiempo_unidades = 0xC8; 
		break;

		default:
			tiempo_decimas = 0x32; /* para que lo prenda por 1 seg */
			tiempo_unidades = 0x00; 
		break;
	}
	buzzer_on();
		Delay10KTCYx(tiempo_decimas);
		Delay10KTCYx(tiempo_unidades);
	buzzer_off();
}
/******************************************************************************
 * Function:        UserBuzzerReceived(byte* recBuffPtr, byte len)
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

void UserBuzzerReceived(byte* recBuffPtr, byte len){
	  byte index;
	  char mens[9] = "BUZZER";	
      byte userBuzzerCounter = 0;
	  byte tiempo;

      switch(((BUZZER_DATA_PACKET*)recBuffPtr)->CMD)
      {
        case READ_VERSION:
              //dataPacket._byte[1] is len
			  ((BUZZER_DATA_PACKET*)sendBufferUsrBuzzer)->_byte[0] = ((BUZZER_DATA_PACKET*)recBuffPtr)->_byte[0]; 
			  ((BUZZER_DATA_PACKET*)sendBufferUsrBuzzer)->_byte[1] = ((BUZZER_DATA_PACKET*)recBuffPtr)->_byte[1]; 
              ((BUZZER_DATA_PACKET*)sendBufferUsrBuzzer)->_byte[2] = BUZZER_MINOR_VERSION;
              ((BUZZER_DATA_PACKET*)sendBufferUsrBuzzer)->_byte[3] = BUZZER_MAJOR_VERSION;
              userBuzzerCounter = 0x04;
              break;  
	          
        case PRENDER:
			  ((BUZZER_DATA_PACKET*)sendBufferUsrBuzzer)->_byte[0] = ((BUZZER_DATA_PACKET*)recBuffPtr)->_byte[0]; 
			  ((BUZZER_DATA_PACKET*)sendBufferUsrBuzzer)->_byte[1] = ((BUZZER_DATA_PACKET*)recBuffPtr)->_byte[1]; 
              buzzer_on();
              userBuzzerCounter = 0x02;
              break;  
        
		case APAGAR:
			  ((BUZZER_DATA_PACKET*)sendBufferUsrBuzzer)->_byte[0] = ((BUZZER_DATA_PACKET*)recBuffPtr)->_byte[0]; 
			  ((BUZZER_DATA_PACKET*)sendBufferUsrBuzzer)->_byte[1] = ((BUZZER_DATA_PACKET*)recBuffPtr)->_byte[1]; 
              buzzer_off(); 
			  userBuzzerCounter = 0x02;
              break;  

		case BUZZER_CORTO:
			  ((BUZZER_DATA_PACKET*)sendBufferUsrBuzzer)->_byte[0] = ((BUZZER_DATA_PACKET*)recBuffPtr)->_byte[0]; 
			  ((BUZZER_DATA_PACKET*)sendBufferUsrBuzzer)->_byte[1] = ((BUZZER_DATA_PACKET*)recBuffPtr)->_byte[1]; 
				tiempo = ((BUZZER_DATA_PACKET*)recBuffPtr)->_byte[2];
              prender_corto(tiempo); 
			  userBuzzerCounter = 0x02;
              break;		
        case RESET:
              Reset();
			  break;
		  
		 case MESS:
				sendMes(mens, sizeof(mens));
              break;
         
		 default:
              break;
      }//end switch(s)
      if(userBuzzerCounter != 0)
      {
          if(!mUSBGenTxIsBusy())
              USBGenWrite2(usrBuzzerHandler, userBuzzerCounter);
      }//end if  	  	
}//end UserBuzzerReceived

/** EOF usr_Buzzer.c ***************************************************************/