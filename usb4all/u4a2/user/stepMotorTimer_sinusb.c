/* Author             									  Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Andres Aguirre								         24/06/07    Original.
 * Andres Aguirre								         24/06/07    Deshardcodeo deconfiguracion del timer.	
 *****************************************************************************/

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

// GLOBAL VARIALES THAT CHANGE IN ISR MUST BE DECLARATED WITH VOLATILE!!

/** P R I V A T E  P R O T O T Y P E S ***************************************/
void MotorTmrInit(byte i);
void MotorAStepEvent();
void MotorBStepEvent();
void MotorRegulationEvent();
void MotorTmrRelease(byte);
void MotorTmrConfigure(void);


// Table used by te framework to get a fixed reference point to the user module functions defined by the framework 
/** USER MODULE REFERENCE*****************************************************/
#pragma romdata user
uTab MotorTmrModuleTable = {&MotorTmrInit,&MotorTmrRelease,&MotorTmrConfigure,"motorTm"}; //modName must be less or equal 8 characters
#pragma code

/** D E C L A R A T I O N S **************************************************/
#pragma code module

void setupMotors(void){
	motorA.phasetype = 1;	//set motor to full stepping moode
	motorA.phaseposition = 0;	//set initial position to 0
	motorA.stepswaiting = 0;	//set steps waiting to 0
	motorA.targetdelaytime = STEP_MIN;
	motorA.delaytime = STEP_MAX;

	motorB.phasetype = 1;	//set motor to full stepping moode
	motorB.phaseposition = 0;	//set initial position to 0
	motorB.stepswaiting = 0;	//set steps waiting to 0
	motorB.targetdelaytime = STEP_MIN;
	motorB.delaytime = STEP_MAX;
}

void MotorTmrInit(byte i){
	mLED_1_On();
	//mLED_2_On();

	setupMotors();
	registerT0event(motorA.delaytime, &MotorAStepEvent);
	registerT0event(motorB.delaytime, &MotorBStepEvent);
	registerT0event(STEP_REGULATION_DELAY, &MotorRegulationEvent);
}

void stepMotor(Motor *motor){
	if(motor->phasetype == 0 || motor->phasetype == 1){
		//if the phase type is wave or full step mode
		//then move on 2 phases
		motor->phaseposition = motor->phaseposition + 2;
	}
	if(motor->phasetype == 2){
		//if the phase type is half stepping then just move on 1 phase
		motor->phaseposition = motor->phaseposition +1;
	}

	if(motor->phaseposition >= 8){
		//there are only 8 phase positions so if we exceed this
		//wrap the position back to 0
		motor->phaseposition = 0;

	}
}

void regulateMotor(Motor *motor) {
	if (motor->delaytime < motor->targetdelaytime) {
		motor->delaytime += STEP_REGULATION_INCREMENT;
		if (motor->delaytime > motor->targetdelaytime) {
			motor->delaytime = motor->targetdelaytime;
		}
	} else if (motor->delaytime > motor->targetdelaytime) {
		motor->delaytime -= STEP_REGULATION_INCREMENT;
		if (motor->delaytime < motor->targetdelaytime) {
			motor->delaytime = motor->targetdelaytime;
		}
	}
}

void MotorAStepEvent(void) {
	moutA1 = phaseArray[motorA.phasetype][motorA.phaseposition][0];
	moutA2 = phaseArray[motorA.phasetype][motorA.phaseposition][1];
	moutA3 = phaseArray[motorA.phasetype][motorA.phaseposition][2];
	moutA4 = phaseArray[motorA.phasetype][motorA.phaseposition][3];

	stepMotor(&motorA);
		
	registerT0eventInEvent(motorA.delaytime, &MotorAStepEvent);
}
void MotorBStepEvent(void) {
	moutB1 = phaseArray[motorB.phasetype][motorB.phaseposition][0];
	moutB2 = phaseArray[motorB.phasetype][motorB.phaseposition][1];
	moutB3 = phaseArray[motorB.phasetype][motorB.phaseposition][2];
	moutB4 = phaseArray[motorB.phasetype][motorB.phaseposition][3];

	stepMotor(&motorB);
		
	registerT0eventInEvent(motorB.delaytime, &MotorBStepEvent);
}
void MotorRegulationEvent(void) {
	regulateMotor(&motorA);
	regulateMotor(&motorB);

	registerT0eventInEvent(STEP_REGULATION_DELAY, &MotorRegulationEvent);
}
void MotorTmrRelease(byte i){
	//mLED_1_Off();
}
void MotorTmrConfigure(void){
}

/** EOF stepMotorISR.c ***************************************************************/
