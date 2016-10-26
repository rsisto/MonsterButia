/* Author             									  Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Andrés Aguirre, Rafael Fernandez					       03/11/07    Original.
 *****************************************************************************/

/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include <usart.h>
#include "system\typedefs.h"
#include "system\usb\usb.h"
#include "user\dac.h"
#include "io_cfg.h"              // I/O pin mapping
#include "user\handlerManager.h"
#include "dynamicPolling.h"                              


/** V A R I A B L E S ********************************************************/
#pragma udata 

byte  usrDACHandler;	 // Handler number asigned to the module
byte* sendBufferUsrDAC; // buffer to send data

/** P R I V A T E  P R O T O T Y P E S ***************************************/
void UserDACProcessIO(void);
void UserDACInit(byte i);
void UserDACReceived(byte*, byte);
void UserDACRelease(byte i);
void UserDACConfigure(void);

// Table used by te framework to get a fixed reference point to the user module functions defined by the framework 
/** USER MODULE REFERENCE*****************************************************/
#pragma romdata user
uTab userDACModuleTable = {&UserDACInit,&UserDACRelease,&UserDACConfigure,"dac"}; 
#pragma code

/** D E C L A R A T I O N S **************************************************/
#pragma code module

/******************************************************************************
 * Function:        UserDACInit(void)
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

void UserDACInit(byte i){
	BOOL res;
	usrDACHandler = i;
	// add my receive function to the handler module, to be called automatically when the pc sends data to the user module
	setHandlerReceiveFunction(usrDACHandler,&UserDACReceived);
	// add my receive pooling function to the dynamic pooling module, to be called periodically 
	//res = addPollingFunction(&UserDACProcessIO);
	// initialize the send buffer, used to send data to the PC
	sendBufferUsrDAC = getSharedBuffer(usrDACHandler);
	mInitDAC_A();
	mInitDAC_BLow();
	mInitDAC_BHigh
	//TODO return res value 
}//end UserDACInit

/******************************************************************************
 * Function:        UserDACConfigure(void)
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
void UserDACConfigure(void){
// Do the configuration
}

/******************************************************************************
 * Function:        UserDACProcessIO(void)
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

void UserDACProcessIO(void){  

    if((usb_device_state < CONFIGURED_STATE)||(UCONbits.SUSPND==1)) return;
	// here enter the code that want to be called periodically, per example interaction with buttons and leds
	
}//end ProcessIO



/******************************************************************************
 * Function:        UserDACRelease(byte i)
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

void UserDACRelease(byte i){
	unsetHandlerReceiveBuffer(i);
	unsetHandlerReceiveFunction(i);
	//removePoolingFunction(&UserDACProcessIO);
	mReleaseDAC_A();
	mReleaseDAC_BLow();
	mReleaseDAC_BHigh();
}


/******************************************************************************
 * Function:        UserDACReceived(byte* recBuffPtr, byte len)
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

void UserDACReceived(byte* recBuffPtr, byte len){
	  byte index, low, high, a;
	  char mens[9] = "DACalive";	
      byte userDACCounter = 0;
	  byte dacs[2] = {0x00, 0x00}; 	
      switch(((DAC_DATA_PACKET*)recBuffPtr)->CMD)
      {
        case READ_VERSION:
              //dataPacket._byte[1] is len
			  ((DAC_DATA_PACKET*)sendBufferUsrDAC)->_byte[0] = ((DAC_DATA_PACKET*)recBuffPtr)->_byte[0]; 
			  ((DAC_DATA_PACKET*)sendBufferUsrDAC)->_byte[1] = ((DAC_DATA_PACKET*)recBuffPtr)->_byte[1]; 
              ((DAC_DATA_PACKET*)sendBufferUsrDAC)->_byte[2] = DAC_MINOR_VERSION;
              ((DAC_DATA_PACKET*)sendBufferUsrDAC)->_byte[3] = DAC_MAJOR_VERSION;
              userDACCounter=0x04;
              break;  
	          
          case SEND_DAC:
              ((DAC_DATA_PACKET*)sendBufferUsrDAC)->_byte[0] = ((DAC_DATA_PACKET*)recBuffPtr)->_byte[0]; 
			  a = ((DAC_DATA_PACKET*)recBuffPtr)->_byte[1]; 
			  //high = ((a / 64) << 6);
			  //low  = (a % 64);
			  //DACPortA = high | (low >> 1);
			  DACPortA = a;	
			  a = ((DAC_DATA_PACKET*)recBuffPtr)->_byte[2];
			  low  = (a % 16);
			  high = ((a / 16) << 4);
			  DACPortBLow  = low >> 1;
			  DACPortBHigh = high;
			  userDACCounter=0x01;
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
      if(userDACCounter != 0)
      {
          if(!mUSBGenTxIsBusy())
              USBGenWrite2(usrDACHandler, userDACCounter);
      }//end if  	  	
}//end UserDACReceived

/** EOF usr_DACeton.c ***************************************************************/
