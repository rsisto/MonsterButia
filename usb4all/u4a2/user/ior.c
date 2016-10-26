/* Author             									  Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Andrés Aguirre									       27/11/08    Original.
 *****************************************************************************/

/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include <usart.h>
#include "system\typedefs.h"
#include "system\usb\usb.h"
#include "user\ior.h"
#include "io_cfg.h"              // I/O pin mapping
#include "user\handlerManager.h"
#include "dynamicPolling.h"                              


/** V A R I A B L E S ********************************************************/
#pragma udata 

byte  usrIORHandler;	 // Handler number asigned to the module
byte* sendBufferUsrIOR; // buffer to send data

/** P R I V A T E  P R O T O T Y P E S ***************************************/
void UserIORProcessIO(void);
void UserIORInit(byte i);
void UserIORReceived(byte*, byte);
void UserIORRelease(byte i);
void UserIORConfigure(void);

// Table used by te framework to get a fixed reference point to the user module functions defined by the framework 
/** USER MODULE REFERENCE*****************************************************/
#pragma romdata user
uTab userIORModuleTable = {&UserIORInit,&UserIORRelease,&UserIORConfigure,"ior"}; 
#pragma code

/** D E C L A R A T I O N S **************************************************/
#pragma code module

/******************************************************************************
 * Function:        UserIORInit(void)
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

void UserIORInit(byte i){
	BOOL res;
	usrIORHandler = i;
	// add my receive function to the handler module, to be called automatically when the pc sends data to the user module
	setHandlerReceiveFunction(usrIORHandler,&UserIORReceived);
	// add my receive pooling function to the dynamic pooling module, to be called periodically 
	//res = addPollingFunction(&UserIORProcessIO);
	// initialize the send buffer, used to send data to the PC
	sendBufferUsrIOR = getSharedBuffer(usrIORHandler);
	mInitIOR_A();
	mInitIOR_BLow();
	mInitIOR_BHigh
	//TODO return res value 
}//end UserIORInit

/******************************************************************************
 * Function:        UserIORConfigure(void)
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
void UserIORConfigure(void){
// Do the configuration
}

/******************************************************************************
 * Function:        UserIORProcessIO(void)
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

void UserIORProcessIO(void){  

    if((usb_device_state < CONFIGURED_STATE)||(UCONbits.SUSPND==1)) return;
	// here enter the code that want to be called periodically, per example interaction with buttons and leds
	
}//end ProcessIO



/******************************************************************************
 * Function:        UserIORRelease(byte i)
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

void UserIORRelease(byte i){
	unsetHandlerReceiveBuffer(i);
	unsetHandlerReceiveFunction(i);
	//removePoolingFunction(&UserIORProcessIO);
	mReleaseIOR_A();
	mReleaseIOR_BLow();
	mReleaseIOR_BHigh();
}


/******************************************************************************
 * Function:        UserIORReceived(byte* recBuffPtr, byte len)
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

void UserIORReceived(byte* recBuffPtr, byte len){
	  byte index, low, high, a, i;
	  char mens[9] = "IORalive";	
      byte userIORCounter = 0;
	  byte iors[2] = {0x00, 0x00}; 	
      switch(((IOR_DATA_PACKET*)recBuffPtr)->CMD)
      {
        case READ_VERSION:
              //dataPacket._byte[1] is len
			  ((IOR_DATA_PACKET*)sendBufferUsrIOR)->_byte[0] = ((IOR_DATA_PACKET*)recBuffPtr)->_byte[0]; 
			  ((IOR_DATA_PACKET*)sendBufferUsrIOR)->_byte[1] = ((IOR_DATA_PACKET*)recBuffPtr)->_byte[1]; 
              ((IOR_DATA_PACKET*)sendBufferUsrIOR)->_byte[2] = IOR_MINOR_VERSION;
              ((IOR_DATA_PACKET*)sendBufferUsrIOR)->_byte[3] = IOR_MAJOR_VERSION;
              userIORCounter=0x04;
              break;  
	          
          case SEND_IOR:
              IORPortBLow = 0x0;  // blanqueo para generar int
			  IORPortBHigh = 0x0; // blanqueo para generar int
              ((IOR_DATA_PACKET*)sendBufferUsrIOR)->_byte[0] = ((IOR_DATA_PACKET*)recBuffPtr)->_byte[0]; 
			  a = ((IOR_DATA_PACKET*)recBuffPtr)->_byte[1]; 
			  IORPortA = a;        
			  a = ((IOR_DATA_PACKET*)recBuffPtr)->_byte[2];
			  userIORCounter=0x01;
			  break;  

          case INTE:
			  IORPortBLow = 0xFF;  // hay datos para consumir
			  IORPortBHigh = 0xFF; // hay datos para consumir
			  for(i=0;i<100;i++);	
			  IORPortBLow = 0x00;  // hay datos para consumir
			  IORPortBHigh = 0x00; // hay datos para consumir
			  userIORCounter=0x01;
			  break;  

          case TEST:
			  IORPortBLow = 0xFF;  // hay datos para consumir
			  IORPortBHigh = 0xFF; // hay datos para consumir
			  userIORCounter=0x01;
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
      if(userIORCounter != 0)
      {
          if(!mUSBGenTxIsBusy())
              USBGenWrite2(usrIORHandler, userIORCounter);
      }//end if  	  	
}//end UserIORReceived

/** EOF usr_IOReton.c ***************************************************************/
