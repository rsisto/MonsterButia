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
#include "user\usr_led_rojo.h"
#include "io_cfg.h"              // I/O pin mapping
#include "user\handlerManager.h"
#include "dynamicPolling.h"                              
 

/** V A R I A B L E S ********************************************************/
#pragma udata 

byte  usrLedRojoHandler;	 // Handler number asigned to the module
byte* sendBufferUsrLedRojo; // buffer to send data

/** P R I V A T E  P R O T O T Y P E S ***************************************/
void UserLedRojoProcessIO(void);
void UserLedRojoInit(byte i);
void UserLedRojoReceived(byte*, byte);
void UserLedRojoRelease(byte i);
void UserLedRojoConfigure(void);

// Table used by te framework to get a fixed reference point to the user module functions defined by the framework 
/** USER MODULE REFERENCE*****************************************************/
#pragma romdata user
uTab userLedRojoModuleTable = {&UserLedRojoInit,&UserLedRojoRelease,&UserLedRojoConfigure,"ledR"}; //modName must be less or equal 8 characters
#pragma code

/** D E C L A R A T I O N S **************************************************/
#pragma code module

/******************************************************************************
 * Function:        UseLedRojoInit(void)
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

void UserLedRojoInit(byte i){
	BOOL res;
	usrLedRojoHandler = i;
	// add my receive function to the handler module, to be called automatically when the pc sends data to the user module
	setHandlerReceiveFunction(usrLedRojoHandler,&UserLedRojoReceived);
	// add my receive pooling function to the dynamic pooling module, to be called periodically 
	//res = addPollingFunction(&UserLedRojoProcessIO);
	// initialize the send buffer, used to send data to the PC
	sendBufferUsrLedRojo = getSharedBuffer(usrLedRojoHandler);
	//TODO return res value 
	mInitLed1();
}//end UserLedRojoInit

/******************************************************************************
/* Function:        UserRojoConfigure(void)
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
void UserLedRojoConfigure(void){
// Do the configuration
}

/******************************************************************************
 * Function:        UserLedRojoProcessIO(void)
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

void UserLedRojoProcessIO(void){  

    if((usb_device_state < CONFIGURED_STATE)||(UCONbits.SUSPND==1)) return;
	// here enter the code that want to be called periodically, per example interaction with buttons and leds
}//end ProcessIO



/******************************************************************************
 * Function:        UserLedRojoRelease(byte i)
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

void UserLedRojoRelease(byte i){
	unsetHandlerReceiveBuffer(i);
	unsetHandlerReceiveFunction(i);
	removePoolingFunction(&UserLedRojoProcessIO);
}


/******************************************************************************
 * Function:        UserLedRojoReceived(byte* recBuffPtr, byte len)
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

void UserLedRojoReceived(byte* recBuffPtr, byte len){
	  byte index;
	  char mens[9] = "User Skeleton is alive";	
      byte userLedRojoCounter = 0;
      switch(((LED_ROJO_DATA_PACKET*)recBuffPtr)->CMD)
      {
        case READ_VERSION:
              //dataPacket._byte[1] is len
		((LED_ROJO_DATA_PACKET*)sendBufferUsrLedRojo)->_byte[0] = ((LED_ROJO_DATA_PACKET*)recBuffPtr)->_byte[0]; 
		((LED_ROJO_DATA_PACKET*)sendBufferUsrLedRojo)->_byte[1] = ((LED_ROJO_DATA_PACKET*)recBuffPtr)->_byte[1]; 
		((LED_ROJO_DATA_PACKET*)sendBufferUsrLedRojo)->_byte[2] = LED_ROJO_MINOR_VERSION;
		((LED_ROJO_DATA_PACKET*)sendBufferUsrLedRojo)->_byte[3] = LED_ROJO_MAJOR_VERSION;
		userLedRojoCounter = 0x04;
		break;    
        case PRENDER:
		((LED_ROJO_DATA_PACKET*)sendBufferUsrLedRojo)->_byte[0] = ((LED_ROJO_DATA_PACKET*)recBuffPtr)->_byte[0]; 
		((LED_ROJO_DATA_PACKET*)sendBufferUsrLedRojo)->_byte[1] = ((LED_ROJO_DATA_PACKET*)recBuffPtr)->_byte[1]; 
		mLED_1_On();
		userLedRojoCounter = 0x02;
	break;  
	case APAGAR:
		((LED_ROJO_DATA_PACKET*)sendBufferUsrLedRojo)->_byte[0] = ((LED_ROJO_DATA_PACKET*)recBuffPtr)->_byte[0]; 
		((LED_ROJO_DATA_PACKET*)sendBufferUsrLedRojo)->_byte[1] = ((LED_ROJO_DATA_PACKET*)recBuffPtr)->_byte[1]; 
		mLED_1_Off(); 
		userLedRojoCounter = 0x02;
	break;  
        case RESET:
		((LED_ROJO_DATA_PACKET*)sendBufferUsrLedRojo)->_byte[0] = ((LED_ROJO_DATA_PACKET*)recBuffPtr)->_byte[0]; 
		((LED_ROJO_DATA_PACKET*)sendBufferUsrLedRojo)->_byte[1] = ((LED_ROJO_DATA_PACKET*)recBuffPtr)->_byte[1]; 
		Reset();
		userLedRojoCounter = 0x02;
	break;
	case MESS:
		((LED_ROJO_DATA_PACKET*)sendBufferUsrLedRojo)->_byte[0] = ((LED_ROJO_DATA_PACKET*)recBuffPtr)->_byte[0]; 
		((LED_ROJO_DATA_PACKET*)sendBufferUsrLedRojo)->_byte[1] = ((LED_ROJO_DATA_PACKET*)recBuffPtr)->_byte[1]; 
		sendMes(mens, sizeof(mens));
		userLedRojoCounter = 0x02;
	break;
	default:
	break;
      }//end switch(s)
      if(userLedRojoCounter != 0)
      {
          if(!mUSBGenTxIsBusy())
              USBGenWrite2(usrLedRojoHandler, userLedRojoCounter);
      }//end if  	  	
}//end UserSkelReceived

/** EOF usr_led_rojo.c ***************************************************************/
