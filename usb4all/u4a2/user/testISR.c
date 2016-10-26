/* Author             									  Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Rafael Fernandez								         14/06/07    Original.
 *****************************************************************************/

/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include "system\typedefs.h"
#include "system\usb\usb.h"
#include "user\testISR.h"
#include "io_cfg.h"              // I/O pin mapping
#include "user\handlerManager.h"
//#include "dynamicPolling.h"                              
#include "usb4all\dynamicISR\dynamicISR.h"

/** V A R I A B L E S ********************************************************/
#pragma udata 

byte  testISRHandler;	 // Handler number asigned to the module
byte* sendBufferTestISR; // buffer to send data
// GLOBAL VARIALES THAT CHANGE IN ISR MUST BE DECLARATED WITH VOLATILE!!
volatile byte estadoLed;

/** P R I V A T E  P R O T O T Y P E S ***************************************/
void TestISRProcessIO(void);
void TestISRInit(byte i);
void TestISRReceived(byte*, byte);
void TestISRRelease(byte i);
void userISRConfigure(void);

// Table used by te framework to get a fixed reference point to the user module functions defined by the framework 
/** USER MODULE REFERENCE*****************************************************/
#pragma romdata user
uTab testISRModuleTable = {&TestISRInit,&TestISRRelease,&userISRConfigure,"testISR"}; //modName must be less or equal 8 characters
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

void TestISRInit(byte i){
	BOOL res;
	testISRHandler = i;
	// add my receive function to the handler module, to be called automatically when the pc sends data to the user module
	setHandlerReceiveFunction(testISRHandler,&TestISRReceived);
	// add my receive ISR function to the dynamic pooling module, to be called periodically 
	res = addISRFunction(&TestISRProcessIO);
	//initialize ISR Resource, for example a timer and set it interrupt bits.
	
	mInitAllLEDs(); //debugueo con leds
	mLED_3_On();
	//TODO inicializar timer
	
	//inicializo timer
	T0CON = 0x03; 
	TMR0L = 1;
	TMR0H = 1;
	T0CONbits.TMR0ON  = TRUE; //prendo el timer	
	INTCONbits.TMR0IF = 0;
	INTCONbits.TMR0IE = 1; //prendo las interrupciones del timer 0 
	//TODO ponerlo en el modulo de ISR y que se ponga cuando hay un tipo colgado y deshabilitar cuando no quedan mas
	estadoLed = 0;
	// initialize the send buffer, used to send data to the PC
	sendBufferTestISR = getSharedBuffer(testISRHandler);
	//TODO return res value 
}//end UserSkelInit

/******************************************************************************
 * Function:        UserISRConfigure(void)
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
void userISRConfigure(void){
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

void TestISRProcessIO(void){  

    //if((usb_device_state < CONFIGURED_STATE)||(UCONbits.SUSPND==1)) return;
	// module ISR routine.
	// VARIABLES modificable with ISR are defines with volatile
	// clear used Interrupt FLAG
	if(	INTCONbits.TMR0IF){
		INTCONbits.TMR0IF = 0; 
		//inicializo denuevo el timer
		T0CON = 0x03; 
		TMR0L = 1;
		TMR0H = 1;
		INTCONbits.TMR0IE = 1; //prendo las interrupciones del timer 0 
		T0CONbits.TMR0ON  = TRUE;
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
	}	
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

void TestISRRelease(byte i){
	
	//apago timer
	T0CON = 0x03; 
	TMR0L = 1;
	TMR0H = 1;
	INTCONbits.TMR0IF = 0;
	T0CONbits.TMR0ON  = FALSE;
	INTCONbits.TMR0IE = 0; //apago las interrupciones del timer 0 
	//TODO ponerlo en el modulo de ISR y que se ponga cuando hay un tipo colgado y deshabilitar cuando no quedan mas
	//INTCONbits.GIE = 0; //apago las interrupciones globales
	
	unsetHandlerReceiveBuffer(i);
	unsetHandlerReceiveFunction(i);
	removeISRFunction(&TestISRProcessIO);
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

void TestISRReceived(byte* recBuffPtr, byte len){
	  byte index;
	  char mens[9] = "testISR";	
      byte testISRCounter = 0;
      switch(((TESTISR_DATA_PACKET*)recBuffPtr)->CMD)
      {
        case READ_VERSION:
              //dataPacket._byte[1] is len
			  ((TESTISR_DATA_PACKET*)sendBufferTestISR)->_byte[0] = ((TESTISR_DATA_PACKET*)recBuffPtr)->_byte[0]; 
			  ((TESTISR_DATA_PACKET*)sendBufferTestISR)->_byte[1] = ((TESTISR_DATA_PACKET*)recBuffPtr)->_byte[1]; 
              ((TESTISR_DATA_PACKET*)sendBufferTestISR)->_byte[2] = TESTISR_MINOR_VERSION;
              ((TESTISR_DATA_PACKET*)sendBufferTestISR)->_byte[3] = TESTISR_MAJOR_VERSION;
              testISRCounter=0x04;
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
      if(testISRCounter != 0)
      {
          if(!mUSBGenTxIsBusy())
              USBGenWrite2(testISRHandler, testISRCounter);
      }//end if  	  	
}//end UserSkelReceived

/** EOF usr_skeleton.c ***************************************************************/
