/* Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Rafael Fernandez    16/06/07 	Original
 * 
 ********************************************************************/

/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include "system\typedefs.h"                        // Required
#include "system\usb\usb.h"                         // Required
#include "io_cfg.h"                                 // Required

#include "dynamicISR.h"                              // Modifiable

#include "system\usb\usb_compile_time_validation.h" // Optional
//#include "user\user.h" 
//#include "user\handlerManager.h"                              // Modifiable


/** V A R I A B L E S ********************************************************/
#pragma udata
volatile void ( *ISRFunction[MAX_ISR_FUNCTIONS]) (void) ;//arreglo de punteros a las funciones ISR de los modulos
volatile byte ISRListeners;
/** P R I V A T E  P R O T O T Y P E S ***************************************/

#pragma code sys


void initISRFunctions(void){
	byte i;
	for (i=0; i<MAX_ISR_FUNCTIONS;i++){
		ISRFunction[i] = 0; //supongo 0 = null
	}
	ISRListeners=0;							
}

BOOL addISRFunction(volatile void (*ISRFun) (void)){
	byte i = 0;
	BOOL termine = FALSE;
	while (i<MAX_ISR_FUNCTIONS && !termine){
		if ( ISRFunction[i] == 0) { //TODO ojo que cambie != por ==
			termine = TRUE;
			ISRFunction[i] = ISRFun;
		}
		i++;
	}
	if (!termine) return FALSE;
	if ((ISRListeners++)==0){
		INTCONbits.GIE = 1; //cuando se agrega la primer funcion listener prendo ints globales 
		//mLED_3_On(); raro, no anda el debug
	}
	return TRUE;
} 		

BOOL removeISRFunction(volatile void (*ISRFun) (void)){
	byte i=0;
	BOOL termine=FALSE;
	while (i<MAX_ISR_FUNCTIONS && !termine){
		if ( ISRFunction[i] == ISRFun) {
			termine = TRUE;
			ISRFunction [i] = 0;
		}
		i++;
	}
	if (!termine) return FALSE;
	if ((--ISRListeners)==0) {
		INTCONbits.GIE = 0; //si se va el ultimo listener apago ints globales 
		//mLED_3_Off(); raro, no anda el debug
	}
	return TRUE;
}
#pragma interrupt interruption save=section(".tmpdata")
void interruption(void){
	volatile byte i=0;
	while (i<MAX_ISR_FUNCTIONS){
		//mLED_4_On();
		if(ISRFunction[i] != 0){ //tube que bajar esta condicion del while al if, porque cuando me remueven pueden quedar "huecos"
			ISRFunction[i]();
		}
		i++;
	}
}

