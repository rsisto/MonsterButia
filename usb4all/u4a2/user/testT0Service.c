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
//#include "dynamicPolling.h"                              
//#include "usb4all\proxys\T0Proxy.h"

/** V A R I A B L E S ********************************************************/
#pragma udata 
//volatile byte cont = 0;

// GLOBAL VARIALES THAT CHANGE IN ISR MUST BE DECLARATED WITH VOLATILE!!

/** P R I V A T E  P R O T O T Y P E S ***************************************/
void TestT0ServiceInit(byte i);
void TestT0Event();
void TestT0Event2();
void TestT0ServiceRelease(byte);
void TestT0ServiceConfigure(void);


// Table used by te framework to get a fixed reference point to the user module functions defined by the framework 
/** USER MODULE REFERENCE*****************************************************/
#pragma romdata user
uTab TestT0ServiceModuleTable = {&TestT0ServiceInit,&TestT0ServiceRelease,&TestT0ServiceConfigure,"testT0S"}; //modName must be less or equal 8 characters
#pragma code

/** D E C L A R A T I O N S **************************************************/
#pragma code module

void TestT0ServiceInit(byte i){
	//mInitAllLEDs(); //debugueo con leds
	mLED_1_On();
	mLED_2_On();
	registerT0event(2000, &TestT0Event);
	registerT0event(3500, &TestT0Event2);
}

void TestT0Event(void) {
	mLED_1_Toggle();
		
	registerT0eventInEvent(2000, &TestT0Event);
}
void TestT0Event2(void) {
	//mLED_1_Toggle();
	mLED_2_Toggle();
		
	registerT0eventInEvent(3500, &TestT0Event2);
}

void TestT0ServiceRelease(byte i){
	//mLED_1_Off();
}
void TestT0ServiceConfigure(void){
}

/** EOF stepMotorISR.c ***************************************************************/
