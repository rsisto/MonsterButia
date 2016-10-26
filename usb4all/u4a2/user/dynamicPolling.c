/* Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Rafael Fernandez    27/03/07 	Original
 * Andres Aguirre      03/04/07
 ********************************************************************/

/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include "system\typedefs.h"                        // Required
#include "system\usb\usb.h"                         // Required
#include "io_cfg.h"                                 // Required

#include "dynamicPolling.h"                              // Modifiable

#include "system\usb\usb_compile_time_validation.h" // Optional
//#include "user\user.h" 
#include "user\handlerManager.h"                              // Modifiable


/** V A R I A B L E S ********************************************************/
#pragma udata
void (*pollingFunction[MAX_POLLING_FUNCTIONS]) (void) ;//arreglo de punteros a las funcioens received de los modulos

/** P R I V A T E  P R O T O T Y P E S ***************************************/

#pragma code sys


void initPollingFunctions(void){
	byte i;
	for (i=0; i<MAX_POLLING_FUNCTIONS;i++){
		pollingFunction[i] = 0; //supongo 0 = null
	}							
}

BOOL addPollingFunction(void (*pollingFun) (void)){
	byte i = 0;
	BOOL termine = FALSE;
	while (i<MAX_POLLING_FUNCTIONS && !termine){
		if ( pollingFunction[i] == 0) { //TODO ojo que cambie != por ==
			termine = TRUE;
			pollingFunction[i] = pollingFun;
		}
		i++;
	}
	if (!termine) return FALSE;
	return TRUE;
} 		

BOOL removePoolingFunction(void (*pollingFun) (void)){
	byte i=0;
	BOOL termine=FALSE;
	while (i<MAX_POLLING_FUNCTIONS && !termine){
		if ( pollingFunction[i] == pollingFun) {
			termine = TRUE;
			pollingFunction [i] = 0;
		}
		i++;
	}
	if (!termine) return FALSE;
	return TRUE;
}

void polling(void){
	byte i=0;
	while (i<MAX_POLLING_FUNCTIONS){
		//mLED_4_On();
		if(pollingFunction[i] != 0){ //tuve que bajar esta condicion del while al if, porque cuando me remueven pueden quedar "huecos"
			pollingFunction[i]();
		}
		i++;
	}
}

