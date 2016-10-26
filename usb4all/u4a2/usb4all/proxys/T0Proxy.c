/* Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Rafael Fernandez      17/06/07 	Original
 * Andrés Aguirre Dorelo 19/04/09   Parametrización de configuración
 ********************************************************************/

/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include "system\typedefs.h"                        // Required
#include "system\usb\usb.h"                         // Required
#include "io_cfg.h"                                 // Required

#include "usb4all\dynamicISR\dynamicISR.h"
#include "t0Proxy.h"                              // Modifiable

#include "system\usb\usb_compile_time_validation.h" // Optional
//#include "user\user.h" 
//#include "user\handlerManager.h"                              // Modifiable

/** V A R I A B L E S ********************************************************/
#pragma udata
volatile void ( *t0Function[MAX_T0_FUNCTIONS]) (void) ;//arreglo de punteros a las funciones ISR de los modulos que usan Timer0
volatile byte t0Listeners;
volatile byte t0LoadHigh;
volatile byte t0LoadLow;
/** P R I V A T E  P R O T O T Y P E S ***************************************/

#pragma code sys


void initT0Functions(void){
	byte i;
	for (i=0; i<MAX_T0_FUNCTIONS;i++){
		t0Function[i] = 0; //supongo 0 = null
	}
	t0Listeners=0;							
}

BOOL addT0Function(volatile void (*ISRFun) (void)){
	byte i = 0;
	BOOL termine = FALSE;
	while (i<MAX_T0_FUNCTIONS && !termine){
		if ( t0Function[i] == 0) { //TODO ojo que cambie != por ==
			termine = TRUE;
			t0Function[i] = ISRFun;
		}
		i++;
	}
	if (!termine) return FALSE;
	if ((t0Listeners++)==0){
		// add my receive ISR function to the dynamic pooling module, to be called periodically 
		addISRFunction(&t0Interrupt);
		INTCONbits.TMR0IF = 0; //apago bandera de interrupcion de timer0 por las dudas
		INTCONbits.TMR0IE = 1; //cuando se agrega la primer funcion listener prendo ints de Timer0 
		//cargo valores
		TMR0L = t0LoadLow;
		TMR0H = t0LoadHigh;
		T0CONbits.TMR0ON  = TRUE; //prendo timer0
		//mLED_3_On(); raro, no anda el debug
	}
	return TRUE;
} 		

BOOL removeT0Function(volatile void (*ISRFun) (void)){
	byte i=0;
	BOOL termine=FALSE;
	while (i<MAX_T0_FUNCTIONS && !termine){
		if ( t0Function[i] == ISRFun) {
			termine = TRUE;
			t0Function [i] = 0;
		}
		i++;
	}
	if (!termine) return FALSE;
	if ((--t0Listeners)==0) {
		T0CONbits.TMR0ON  = FALSE; //apago timer 0
		INTCONbits.TMR0IF = 0; //apago bandera de interrupcion de timer0 por las dudas
		INTCONbits.TMR0IE = 0; //cuando se quita la ultima funcion listener apagago ints de Timer0 
		removeISRFunction(&t0Interrupt);
		//mLED_3_Off(); raro, no anda el debug
	}
	return TRUE;
}

void configT0(unsigned char t0_low, unsigned char t0_high){
	T0CON = 0x02; //TODO parametrizarlo también
	//t0LoadLow = 1;
	//t0LoadHigh = 1;
	t0LoadLow = t0_low;
	t0LoadHigh = t0_high;
}

void t0Interrupt(void){
	volatile byte i=0;
	if (INTCONbits.TMR0IF){
		//si hubo interrupcion del timer0 apago la bandera de interrucion y hago notify a todos los observers
		INTCONbits.TMR0IF = 0;
		//vuelvo a setear el timer
		TMR0L = t0LoadLow;
		TMR0H = t0LoadHigh;
		while (i<MAX_T0_FUNCTIONS){
			//mLED_4_On();
			if(t0Function[i] != 0){ //tube que bajar esta condicion del while al if, porque cuando me remueven pueden quedar "huecos"
				t0Function[i]();
			}
			i++;
		}
	}
}

