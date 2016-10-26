/* Author             									  Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Andrés Aguirre, Rafael Fernandez					       03/11/07    Original.
 *****************************************************************************/

/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include <usart.h>
#include "system\typedefs.h"
#include "system\usb\usb.h"
#include "user\usr_skeleton.h"
#include "io_cfg.h"              // I/O pin mapping
#include "user\handlerManager.h"
#include "dynamicPolling.h"                              


/** V A R I A B L E S ********************************************************/
#pragma udata 

byte  usrSkelHandler;	 // Handler number asigned to the module
byte* sendBufferUsrSkel; // buffer to send data

/** P R I V A T E  P R O T O T Y P E S ***************************************/
void UserSkelProcessIO(void);
void UserSkelInit(byte i);
void UserSkelReceived(byte*, byte);
void UserSkelRelease(byte i);
void UserSkelConfigure(void);

// Table used by te framework to get a fixed reference point to the user module functions defined by the framework 
/** USER MODULE REFERENCE*****************************************************/
#pragma romdata user
uTab userSkelModuleTable = {&UserSkelInit,&UserSkelRelease,&UserSkelConfigure,"modName"}; //modName must be less or equal 8 characters
#pragma code

/** D E C L A R A T I O N S **************************************************/
#pragma code module

/******************************************************************************
 * Function:        UserSkelInit(void)
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

void UserSkelInit(byte i){
	BOOL res;
	usrSkelHandler = i;
	// add my receive function to the handler module, to be called automatically when the pc sends data to the user module
	setHandlerReceiveFunction(usrSkelHandler,&UserSkelReceived);
	// add my receive pooling function to the dynamic pooling module, to be called periodically 
	res = addPollingFunction(&UserSkelProcessIO);
	// initialize the send buffer, used to send data to the PC
	sendBufferUsrSkel = getSharedBuffer(usrSkelHandler);
	//TODO return res value 
}//end UserSkelInit

/******************************************************************************
 * Function:        UserSkelConfigure(void)
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
void UserSkelConfigure(void){
// Do the configuration
}

/******************************************************************************
 * Function:        UserSkelProcessIO(void)
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

void UserSkelProcessIO(void){  

    if((usb_device_state < CONFIGURED_STATE)||(UCONbits.SUSPND==1)) return;
	// here enter the code that want to be called periodically, per example interaction with buttons and leds
	
}//end ProcessIO



/******************************************************************************
 * Function:        UserSkelRelease(byte i)
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

void UserSkelRelease(byte i){
	unsetHandlerReceiveBuffer(i);
	unsetHandlerReceiveFunction(i);
	removePoolingFunction(&UserSkelProcessIO);
}


/******************************************************************************
 * Function:        UserSkelReceived(byte* recBuffPtr, byte len)
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

void UserSkelReceived(byte* recBuffPtr, byte len){
	  byte index;
	  char mens[9] = "User Skeleton is alive";	
      byte userSkelCounter = 0;
      switch(((SKEL_DATA_PACKET*)recBuffPtr)->CMD)
      {
        case READ_VERSION:
              //dataPacket._byte[1] is len
			  ((SKEL_DATA_PACKET*)sendBufferUsrSkel)->_byte[0] = ((SKEL_DATA_PACKET*)recBuffPtr)->_byte[0]; 
			  ((SKEL_DATA_PACKET*)sendBufferUsrSkel)->_byte[1] = ((SKEL_DATA_PACKET*)recBuffPtr)->_byte[1]; 
              ((SKEL_DATA_PACKET*)sendBufferUsrSkel)->_byte[2] = SKEL_MINOR_VERSION;
              ((SKEL_DATA_PACKET*)sendBufferUsrSkel)->_byte[3] = SKEL_MAJOR_VERSION;
              userSkelCounter=0x04;
              break;  
	          
        //case ANY_COMMAND:
        //      fill the SKEL_DATA_PACKET acording to the data to be sent
		//	  userSkelCounter=size of data to be sent;
        //      break;  
         
          case RESET:
              Reset();
			  break;
		  
		 case MESS:
				sendMes(mens, sizeof(mens));
              break;
         
		 default:
              break;
      }//end switch(s)
      if(userSkelCounter != 0)
      {
          if(!mUSBGenTxIsBusy())
              USBGenWrite2(usrSkelHandler, userSkelCounter);
      }//end if  	  	

}//end UserSkelReceived

/** EOF usr_skeleton.c ***************************************************************/
