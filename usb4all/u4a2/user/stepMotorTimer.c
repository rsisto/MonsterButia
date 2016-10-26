/* 
	jvisca@fing.edu.uy
*/


/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include "system\typedefs.h"
#include "system\usb\usb.h"
#include "testT0Service.h"
#include "usb4all\proxys\T0Service.h"
#include "io_cfg.h"              // I/O pin mapping
#include "user\handlerManager.h"
#include "stepMotorTimer.h"
//#include "dynamicPolling.h"
//#include "usb4all\proxys\T0Proxy.h"

/** V A R I A B L E S ********************************************************/
#pragma udata 
volatile Motor motorA;
volatile Motor motorB;

byte  MotorTmrHandler;	  // Handler number asigned to the module
byte* sendBufferMotorTmr; // buffer to send data

// GLOBAL VARIALES THAT CHANGE IN ISR MUST BE DECLARATED WITH VOLATILE!!

/** P R I V A T E  P R O T O T Y P E S ***************************************/
void MotorTmrInit(byte i);
void MotorAStepEvent();
void MotorBStepEvent();
void MotorRegulationEvent();
void MotorTmrRelease(byte);
void MotorTmrConfigure(void);
void MotorTmrReceived(byte*, byte);

// Table used by te framework to get a fixed reference point to the user module functions defined by the framework 
/** USER MODULE REFERENCE*****************************************************/
#pragma romdata user
uTab MotorTmrModuleTable = {&MotorTmrInit,&MotorTmrRelease,&MotorTmrConfigure,"motorTm"}; //modName must be less or equal 8 characters
#pragma code

/** D E C L A R A T I O N S **************************************************/
#pragma code module

void setupMotors(void){
	motorA.phasetype = FULL_STEP;
	motorA.phaseposition = 0;
	motorA.power_on = POWER_ON;
	motorA.direction = DIR_FORWARD;
	motorA.targetdirection = DIR_FORWARD;
	motorA.stepswaiting = 0;
	motorA.targetdelaytime = max_step_length;
	motorA.delaytime = max_step_length;

	motorB.phasetype = FULL_STEP;
	motorB.phaseposition = 0;
	motorB.power_on = POWER_ON;
	motorB.direction = DIR_FORWARD;
	motorA.targetdirection = DIR_FORWARD;
	motorB.stepswaiting = 0;
	motorB.targetdelaytime = max_step_length;
	motorB.delaytime = max_step_length;
}

void MotorTmrInit(byte i){
	//mLED_1_On();
	//mLED_2_On();

	MotorTmrHandler = i;
	// initialize the send buffer, used to send data to the PC
	sendBufferMotorTmr = getSharedBuffer(MotorTmrHandler);

	setHandlerReceiveFunction(MotorTmrHandler,&MotorTmrReceived);

	setupMotors();
	registerT0event(motorA.delaytime, &MotorAStepEvent);
	registerT0event(motorB.delaytime, &MotorBStepEvent);
	registerT0event(step_regulation_delay, &MotorRegulationEvent);
}

void stepMotor(const Motor *motor){
	motor->phaseposition += motor->direction;
	if(motor->phasetype == WAVE_STEP || motor->phasetype == FULL_STEP){
		//if the phase type is wave or full step mode
		//then move on 2 phases
		motor->phaseposition += motor->direction;
	}

	//wrap up 8 phases
	if(motor->phaseposition >= 8){
		motor->phaseposition = 0;
	} else if(motor->phaseposition < 0){
		motor->phaseposition = 7;
	}
	
	if (motor->stepswaiting>0) {
		motor->stepswaiting--;
		if (motor->stepswaiting==0) {
			motor->direction=DIR_STOP;
		}
	}
}

void regulateMotor(const Motor *motor) {
	if (motor->direction == motor->targetdirection){
		//misma direccion
		if (motor->delaytime < motor->targetdelaytime) {
			//acelerar
			motor->delaytime += step_regulation_increment;
			if (motor->delaytime > motor->targetdelaytime) {
				motor->delaytime = motor->targetdelaytime;
			}
		} else if (motor->delaytime > motor->targetdelaytime) {
			//frenar
			motor->delaytime -= step_regulation_increment;
			if (motor->delaytime < motor->targetdelaytime) {
				motor->delaytime = motor->targetdelaytime;
			} 
		} //else mLED_3_Toggle();
	} else if (motor->direction != motor->targetdirection) {
		//cambio de direccion
		//frenamos en un paso
		motor->delaytime = max_step_length;
		motor->direction = motor->targetdirection;
	}
}

void MotorAStepEvent(void) {
	byte filter = mout & 0b11110000; //reset bits for this motor
	if (motorA.power_on==1) {
		mout = filter | phaseArray[motorA.phasetype][motorA.phaseposition]; //apply motor's bits
	} else {
		mout = filter;
	}

	stepMotor(&motorA);
		
	registerT0eventInEvent(motorA.delaytime, &MotorAStepEvent);
}
void MotorBStepEvent(void) {
	byte filter = mout & 0b00001111; //reset bits for this motor
	if (motorB.power_on==1) {
		mout = filter | (phaseArray[motorB.phasetype][motorB.phaseposition]<<4); //apply motor's bits
	} else {
		mout = filter;
	}

	stepMotor(&motorB);
		
	registerT0eventInEvent(motorB.delaytime, &MotorBStepEvent);
}
void MotorRegulationEvent(void) {
	regulateMotor(&motorA);
	regulateMotor(&motorB);

	registerT0eventInEvent(step_regulation_delay, &MotorRegulationEvent);
}
void MotorTmrRelease(byte i){
	//mLED_1_Off();
	unregisterT0event(&MotorAStepEvent);
	unregisterT0event(&MotorBStepEvent);
	unregisterT0event(&MotorRegulationEvent);
}
void MotorTmrConfigure(void){
}

void MotorTmrReceived(byte* recBuffPtr, byte len){
	char mens[9] = "stepmots";	
	byte MotorTmrCounter = 0;
//mLED_1_Toggle();
	//byte motor_id;
	//mLED_1_On();
	//mLED_2_On();

	switch(((MOTORTMR_DATA_PACKET*)recBuffPtr)->CMD)
	{
	case READ_VERSION:
		//dataPacket._byte[1] is len
		((MOTORTMR_DATA_PACKET*)sendBufferMotorTmr)->_byte[0] = ((MOTORTMR_DATA_PACKET*)recBuffPtr)->_byte[0]; 
		((MOTORTMR_DATA_PACKET*)sendBufferMotorTmr)->_byte[1] = ((MOTORTMR_DATA_PACKET*)recBuffPtr)->_byte[1]; 
		((MOTORTMR_DATA_PACKET*)sendBufferMotorTmr)->_byte[2] = MOTOR_TIMER_MINOR_VERSION;
		((MOTORTMR_DATA_PACKET*)sendBufferMotorTmr)->_byte[3] = MOTOR_TIMER_MAJOR_VERSION;
		MotorTmrCounter=0x04;
		break;

	case T0CFG:
		((MOTORTMR_DATA_PACKET*)sendBufferMotorTmr)->_byte[0] = ((MOTORTMR_DATA_PACKET*)recBuffPtr)->_byte[0];
		T0CON = ((MOTORTMR_DATA_PACKET*)recBuffPtr)->_byte[1];
		MotorTmrCounter=0x1;	
		break; 

	case STEP_A:
//mLED_4_Toggle();
		((MOTORTMR_DATA_PACKET*)sendBufferMotorTmr)->_byte[0] = ((MOTORTMR_DATA_PACKET*)recBuffPtr)->_byte[0]; 
	
		motorA.targetdelaytime=((unsigned int)(((MOTORTMR_DATA_PACKET*)recBuffPtr)->_byte[1]) << 8)
			| (((MOTORTMR_DATA_PACKET*)recBuffPtr)->_byte[2]);

		MotorTmrCounter=0x1;
		break;

	case STEP_B:

		((MOTORTMR_DATA_PACKET*)sendBufferMotorTmr)->_byte[0] = ((MOTORTMR_DATA_PACKET*)recBuffPtr)->_byte[0]; 
	
		motorB.targetdelaytime=((unsigned int)(((MOTORTMR_DATA_PACKET*)recBuffPtr)->_byte[1]) << 8)
			| (((MOTORTMR_DATA_PACKET*)recBuffPtr)->_byte[2]);

		MotorTmrCounter=0x1;
		break;

	case STEPS_A:
		((MOTORTMR_DATA_PACKET*)sendBufferMotorTmr)->_byte[0] = ((MOTORTMR_DATA_PACKET*)recBuffPtr)->_byte[0]; 

		motorA.stepswaiting =((unsigned int)(((MOTORTMR_DATA_PACKET*)recBuffPtr)->_byte[1]) << 8)
			| (((MOTORTMR_DATA_PACKET*)recBuffPtr)->_byte[2]);

		MotorTmrCounter=0x1;
		break;

	case STEPS_B:
		((MOTORTMR_DATA_PACKET*)sendBufferMotorTmr)->_byte[0] = ((MOTORTMR_DATA_PACKET*)recBuffPtr)->_byte[0]; 

		motorB.stepswaiting =((unsigned int)(((MOTORTMR_DATA_PACKET*)recBuffPtr)->_byte[1]) << 8)
			| (((MOTORTMR_DATA_PACKET*)recBuffPtr)->_byte[2]);

		MotorTmrCounter=0x1;
		break;

	case PHASETYPE_A:
		((MOTORTMR_DATA_PACKET*)sendBufferMotorTmr)->_byte[0] = ((MOTORTMR_DATA_PACKET*)recBuffPtr)->_byte[0]; 

		motorA.phasetype = ((MOTORTMR_DATA_PACKET*)recBuffPtr)->_byte[1];

		MotorTmrCounter=0x1;
		break;

	case PHASETYPE_B:
		((MOTORTMR_DATA_PACKET*)sendBufferMotorTmr)->_byte[0] = ((MOTORTMR_DATA_PACKET*)recBuffPtr)->_byte[0]; 

		motorB.phasetype = ((MOTORTMR_DATA_PACKET*)recBuffPtr)->_byte[1];

		MotorTmrCounter=0x1;
		break;

	case POWERON_A:
		((MOTORTMR_DATA_PACKET*)sendBufferMotorTmr)->_byte[0] = ((MOTORTMR_DATA_PACKET*)recBuffPtr)->_byte[0]; 

		motorA.power_on = ((MOTORTMR_DATA_PACKET*)recBuffPtr)->_byte[1];

		MotorTmrCounter=0x1;
		break;

	case POWERON_B:
		((MOTORTMR_DATA_PACKET*)sendBufferMotorTmr)->_byte[0] = ((MOTORTMR_DATA_PACKET*)recBuffPtr)->_byte[0]; 

		motorB.power_on = ((MOTORTMR_DATA_PACKET*)recBuffPtr)->_byte[1];

		MotorTmrCounter=0x1;
		break;

	case DIR_A:
		((MOTORTMR_DATA_PACKET*)sendBufferMotorTmr)->_byte[0] = ((MOTORTMR_DATA_PACKET*)recBuffPtr)->_byte[0]; 

		motorA.targetdirection = ((MOTORTMR_DATA_PACKET*)recBuffPtr)->_byte[1];
		if (!(motorA.targetdirection==0||motorA.targetdirection==1)) motorA.targetdirection=-1;

		MotorTmrCounter=0x1;
		break;

	case DIR_B:
		((MOTORTMR_DATA_PACKET*)sendBufferMotorTmr)->_byte[0] = ((MOTORTMR_DATA_PACKET*)recBuffPtr)->_byte[0]; 

		motorB.targetdirection = ((MOTORTMR_DATA_PACKET*)recBuffPtr)->_byte[1];
		if (!(motorB.targetdirection==0||motorB.targetdirection==1)) motorB.targetdirection=-1;

		MotorTmrCounter=0x1;
		break;

	case MAX_STEP:

		((MOTORTMR_DATA_PACKET*)sendBufferMotorTmr)->_byte[0] = ((MOTORTMR_DATA_PACKET*)recBuffPtr)->_byte[0]; 
	
		max_step_length=((unsigned int)(((MOTORTMR_DATA_PACKET*)recBuffPtr)->_byte[1]) << 8)
			| (((MOTORTMR_DATA_PACKET*)recBuffPtr)->_byte[2]);

		MotorTmrCounter=0x1;
		break;

	case REGULATION_DELAY:

		((MOTORTMR_DATA_PACKET*)sendBufferMotorTmr)->_byte[0] = ((MOTORTMR_DATA_PACKET*)recBuffPtr)->_byte[0]; 
	
		step_regulation_delay=((unsigned int)(((MOTORTMR_DATA_PACKET*)recBuffPtr)->_byte[1]) << 8)
			| (((MOTORTMR_DATA_PACKET*)recBuffPtr)->_byte[2]);

		MotorTmrCounter=0x1;
		break;

	case REGULATION_INC:

		((MOTORTMR_DATA_PACKET*)sendBufferMotorTmr)->_byte[0] = ((MOTORTMR_DATA_PACKET*)recBuffPtr)->_byte[0]; 
	
		step_regulation_increment=((unsigned int)(((MOTORTMR_DATA_PACKET*)recBuffPtr)->_byte[1]) << 8)
			| (((MOTORTMR_DATA_PACKET*)recBuffPtr)->_byte[2]);

		MotorTmrCounter=0x1;
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

	if(MotorTmrCounter!= 0)
	{
		if(!mUSBGenTxIsBusy()) {
			USBGenWrite2(MotorTmrHandler, MotorTmrCounter);
			//mLED_4_On();
		}
	}//end if  	  	
}//end MotorTmrReceived


/** EOF stepMotorTmr.c ***************************************************************/
