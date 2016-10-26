/* Author             	Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Andrés Aguirre       20/05/07    Original.
 *****************************************************************************/

/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include <usart.h>
#include "system\typedefs.h"
#include "system\usb\usb.h"
#include "user\stepMotor.h"
#include "io_cfg.h"              // I/O pin mapping
#include "user\handlerManager.h"
#include "dynamicPolling.h"                              


/** V A R I A B L E S ********************************************************/
#pragma udata 

const rom byte PATTERN[4] ={0x10,0x20,0x40,0x80};  // 1-Phase drive or full step drive pattern modifidable
char Direction = 0;
short speed = 3000;
short MotorCount = 0; // cuenta relacionada con la velocidad del motor
char PatPtr=0; 	 
short stepsToLive = 0;
short steps = 0;
BOOL packetMade = FALSE;
byte  motorHandler;	 // Handler number asigned to the module
byte* sendBufferMotor; // buffer to send data

/** P R I V A T E  P R O T O T Y P E S ***************************************/
void MotorProcessIO(void);
void MotorInit(byte i);
void MotorReceived(byte*, byte);
void MotorRelease(byte i);
void UpdateMotor(byte Dir);
void userMotorConfigure(void);

// Table used by te framework to get a fixed reference point to the user module functions defined by the framework 
/** USER MODULE REFERENCE*****************************************************/
#pragma romdata user
uTab motorModuleTable = {&MotorInit,&MotorRelease,&userMotorConfigure,"motor"}; 
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

void MotorInit(byte i){
	BOOL res;
	motorHandler = i;
	// add my receive function to the handler module, to be called automatically when the pc sends data to the user module
	setHandlerReceiveFunction(motorHandler,&MotorReceived);
	// add my receive pooling function to the dynamic pooling module, to be called periodically 
	res = addPollingFunction(&MotorProcessIO);
	// initialize the send buffer, used to send data to the PC
	sendBufferMotor = getSharedBuffer(motorHandler);
	mInitMotor(); //TODO cambiar al configure
	//TODO return res value 
}//end UserSkelInit

/******************************************************************************
 * Function:        UserMotorConfigure(void)
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
void userMotorConfigure(void){
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

void MotorProcessIO(void){  

    //if((usb_device_state < CONFIGURED_STATE)||(UCONbits.SUSPND==1)) return;
	UpdateMotor(Direction);
	
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

void MotorRelease(byte i){
	unsetHandlerReceiveBuffer(i);
	unsetHandlerReceiveFunction(i);
	removePoolingFunction(&MotorProcessIO);
	mReleaseMotor();//TODO hacer en funcion del confiure
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

void MotorReceived(byte* recBuffPtr, byte len){
	  byte index;
	  char mens[9] = "ping";	
      byte motorCounter = 0;
      switch(((MOTOR_DATA_PACKET*)recBuffPtr)->CMD)
      {
        case READ_VERSION:
              //dataPacket._byte[1] is len
			  ((MOTOR_DATA_PACKET*)sendBufferMotor)->_byte[0] = ((MOTOR_DATA_PACKET*)recBuffPtr)->_byte[0]; 
			  ((MOTOR_DATA_PACKET*)sendBufferMotor)->_byte[1] = ((MOTOR_DATA_PACKET*)recBuffPtr)->_byte[1]; 
              ((MOTOR_DATA_PACKET*)sendBufferMotor)->_byte[2] = MOTOR_MINOR_VERSION;
              ((MOTOR_DATA_PACKET*)sendBufferMotor)->_byte[3] = MOTOR_MAJOR_VERSION;
              motorCounter=0x04;
              break;  
	          
        case MOTOR_ON:            
             stepsToLive = steps;
			 Direction = ((MOTOR_DATA_PACKET*)recBuffPtr)->_byte[1];   // 0= stop , 1 = turn left, 2 = turn right
			 ((MOTOR_DATA_PACKET*)sendBufferMotor)->_byte[0] = ((MOTOR_DATA_PACKET*)recBuffPtr)->_byte[0]; 
             //dataPacket._byte[0] is MOTOR_ON command
             //mLED_1_Toggle();
          	 motorCounter=0x0;
			 packetMade = TRUE;
           	 break;           
        
		case SPEED:
			 ((MOTOR_DATA_PACKET*)sendBufferMotor)->_byte[0] = ((MOTOR_DATA_PACKET*)recBuffPtr)->_byte[0]; 
			 ((byte *)&speed)[0] = ((MOTOR_DATA_PACKET*)recBuffPtr)->_byte[1]; //low byte 
			 ((byte *)&speed)[1] = ((MOTOR_DATA_PACKET*)recBuffPtr)->_byte[2]; //high byte
			  motorCounter=0x1;
           	  break;
		
		case STEPS:
			 ((MOTOR_DATA_PACKET*)sendBufferMotor)->_byte[0] = ((MOTOR_DATA_PACKET*)recBuffPtr)->_byte[0]; 
			 ((byte *)&steps)[0] = ((MOTOR_DATA_PACKET*)recBuffPtr)->_byte[1]; //steps low byte
			 ((byte *)&steps)[1] = ((MOTOR_DATA_PACKET*)recBuffPtr)->_byte[2]; //steps high byte 	
			  motorCounter=0x1;
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
      if(motorCounter != 0)
      {
          if(!mUSBGenTxIsBusy())
              USBGenWrite2(motorHandler, motorCounter);
      }//end if  	  	
}//end UserSkelReceived

void UpdateMotor(byte Dir){ 	      
	 if (Dir!=STOP && stepsToLive!=0){
	 	if (Dir==LEFT){
		 	if (MotorCount==0){
			 	MotorCount=speed;
		 		PatPtr--;
				stepsToLive--;
		 		if (PatPtr < 0) PatPtr = 3;	 //TODO cambiar a operacion mod	 	
		 		MotorPort = PATTERN[PatPtr];  
		 	}
		 	MotorCount--;
	 	}
	 	else{
		 	if (Dir==RIGHT){
			 	if (MotorCount==0){
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
		if (MotorCount==0){
		 	MotorCount=speed;

			MotorPort = 0;
			if(packetMade){
				packetMade = FALSE;
				if(!mUSBGenTxIsBusy()){
              		USBGenWrite2(motorHandler, 0x01);
				}
			}
		}
		MotorCount--;
	}	 	 
}	


/** EOF usr_skeleton.c ***************************************************************/
