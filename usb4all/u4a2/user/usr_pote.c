/* Author             									  Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *Andres Aguirre					       30/08/09    Original.
 *****************************************************************************/

/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include <usart.h>
#include "system\typedefs.h"
#include "system\usb\usb.h"
#include "user\usr_pote.h"
#include "io_cfg.h"              // I/O pin mapping
#include "user\handlerManager.h"
#include "dynamicPolling.h"                              


/** V A R I A B L E S ********************************************************/
#pragma udata 

byte  usrPoteHandler;	 // Handler number asigned to the module
byte* sendBufferUsrPote; // buffer to send data

/** P R I V A T E  P R O T O T Y P E S ***************************************/
void UserPoteProcessIO(void);
void UserPoteInit(byte i);
void UserPoteReceived(byte*, byte);
void UserPoteRelease(byte i);
void UserPoteConfigure(void);
void ReadPOT(void);

// Table used by te framework to get a fixed reference point to the user module functions defined by the framework 
/** USER MODULE REFERENCE*****************************************************/
#pragma romdata user
uTab userPoteModuleTable = {&UserPoteInit,&UserPoteRelease,&UserPoteConfigure,"pote"}; //modName must be less or equal 8 characters
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

void UserPoteInit(byte i){
	BOOL res;
	usrPoteHandler = i;
	// add my receive function to the handler module, to be called automatically when the pc sends data to the user module
	setHandlerReceiveFunction(usrPoteHandler,&UserPoteReceived);
	// add my receive pooling function to the dynamic pooling module, to be called periodically 
	//res = addPollingFunction(&UserPoteProcessIO);
	// initialize the send buffer, used to send data to the PC
	sendBufferUsrPote = getSharedBuffer(usrPoteHandler);
	//TODO return res value 
	mInitPOT();
	ADCON2bits.ADFM = 1;   // ADC result right justified
}//end UserPoteInit

void ReadPOT(void)
{
    ADCON0bits.GO = 1;              // Start AD conversion
    while(ADCON0bits.NOT_DONE);     // Wait for conversion
    return;
}//end ReadPOT

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
void UserPoteConfigure(void){
// Do the configuration
}

/******************************************************************************
 * Function:        UserPoteProcessIO(void)
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

void UserPoteProcessIO(void){  

    if((usb_device_state < CONFIGURED_STATE)||(UCONbits.SUSPND==1)) return;
	// here enter the code that want to be called periodically, per example interaction with buttons and leds
}//end ProcessIO



/******************************************************************************
 * Function:        UserPoteRelease(byte i)
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

void UserPoteRelease(byte i){
	unsetHandlerReceiveBuffer(i);
	unsetHandlerReceiveFunction(i);
	removePoolingFunction(&UserPoteProcessIO);
}


/******************************************************************************
 * Function:        UserPoteReceived(byte* recBuffPtr, byte len)
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

void UserPoteReceived(byte* recBuffPtr, byte len){
	  byte index;
	  char mens[9] = "User Skeleton is alive";	
      byte userPoteCounter = 0;
      switch(((POTE_DATA_PACKET*)recBuffPtr)->CMD)
      {
        case READ_VERSION:
              //dataPacket._byte[1] is len
			  ((POTE_DATA_PACKET*)sendBufferUsrPote)->_byte[0] = ((POTE_DATA_PACKET*)recBuffPtr)->_byte[0]; 
			  ((POTE_DATA_PACKET*)sendBufferUsrPote)->_byte[1] = ((POTE_DATA_PACKET*)recBuffPtr)->_byte[1]; 
              ((POTE_DATA_PACKET*)sendBufferUsrPote)->_byte[2] = POTE_MINOR_VERSION;
              ((POTE_DATA_PACKET*)sendBufferUsrPote)->_byte[3] = POTE_MAJOR_VERSION;
              userPoteCounter = 0x04;
              break;  
	          
         case RD_POT:
              ReadPOT();
			  ((POTE_DATA_PACKET*)sendBufferUsrPote)->_byte[0] = ((POTE_DATA_PACKET*)recBuffPtr)->_byte[0]; 
			  ((POTE_DATA_PACKET*)sendBufferUsrPote)->_byte[1] = ADRESL; 	
              ((POTE_DATA_PACKET*)sendBufferUsrPote)->_byte[2] = ADRESH;
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
              USBGenWrite2(usrPoteHandler, userPoteCounter);
      }//end if  	  	
}//end UserAmarilloReceived

/** EOF usr_led_amarillo.c ***************************************************************/