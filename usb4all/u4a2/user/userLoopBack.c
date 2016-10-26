/* Author             									  Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Andrés Aguirre, Rafael Fernandez, Carlos Grossy       16/10/07    Original.
 *****************************************************************************/

/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include <usart.h>
#include "system\typedefs.h"
#include "system\usb\usb.h"
#include "user\userLoopBack.h"
#include "io_cfg.h"              // I/O pin mapping
#include "user\handlerManager.h"
#include "dynamicPolling.h"                              


/** V A R I A B L E S ********************************************************/
#pragma udata 

byte  usrLoopbackHandler;	 // Handler number asigned to the module
byte* sendBufferUsrLoopback; // buffer to send data

/** P R I V A T E  P R O T O T Y P E S ***************************************/
void UserLoopBackProcessIO(void);
void UserLoopBackInit(byte i);
void UserLoopBackReceived(byte*, byte);
void UserLoopBackRelease(byte i);
void UserLoopBackConfigure(void);

// Table used by te framework to get a fixed reference point to the user module functions defined by the framework 
/** USER MODULE REFERENCE*****************************************************/
#pragma romdata user
uTab UserLoopBackModuleTable = {&UserLoopBackInit,&UserLoopBackRelease,&UserLoopBackConfigure,"lback"}; //modName must be less or equal 8 characters
#pragma code

/** D E C L A R A T I O N S **************************************************/
#pragma code module

/******************************************************************************
 * Function:        UserLoopBackInit(void)
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

void UserLoopBackInit(byte i){
	BOOL res;
	usrLoopbackHandler = i;
	// add my receive function to the handler module, to be called automatically when the pc sends data to the user module
	setHandlerReceiveFunction(usrLoopbackHandler,&UserLoopBackReceived);
	// add my receive pooling function to the dynamic pooling module, to be called periodically 
	res = addPollingFunction(&UserLoopBackProcessIO);
	// initialize the send buffer, used to send data to the PC
	sendBufferUsrLoopback = getSharedBuffer(usrLoopbackHandler);
	//TODO return res value 
}//end UserLoopBackInit

/******************************************************************************
 * Function:        UserLoopBackConfigure(void)
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
void UserLoopBackConfigure(void){
// Do the configuration
}

/******************************************************************************
 * Function:        UserLoopBackProcessIO(void)
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

void UserLoopBackProcessIO(void){  

    if((usb_device_state < CONFIGURED_STATE)||(UCONbits.SUSPND==1)) return;
	// here enter the code that want to be called periodically, per example interaction with buttons and leds
	
}//end ProcessIO



/******************************************************************************
 * Function:        UserLoopBackRelease(byte i)
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

void UserLoopBackRelease(byte i){
	unsetHandlerReceiveBuffer(i);
	unsetHandlerReceiveFunction(i);
	removePoolingFunction(&UserLoopBackProcessIO);
}


/******************************************************************************
 * Function:        UserLoopBackReceived(byte* recBuffPtr, byte len)
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

void UserLoopBackReceived(byte* recBuffPtr, byte len){
	  byte index;
	  char mens[9] = "LBACK :)";	
      byte UserLoopBackCounter = 0;
	  for (UserLoopBackCounter=0 ; UserLoopBackCounter < len ; UserLoopBackCounter++){
	  	*(sendBufferUsrLoopback+UserLoopBackCounter)= *(recBuffPtr+UserLoopBackCounter); // TODO pensar algo mas eficiente
	  }
	  UserLoopBackCounter=len; //por las dudas
	  /*
	  //para que devuelva el doble de lo que recibe
	  for (UserLoopBackCounter=0 ; UserLoopBackCounter < len ; UserLoopBackCounter++){
	  	*(sendBufferUsrLoopback+(UserLoopBackCounter+len))= *(recBuffPtr+UserLoopBackCounter); // TODO pensar algo mas eficiente
	  }
      UserLoopBackCounter=len*2; //por las dudas
	  */
	  
      if(UserLoopBackCounter != 0)
      {
          if(!mUSBGenTxIsBusy())
              USBGenWrite2(usrLoopbackHandler, UserLoopBackCounter);
      }//end if  	  	
}//end UserLoopBackReceived

/** EOF usr_skeleton.c ***************************************************************/
