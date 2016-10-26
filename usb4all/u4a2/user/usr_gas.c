/* Author             									  Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *Andres Aguirre					       30/08/09    Original.
 *****************************************************************************/

/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include <usart.h>
#include "system\typedefs.h"
#include "system\usb\usb.h"
#include "user\usr_gas.h"
#include "io_cfg.h"              // I/O pin mapping
#include "user\handlerManager.h"
#include "dynamicPolling.h"                              


/** V A R I A B L E S ********************************************************/
#pragma udata 

byte  usrGasHandler;	 // Handler number asigned to the module
byte* sendBufferUsrGas; // buffer to send data

/** P R I V A T E  P R O T O T Y P E S ***************************************/
void UserGasProcessIO(void);
void UserGasInit(byte i);
void UserGasReceived(byte*, byte);
void UserGasRelease(byte i);
void UserGasConfigure(void);
void ReadGAS(void);

// Table used by te framework to get a fixed reference point to the user module functions defined by the framework 
/** USER MODULE REFERENCE*****************************************************/
#pragma romdata user
uTab userGasModuleTable = {&UserGasInit,&UserGasRelease,&UserGasConfigure,"gas"}; //modName must be less or equal 8 characters
#pragma code

/** D E C L A R A T I O N S **************************************************/
#pragma code module

/******************************************************************************
 * Function:        UsePoteInit(void)
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

void UserGasInit(byte i){
	BOOL res;
	usrGasHandler = i;
	// add my receive function to the handler module, to be called automatically when the pc sends data to the user module
	setHandlerReceiveFunction(usrGasHandler,&UserGasReceived);
	// add my receive pooling function to the dynamic pooling module, to be called periodically 
	//res = addPollingFunction(&UserGasProcessIO);
	// initialize the send buffer, used to send data to the PC
	sendBufferUsrGas = getSharedBuffer(usrGasHandler);
	//TODO return res value 
	mInitGAS();
	ADCON2bits.ADFM = 1;   // ADC result right justified
}//end UserGasInit

void ReadGAS(void)
{
    ADCON0bits.GO = 1;              // Start AD conversion
    while(ADCON0bits.NOT_DONE);     // Wait for conversion
    return;
}//end ReadGAS

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
void UserGasConfigure(void){
// Do the configuration
}

/******************************************************************************
 * Function:        UserGasProcessIO(void)
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

void UserGasProcessIO(void){  

    if((usb_device_state < CONFIGURED_STATE)||(UCONbits.SUSPND==1)) return;
	// here enter the code that want to be called periodically, per example interaction with buttons and leds
}//end ProcessIO



/******************************************************************************
 * Function:        UserGasRelease(byte i)
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

void UserGasRelease(byte i){
	unsetHandlerReceiveBuffer(i);
	unsetHandlerReceiveFunction(i);
	removePoolingFunction(&UserGasProcessIO);
}


/******************************************************************************
 * Function:        UserGasReceived(byte* recBuffPtr, byte len)
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

void UserGasReceived(byte* recBuffPtr, byte len){
	  byte index;
	  char mens[9] = "User Skeleton is alive";	
      byte userPoteCounter = 0;
      switch(((GAS_DATA_PACKET*)recBuffPtr)->CMD)
      {
        case READ_VERSION:
			  ((GAS_DATA_PACKET*)sendBufferUsrGas)->_byte[0] = ((GAS_DATA_PACKET*)recBuffPtr)->_byte[0]; 
			  ((GAS_DATA_PACKET*)sendBufferUsrGas)->_byte[1] = ((GAS_DATA_PACKET*)recBuffPtr)->_byte[1]; 
              ((GAS_DATA_PACKET*)sendBufferUsrGas)->_byte[2] = GAS_MINOR_VERSION;
              ((GAS_DATA_PACKET*)sendBufferUsrGas)->_byte[3] = GAS_MAJOR_VERSION;
              userPoteCounter = 0x04;
              break;  
	          
         case RD_GAS:
              ReadGAS();
		((GAS_DATA_PACKET*)sendBufferUsrGas)->_byte[0] = ((GAS_DATA_PACKET*)recBuffPtr)->_byte[0]; 
		((GAS_DATA_PACKET*)sendBufferUsrGas)->_byte[1] = ADRESL; 	
		((GAS_DATA_PACKET*)sendBufferUsrGas)->_byte[2] = ADRESH;
              userPoteCounter = 0x03;
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
      if(userPoteCounter != 0)
      {
          if(!mUSBGenTxIsBusy())
              USBGenWrite2(usrGasHandler, userPoteCounter);
      }//end if  	  	
}//end UserAmarilloReceived

/** EOF usr_led_amarillo.c ***************************************************************/
