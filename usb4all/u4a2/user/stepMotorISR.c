/* Author             									  Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Andres Aguirre								         24/06/07    Original.
 * Andres Aguirre								         24/06/07    Deshardcodeo deconfiguracion del timer.	
 *****************************************************************************/

/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include "system\typedefs.h"
#include "system\usb\usb.h"
#include "stepMotorISR.h"
#include "io_cfg.h"              // I/O pin mapping
#include "user\handlerManager.h"
//#include "dynamicPolling.h"                              
#include "usb4all\proxys\T0Proxy.h"

/** V A R I A B L E S ********************************************************/
#pragma udata 

byte  MotorISRHandler;	  // Handler number asigned to the module
byte* sendBufferMotorISR; // buffer to send data


//const rom volatile byte PATTERN[4] ={0x10,0x20,0x40,0x80};  // 1-Phase drive or full step drive pattern modifidable
byte volatile PATTERN[4] ={0x10,0x20,0x40,0x80};  // 1-Phase drive or full step drive pattern modifidable
char volatile Direction = 0;
short volatile speed = 0;
short volatile MotorCount = 0; // cuenta relacionada con la velocidad del motor
char volatile PatPtr=0; 	 
short volatile stepsToLive = 0;
short volatile steps = 0;
BOOL volatile packetMade = FALSE;
//byte  motorHandler;	 // Handler number asigned to the module
//byte* sendBufferMotor; // buffer to send data

// GLOBAL VARIALES THAT CHANGE IN ISR MUST BE DECLARATED WITH VOLATILE!!
volatile byte estadoLed2Mot;

/** P R I V A T E  P R O T O T Y P E S ***************************************/
void MotorISRProcessIO(void);
void MotorISRInit(byte i);
void MotorISRReceived(byte*, byte);
void MotorISRRelease(byte i);
void MotorISRConfigure(void);

// Table used by te framework to get a fixed reference point to the user module functions defined by the framework 
/** USER MODULE REFERENCE*****************************************************/
#pragma romdata user
uTab MotorISRModuleTable = {&MotorISRInit,&MotorISRRelease,&MotorISRConfigure,"motorin"}; //modName must be less or equal 8 characters
#pragma code

/** D E C L A R A T I O N S **************************************************/
#pragma code module

/******************************************************************************
 * Function:        MotorISRInit(void)
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

void MotorISRInit(byte i){
	BOOL res;
	MotorISRHandler = i;
	// add my receive function to the handler module, to be called automatically when the pc sends data to the user module
	setHandlerReceiveFunction(MotorISRHandler,&MotorISRReceived);
	mInitMotor();
	// add my receive ISR function to the dynamic pooling module, to be called periodically 
	//TODO inicializar T0Proxy en otro lado
	initT0Functions();
	//configurar T0Proxy
	configT0(1,1);  //1,1 es el valor por defecto que se venia usando
	res = addT0Function(&MotorISRProcessIO);
	//initialize ISR Resource, for example a timer and set it interrupt bits.
	
	mInitAllLEDs(); //debugueo con leds
	mLED_1_On();
	//TODO inicializar timer
	
	//TODO ponerlo en el modulo de ISR y que se ponga cuando hay un tipo colgado y deshabilitar cuando no quedan mas
	estadoLed2Mot = 0;
	// initialize the send buffer, used to send data to the PC
	sendBufferMotorISR = getSharedBuffer(MotorISRHandler);
	//TODO return res value 
}//end UserSkelInit

/******************************************************************************
 * Function:        MotorISRConfigure(void)
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
void MotorISRConfigure(void){
// Do the configuration
	mLED_2_Toggle();
}

/******************************************************************************
 * Function:        MotorISRProcessIO(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is registered in the dinamic ISR, who call ir periodically to process the IO interaction
 *					int the PIC, also it can comunicate things to the pc by the USB	
 *
 * Note:            None
 *****************************************************************************/

void MotorISRProcessIO(void){  
/*
	PatPtr--;
	if (PatPtr < 0) PatPtr = 3;	 //TODO cambiar a operacion mod	 	
 		MotorPort = PATTERN[PatPtr];  
*/
	 //mLED_2_Toggle();
	 if (Direction!=STOP && stepsToLive!=0){
	 //	mLED_2_Toggle();
	 //mLED_4_Toggle();
		if (Direction==LEFT){
		 	if (MotorCount<=0){
			 	MotorCount=speed;
		 		PatPtr--;
				stepsToLive--;
		 		if (PatPtr < 0) PatPtr = 3;	 //TODO cambiar a operacion mod	 	
		 		MotorPort = PATTERN[PatPtr];  
		 	}
		 	MotorCount--;
	 	}
	 	else{
		 	if (Direction==RIGHT){
			 	if (MotorCount<=0){
				 	MotorCount=speed;	
			    	PatPtr++;
					stepsToLive--;
			 		if (PatPtr > 3) PatPtr = 0;	//TODO cambiar a operacion mod	 	
			 		MotorPort = PATTERN[PatPtr];
			 	}
			 	MotorCount--;
		 	}
		}//end else 
	}//end if
	else{
		//mLED_3_Toggle();
		if (MotorCount==0){
		 	MotorCount=speed;
			MotorPort = 0;
			if(packetMade){
				packetMade = FALSE;
				if(!mUSBGenTxIsBusy()){
              		USBGenWrite2(MotorISRHandler, 0x01);
				}
			}
		}
		MotorCount--;
	}	

}//end ProcessIO



/******************************************************************************
 * Function:        MotorISRRelease(byte i)
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

void MotorISRRelease(byte i){
	
	//apago timer
	//TODO ponerlo en el modulo de ISR y que se ponga cuando hay un tipo colgado y deshabilitar cuando no quedan mas
	//INTCONbits.GIE = 0; //apago las interrupciones globales
	
	unsetHandlerReceiveBuffer(i);
	unsetHandlerReceiveFunction(i);
	removeT0Function(&MotorISRProcessIO);
	mReleaseMotor();
}


/******************************************************************************
 * Function:        MotorISRReceived(byte* recBuffPtr, byte len)
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

void MotorISRReceived(byte* recBuffPtr, byte len){
	  byte index;
	  char mens[9] = "testmoti";	
      byte MotorISRCounter = 0;
      switch(((MOTORISR_DATA_PACKET*)recBuffPtr)->CMD)
      {
        case READ_VERSION:
              //dataPacket._byte[1] is len
			  ((MOTORISR_DATA_PACKET*)sendBufferMotorISR)->_byte[0] = ((MOTORISR_DATA_PACKET*)recBuffPtr)->_byte[0]; 
			  ((MOTORISR_DATA_PACKET*)sendBufferMotorISR)->_byte[1] = ((MOTORISR_DATA_PACKET*)recBuffPtr)->_byte[1]; 
              ((MOTORISR_DATA_PACKET*)sendBufferMotorISR)->_byte[2] = MOTOR_ISR_MINOR_VERSION;
              ((MOTORISR_DATA_PACKET*)sendBufferMotorISR)->_byte[3] = MOTOR_ISR_MAJOR_VERSION;
              MotorISRCounter=0x04;
              break;  

        case MOTOR_ON:            
             stepsToLive = steps;
			 Direction = ((MOTORISR_DATA_PACKET*)recBuffPtr)->_byte[1];   // 0= stop , 1 = turn left, 2 = turn right
			 ((MOTORISR_DATA_PACKET*)sendBufferMotorISR)->_byte[0] = ((MOTORISR_DATA_PACKET*)recBuffPtr)->_byte[0]; 
             //dataPacket._byte[0] is MOTOR_ON command
             //mLED_1_Toggle();
          	 MotorISRCounter=0x0;
			 packetMade = TRUE;
           	 break;           
        
		case SPEED:
			 ((MOTORISR_DATA_PACKET*)sendBufferMotorISR)->_byte[0] = ((MOTORISR_DATA_PACKET*)recBuffPtr)->_byte[0]; 
			 ((byte *)&speed)[0] = ((MOTORISR_DATA_PACKET*)recBuffPtr)->_byte[1]; //low byte 
			 ((byte *)&speed)[1] = ((MOTORISR_DATA_PACKET*)recBuffPtr)->_byte[2]; //high byte
			  MotorISRCounter=0x1;
           	  break;
		
		case STEPS:
			 ((MOTORISR_DATA_PACKET*)sendBufferMotorISR)->_byte[0] = ((MOTORISR_DATA_PACKET*)recBuffPtr)->_byte[0]; 
			 ((byte *)&steps)[0] = ((MOTORISR_DATA_PACKET*)recBuffPtr)->_byte[1]; //steps low byte
			 ((byte *)&steps)[1] = ((MOTORISR_DATA_PACKET*)recBuffPtr)->_byte[2]; //steps high byte 	
			  MotorISRCounter=0x1;
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
      if(MotorISRCounter!= 0)
      {
          if(!mUSBGenTxIsBusy())
              USBGenWrite2(MotorISRHandler, MotorISRCounter);
      }//end if  	  	
}//end MotorISRReceived


/** EOF stepMotorISR.c ***************************************************************/
