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
#include "user\usr_puerta.h"
#include "io_cfg.h"              // I/O pin mapping
#include "user\handlerManager.h"
#include "dynamicPolling.h"                              

 
/** V A R I A B L E S ********************************************************/
#pragma udata 

byte  usrPuertaHandler;	 // Handler number asigned to the module
byte* sendBufferUsrPuerta; // buffer to send data

/** P R I V A T E  P R O T O T Y P E S ***************************************/
void UserPuertaProcessIO(void);
void UserPuertaInit(byte i);
void UserPuertaReceived(byte*, byte);
void UserPuertaRelease(byte i);
void UserPuertaConfigure(void);

// Table used by te framework to get a fixed reference point to the user module functions defined by the framework 
/** USER MODULE REFERENCE*****************************************************/
#pragma romdata user
uTab userPuertaModuleTable = {&UserPuertaInit,&UserPuertaRelease,&UserPuertaConfigure,"puerta"}; //modName must be less or equal 8 characters
#pragma code

/** D E C L A R A T I O N S **************************************************/
#pragma code module

/******************************************************************************
 * Function:        UsePuertaInit(void)
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

void UserPuertaInit(byte i){
	BOOL res;
	usrPuertaHandler = i;
	// add my receive function to the handler module, to be called automatically when the pc sends data to the user module
	setHandlerReceiveFunction(usrPuertaHandler,&UserPuertaReceived);
	// add my receive pooling function to the dynamic pooling module, to be called periodically 
	//res = addPollingFunction(&UserPuertaProcessIO);
	// initialize the send buffer, used to send data to the PC
	sendBufferUsrPuerta = getSharedBuffer(usrPuertaHandler);
	//TODO return res value 
	mInitPuerta();
	puerta_off();
}//end UserPuertaInit

/******************************************************************************
/* Function:        UserPuertaConfigure(void)
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
void UserPuertaConfigure(void){
// Do the configuration
}

/******************************************************************************
 * Function:        UserPuertaProcessIO(void)
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

void UserPuertaProcessIO(void){  

    if((usb_device_state < CONFIGURED_STATE)||(UCONbits.SUSPND==1)) return;
	// here enter the code that want to be called periodically, per example interaction with buttons and leds
}//end ProcessIO



/******************************************************************************
 * Function:        UserPuertaRelease(byte i)
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

void UserPuertaRelease(byte i){
	unsetHandlerReceiveBuffer(i);
	unsetHandlerReceiveFunction(i);
	removePoolingFunction(&UserPuertaProcessIO);
}


/******************************************************************************
 * Function:        UserPuertaReceived(byte* recBuffPtr, byte len)
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

void UserPuertaReceived(byte* recBuffPtr, byte len){
	  byte index;
	  char mens[9] = "puerta";	
      byte userPuertaCounter = 0;
      switch(((PUERTA_DATA_PACKET*)recBuffPtr)->CMD)
      {
        case READ_VERSION:
              //dataPacket._byte[1] is len
			  ((PUERTA_DATA_PACKET*)sendBufferUsrPuerta)->_byte[0] = ((PUERTA_DATA_PACKET*)recBuffPtr)->_byte[0]; 
			  ((PUERTA_DATA_PACKET*)sendBufferUsrPuerta)->_byte[1] = ((PUERTA_DATA_PACKET*)recBuffPtr)->_byte[1]; 
              ((PUERTA_DATA_PACKET*)sendBufferUsrPuerta)->_byte[2] = PUERTA_MINOR_VERSION;
              ((PUERTA_DATA_PACKET*)sendBufferUsrPuerta)->_byte[3] = PUERTA_MAJOR_VERSION;
              userPuertaCounter = 0x04;
              break;  
	          
        case ABRIR:
              		puerta_on();
					Delay10KTCYx(150);
					puerta_off();
					userPuertaCounter = 0x00;
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
      if(userPuertaCounter != 0)
      {
          if(!mUSBGenTxIsBusy())
              USBGenWrite2(usrPuertaHandler, userPuertaCounter);
      }//end if  	  	
}//end UserPuertaReceived

/** EOF usr_puerta.c ***************************************************************/
