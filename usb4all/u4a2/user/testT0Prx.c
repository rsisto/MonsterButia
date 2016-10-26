/* Author             									  Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Rafael Fernandez								         17/06/07    Original.
 *****************************************************************************/

/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include "system\typedefs.h"
#include "system\usb\usb.h"
#include "user\testT0Prx.h"
#include "io_cfg.h"              // I/O pin mapping
#include "user\handlerManager.h"
//#include "dynamicPolling.h"                              
#include "usb4all\proxys\T0Proxy.h"

/** V A R I A B L E S ********************************************************/
#pragma udata 

byte  testT0PrxHandler;	 // Handler number asigned to the module
byte* sendBufferTestT0Prx; // buffer to send data
// GLOBAL VARIALES THAT CHANGE IN ISR MUST BE DECLARATED WITH VOLATILE!!
volatile byte estadoLed2;

/** P R I V A T E  P R O T O T Y P E S ***************************************/
void TestT0PrxProcessIO(void);
void TestT0PrxInit(byte i);
void TestT0PrxReceived(byte*, byte);
void TestT0PrxRelease(byte i);
void TestT0Configure(void);

// Table used by te framework to get a fixed reference point to the user module functions defined by the framework 
/** USER MODULE REFERENCE*****************************************************/
#pragma romdata user
uTab testT0PrxModuleTable = {&TestT0PrxInit,&TestT0PrxRelease,&TestT0Configure,"testTpx"}; //modName must be less or equal 8 characters
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

void TestT0PrxInit(byte i){
	BOOL res;
	testT0PrxHandler = i;
	// add my receive function to the handler module, to be called automatically when the pc sends data to the user module
	setHandlerReceiveFunction(testT0PrxHandler,&TestT0PrxReceived);
	

	// add my receive ISR function to the dynamic pooling module, to be called periodically 
	//TODO inicializar T0Proxy en otro lado
	initT0Functions();
	//configurar T0Proxy
	configT0(1, 1); 
	res = addT0Function(&TestT0PrxProcessIO);
	//initialize ISR Resource, for example a timer and set it interrupt bits.
	
	mLED_1_On();
	//TODO inicializar timer
	
	//TODO ponerlo en el modulo de ISR y que se ponga cuando hay un tipo colgado y deshabilitar cuando no quedan mas
	estadoLed2 = 0;
	// initialize the send buffer, used to send data to the PC
	sendBufferTestT0Prx = getSharedBuffer(testT0PrxHandler);
	//TODO return res value 
}//end UserSkelInit

/******************************************************************************
 * Function:        TestT0Configure(void)
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
void TestT0Configure(void){
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

void TestT0PrxProcessIO(void){  

    //if((usb_device_state < CONFIGURED_STATE)||(UCONbits.SUSPND==1)) return;
	// module ISR routine.
	// VARIABLES modificable with ISR are defines with volatile
	// clear used Interrupt FLAG
		mLED_2_Toggle();
		/*
		if (estadoLed == 0) {
			estadoLed=1;
			mLED_2_On();
		}	
		else {
			estadoLed=0;
			mLED_2_Off();
		}
		*/	
		
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

void TestT0PrxRelease(byte i){
	
	//apago timer
	//TODO ponerlo en el modulo de ISR y que se ponga cuando hay un tipo colgado y deshabilitar cuando no quedan mas
	//INTCONbits.GIE = 0; //apago las interrupciones globales
	
	unsetHandlerReceiveBuffer(i);
	unsetHandlerReceiveFunction(i);
	removeT0Function(&TestT0PrxProcessIO);
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

void TestT0PrxReceived(byte* recBuffPtr, byte len){
	  byte index;
	  char mens[9] = "testISRP";	
      byte testT0PrxCounter = 0;
      switch(((TESTT0PRX_DATA_PACKET*)recBuffPtr)->CMD)
      {
        case READ_VERSION:
              //dataPacket._byte[1] is len
			  ((TESTT0PRX_DATA_PACKET*)sendBufferTestT0Prx)->_byte[0] = ((TESTT0PRX_DATA_PACKET*)recBuffPtr)->_byte[0]; 
			  ((TESTT0PRX_DATA_PACKET*)sendBufferTestT0Prx)->_byte[1] = ((TESTT0PRX_DATA_PACKET*)recBuffPtr)->_byte[1]; 
              ((TESTT0PRX_DATA_PACKET*)sendBufferTestT0Prx)->_byte[2] = TESTT0PRX_MINOR_VERSION;
              ((TESTT0PRX_DATA_PACKET*)sendBufferTestT0Prx)->_byte[3] = TESTT0PRX_MAJOR_VERSION;
              testT0PrxCounter=0x04;
              break;  
	          
        /*case ANY_COMMAND:
              fill the SKEL_DATA_PACKET acording to the data to be sent
			  userSkelCounter=size of data to be sent;
              break;  
        */ 
          case RESET:
              Reset();
			  break;
		  
		 case MESS:
				sendMes(mens, sizeof(mens));
              break;
         
		 default:
              break;
      }//end switch(s)
      if(testT0PrxCounter != 0)
      {
          if(!mUSBGenTxIsBusy())
              USBGenWrite2(testT0PrxHandler, testT0PrxCounter);
      }//end if  	  	
}//end UserSkelReceived

/** EOF testT0Prx.c ***************************************************************/
