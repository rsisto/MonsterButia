/* Author             									  Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *Santiago Reyes					       03/07/09    Original.
 *Andres Aguirre					       30/07/10    Interaccion con timmer y debugeo
 *****************************************************************************/

/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include <usart.h>
#include "system\typedefs.h"
#include "system\usb\usb.h"
#include "user\usr_led_verde.h"
#include "io_cfg.h"              // I/O pin mapping
#include "user\handlerManager.h"
#include "dynamicPolling.h"                              
 

/** V A R I A B L E S ********************************************************/
#pragma udata 

byte  usrLedVerdeHandler;	 // Handler number asigned to the module
byte* sendBufferUsrLedVerde; // buffer to send data

/** P R I V A T E  P R O T O T Y P E S ***************************************/
void UserLedVerdeProcessIO(void);
void UserLedVerdeInit(byte i);
void UserLedVerdeReceived(byte*, byte);
void UserLedVerdeRelease(byte i);
void UserLedVerdeConfigure(void);

// Table used by te framework to get a fixed reference point to the user module functions defined by the framework 
/** USER MODULE REFERENCE*****************************************************/
#pragma romdata user
uTab userLedVerdeModuleTable = {&UserLedVerdeInit,&UserLedVerdeRelease,&UserLedVerdeConfigure,"ledV"}; //modName must be less or equal 8 characters
#pragma code

/** D E C L A R A T I O N S **************************************************/
#pragma code module

/******************************************************************************
 * Function:        UseLedVerdeInit(void)
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

void UserLedVerdeInit(byte i){
	BOOL res;
	usrLedVerdeHandler = i;
	// add my receive function to the handler module, to be called automatically when the pc sends data to the user module
	setHandlerReceiveFunction(usrLedVerdeHandler,&UserLedVerdeReceived);
	// add my receive pooling function to the dynamic pooling module, to be called periodically 
	//res = addPollingFunction(&UserLedVerdeProcessIO);
	// initialize the send buffer, used to send data to the PC
	sendBufferUsrLedVerde = getSharedBuffer(usrLedVerdeHandler);
	//TODO return res value 
	mInitLed2();
}//end UserLedVerdeInit

/******************************************************************************
/* Function:        UserVerdeConfigure(void)
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
void UserLedVerdeConfigure(void){
// Do the configuration
}

/******************************************************************************
 * Function:        UserLedVerdeProcessIO(void)
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

void UserLedVerdeProcessIO(void){  

    if((usb_device_state < CONFIGURED_STATE)||(UCONbits.SUSPND==1)) return;
	// here enter the code that want to be called periodically, per example interaction with buttons and leds
}//end ProcessIO



/******************************************************************************
 * Function:        UserLedVerdeRelease(byte i)
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

void UserLedVerdeRelease(byte i){
	unsetHandlerReceiveBuffer(i);
	unsetHandlerReceiveFunction(i);
	removePoolingFunction(&UserLedVerdeProcessIO);
}


/******************************************************************************
 * Function:        UserLedVerdeReceived(byte* recBuffPtr, byte len)
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

void UserLedVerdeReceived(byte* recBuffPtr, byte len){
	  byte index;
	  char mens[9] = "LDV is on";	
      byte userLedVerdeCounter = 0;
      switch(((LED_VERDE_DATA_PACKET*)recBuffPtr)->CMD)
      {
        case READ_VERSION:
		//dataPacket._byte[1] is len
		((LED_VERDE_DATA_PACKET*)sendBufferUsrLedVerde)->_byte[0] = ((LED_VERDE_DATA_PACKET*)recBuffPtr)->_byte[0]; 
		((LED_VERDE_DATA_PACKET*)sendBufferUsrLedVerde)->_byte[1] = ((LED_VERDE_DATA_PACKET*)recBuffPtr)->_byte[1]; 
		((LED_VERDE_DATA_PACKET*)sendBufferUsrLedVerde)->_byte[2] = LED_VERDE_MINOR_VERSION;
		((LED_VERDE_DATA_PACKET*)sendBufferUsrLedVerde)->_byte[3] = LED_VERDE_MAJOR_VERSION;
		userLedVerdeCounter = 0x04;
	break;  
        case PRENDER:
		((LED_VERDE_DATA_PACKET*)sendBufferUsrLedVerde)->_byte[0] = ((LED_VERDE_DATA_PACKET*)recBuffPtr)->_byte[0]; 
		((LED_VERDE_DATA_PACKET*)sendBufferUsrLedVerde)->_byte[1] = ((LED_VERDE_DATA_PACKET*)recBuffPtr)->_byte[1]; 
		mLED_2_On();
		userLedVerdeCounter = 0x02;
	break;  
	case APAGAR:
		((LED_VERDE_DATA_PACKET*)sendBufferUsrLedVerde)->_byte[0] = ((LED_VERDE_DATA_PACKET*)recBuffPtr)->_byte[0]; 
		((LED_VERDE_DATA_PACKET*)sendBufferUsrLedVerde)->_byte[1] = ((LED_VERDE_DATA_PACKET*)recBuffPtr)->_byte[1]; 
		mLED_2_Off(); 
		userLedVerdeCounter = 0x02;
	break;  
        case RESET:
		((LED_VERDE_DATA_PACKET*)sendBufferUsrLedVerde)->_byte[0] = ((LED_VERDE_DATA_PACKET*)recBuffPtr)->_byte[0]; 
		((LED_VERDE_DATA_PACKET*)sendBufferUsrLedVerde)->_byte[1] = ((LED_VERDE_DATA_PACKET*)recBuffPtr)->_byte[1]; 
		Reset();
		userLedVerdeCounter = 0x02;
	break;
	case MESS:
		((LED_VERDE_DATA_PACKET*)sendBufferUsrLedVerde)->_byte[0] = ((LED_VERDE_DATA_PACKET*)recBuffPtr)->_byte[0]; 
		((LED_VERDE_DATA_PACKET*)sendBufferUsrLedVerde)->_byte[1] = ((LED_VERDE_DATA_PACKET*)recBuffPtr)->_byte[1]; 
		sendMes(mens, sizeof(mens));
		userLedVerdeCounter = 0x02;
	break;
	default:
	break;
      }//end switch(s)
      if(userLedVerdeCounter != 0)
      {
          if(!mUSBGenTxIsBusy())
              USBGenWrite2(usrLedVerdeHandler, userLedVerdeCounter);
      }//end if  	  	
}//end UserVerdeReceived

/** EOF usr_led_verde.c ***************************************************************/