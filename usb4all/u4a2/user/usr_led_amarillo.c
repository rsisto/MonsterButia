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
#include "user\usr_led_amarillo.h"
#include "io_cfg.h"              // I/O pin mapping
#include "user\handlerManager.h"
#include "dynamicPolling.h"                              
#include "usb4all\proxys\T0Service.h" 
 
/** V A R I A B L E S ********************************************************/
#pragma udata 

byte  usrLedAmarilloHandler;	 // Handler number asigned to the module
byte* sendBufferUsrLedAmarillo;  // buffer to send data
byte  blinkFrecuency = 10;

/** P R I V A T E  P R O T O T Y P E S ***************************************/
void UserLedAmarilloProcessIO(void);
void UserLedAmarilloInit(byte i);
void UserLedAmarilloReceived(byte*, byte);
void UserLedAmarilloRelease(byte i);
void UserLedAmarilloConfigure(void);

// Table used by te framework to get a fixed reference point to the user module functions defined by the framework 
/** USER MODULE REFERENCE*****************************************************/
#pragma romdata user
uTab userLedAmarilloModuleTable = {&UserLedAmarilloInit,&UserLedAmarilloRelease,&UserLedAmarilloConfigure,"ledA"}; //modName must be less or equal 8 characters
#pragma code

/** D E C L A R A T I O N S **************************************************/
#pragma code module

/******************************************************************************
 * Function:        UseLedAmarilloInit(void)
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

void UserLedAmarilloInit(byte i){
	BOOL res;
	usrLedAmarilloHandler = i;
	// add my receive function to the handler module, to be called automatically when the pc sends data to the user module
	setHandlerReceiveFunction(usrLedAmarilloHandler,&UserLedAmarilloReceived);
	// add my receive pooling function to the dynamic pooling module, to be called periodically 
	//res = addPollingFunction(&UserLedAmarilloProcessIO);
	// initialize the send buffer, used to send data to the PC
	sendBufferUsrLedAmarillo = getSharedBuffer(usrLedAmarilloHandler);
	mInitLed3();
	initT0Service();
}//end UserLedAmarilloInit

/******************************************************************************
/* Function:        UserAmarilloConfigure(void)
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
void UserLedAmarilloConfigure(void){
// Do the configuration
}

/******************************************************************************
 * Function:        UserLedAmarilloProcessIO(void)
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

void UserLedAmarilloProcessIO(void){  

    if((usb_device_state < CONFIGURED_STATE)||(UCONbits.SUSPND==1)) return;
	// here enter the code that want to be called periodically, per example interaction with buttons and leds
}//end ProcessIO



/******************************************************************************
 * Function:        UserLedAmarilloRelease(byte i)
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

void UserLedAmarilloRelease(byte i){
	unsetHandlerReceiveBuffer(i);
	unsetHandlerReceiveFunction(i);
	removePoolingFunction(&UserLedAmarilloProcessIO);
}

/***********************************************************************
*
* Callback function to execute periodicaly by te Timmer interrupt ISR
*
************************************************************************/
void blinkEvent(void) {
	mLED_3_Toggle();
	registerT0eventInEvent(2000, &blinkEvent);
}

/******************************************************************************
 * Function:        UserLedAmarilloReceived(byte* recBuffPtr, byte len)
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

void UserLedAmarilloReceived(byte* recBuffPtr, byte len){
	  byte index;
	  char mens[9] = "User Skeleton is alive";	
      byte userLedAmarilloCounter = 0;
      switch(((LED_AMARILLO_DATA_PACKET*)recBuffPtr)->CMD)
      {
        case READ_VERSION:
              //dataPacket._byte[1] is len
		((LED_AMARILLO_DATA_PACKET*)sendBufferUsrLedAmarillo)->_byte[0] = ((LED_AMARILLO_DATA_PACKET*)recBuffPtr)->_byte[0];
		((LED_AMARILLO_DATA_PACKET*)sendBufferUsrLedAmarillo)->_byte[1] = ((LED_AMARILLO_DATA_PACKET*)recBuffPtr)->_byte[1]; 
		((LED_AMARILLO_DATA_PACKET*)sendBufferUsrLedAmarillo)->_byte[2] = LED_AMARILLO_MINOR_VERSION;
		((LED_AMARILLO_DATA_PACKET*)sendBufferUsrLedAmarillo)->_byte[3] = LED_AMARILLO_MAJOR_VERSION;
		userLedAmarilloCounter = 0x04;
	break;
        case BLINK:
		((LED_AMARILLO_DATA_PACKET*)sendBufferUsrLedAmarillo)->_byte[0] = ((LED_AMARILLO_DATA_PACKET*)recBuffPtr)->_byte[0];
		((LED_AMARILLO_DATA_PACKET*)sendBufferUsrLedAmarillo)->_byte[1] = ((LED_AMARILLO_DATA_PACKET*)recBuffPtr)->_byte[1]; 		
		registerT0event(2000, &blinkEvent);
		userLedAmarilloCounter = 0x02;
	break;
	case PRENDER:
		((LED_AMARILLO_DATA_PACKET*)sendBufferUsrLedAmarillo)->_byte[0] = ((LED_AMARILLO_DATA_PACKET*)recBuffPtr)->_byte[0];
		((LED_AMARILLO_DATA_PACKET*)sendBufferUsrLedAmarillo)->_byte[1] = ((LED_AMARILLO_DATA_PACKET*)recBuffPtr)->_byte[1]; 
		mLED_3_On();
		userLedAmarilloCounter = 0x02;
	break;
	case APAGAR:
		((LED_AMARILLO_DATA_PACKET*)sendBufferUsrLedAmarillo)->_byte[0] = ((LED_AMARILLO_DATA_PACKET*)recBuffPtr)->_byte[0];
		((LED_AMARILLO_DATA_PACKET*)sendBufferUsrLedAmarillo)->_byte[1] = ((LED_AMARILLO_DATA_PACKET*)recBuffPtr)->_byte[1]; 
		mLED_3_Off(); 
		unregisterT0event(&blinkEvent);
		userLedAmarilloCounter = 0x02;
	break;
        case RESET:
		((LED_AMARILLO_DATA_PACKET*)sendBufferUsrLedAmarillo)->_byte[0] = ((LED_AMARILLO_DATA_PACKET*)recBuffPtr)->_byte[0];
		((LED_AMARILLO_DATA_PACKET*)sendBufferUsrLedAmarillo)->_byte[1] = ((LED_AMARILLO_DATA_PACKET*)recBuffPtr)->_byte[1]; 
		Reset();
		userLedAmarilloCounter = 0x02;
	break;
	case MESS:
		((LED_AMARILLO_DATA_PACKET*)sendBufferUsrLedAmarillo)->_byte[0] = ((LED_AMARILLO_DATA_PACKET*)recBuffPtr)->_byte[0];
		((LED_AMARILLO_DATA_PACKET*)sendBufferUsrLedAmarillo)->_byte[1] = ((LED_AMARILLO_DATA_PACKET*)recBuffPtr)->_byte[1]; 
		sendMes(mens, sizeof(mens));
		userLedAmarilloCounter = 0x02;
	break;
	default:
        break;
      }//end switch(s)
      if(userLedAmarilloCounter != 0)
      {
          if(!mUSBGenTxIsBusy())
              USBGenWrite2(usrLedAmarilloHandler, userLedAmarilloCounter);
      }//end if  	  	
}//end UserAmarilloReceived

/** EOF usr_led_amarillo.c ***************************************************************/