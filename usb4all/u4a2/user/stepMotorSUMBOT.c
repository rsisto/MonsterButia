/* Author             									  Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Gonzalo Tejera, Santiago Margini, Andres Aguirre       17/04/2009  Original.
 *****************************************************************************/

/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include "system\typedefs.h"
#include "system\usb\usb.h"
#include "stepMotorSUMBOT.h"
#include "io_cfg.h"              // I/O pin mapping
#include "user\handlerManager.h"                             
#include "usb4all\proxys\T0Proxy.h" 

/** V A R I A B L E S ********************************************************/
#pragma udata 

#define 	_INS						1
#define _FRENO_SUELTO	1  // 1 - Frena sin energizar bobinas, 0 - Frena energizando bobinas

#define _ACEL_INI		35   // 15
#define _VELZERO 		150

#define _VEL_TORQUE_BAJA_INI 		1     // REVER PORQUE EL VALOR ANTERIOR ERA 5 Y EN REALIDAD LO QUERÍA PONER EN 3 PERO SURGIÓ ESTA PRUEBA

#define _MAXPASO_INI	4
#define _MINPASO_INI	1

#define _ADELANTE 		1
#define _ATRAS			0

#define _TMR1_INI_L		119
#define _TMR1_INI_H		236	



byte  MotorISRHandler;	  // Handler number asigned to the module
byte* sendBufferMotorISR; // buffer to send data

unsigned char ins     = _INS;
unsigned char cuenta;
unsigned char ACEL;
unsigned char paso_puerto1;
unsigned char paso_puerto2;
unsigned char puertoAux;
unsigned char puerto1 		= 1;
unsigned char puerto2 		= 1;
unsigned char velActual_1  	= _VELZERO;
unsigned char velNueva_1   	= _VELZERO;
unsigned char velObjetivo_1	= _VELZERO;


unsigned char tiempoAcel_1 	= 1;
unsigned char paso_1       	= _MAXPASO_INI;
unsigned char tiempoPaso_1 	= 1;
unsigned char dirActual_1  	= _ADELANTE;
unsigned char dirObjetivo_1	= _ADELANTE;


unsigned char velActual_2  	= _VELZERO;
unsigned char velNueva_2   	= _VELZERO;
unsigned char velObjetivo_2	= _VELZERO;
unsigned char tiempoAcel_2 	= 1;
unsigned char paso_2       	= _MAXPASO_INI;
unsigned char tiempoPaso_2 	= 1;
unsigned char dirActual_2  	= _ADELANTE;
unsigned char dirObjetivo_2	= _ADELANTE;

unsigned char TORQUE_1      = 1;
unsigned char TORQUE_2      = 1;
unsigned char VEL_TORQUE_BAJA;
#define TORQUE_FIJO     1  // 1 doble bobina 
						   // 2 microSteeping	
						   // 3 simple bobina
#define ADAPTATIVO		1
#define N0_ADAPTATIVO   0

unsigned char MAXPASO_1;
unsigned char MINPASO_1;
unsigned char MAXPASO_2;
unsigned char MINPASO_2;

unsigned char FRENO_SUELTO = 1;
unsigned char CONTROL_TORQUE = N0_ADAPTATIVO;

BOOL volatile packetMade = FALSE;
char volatile speed_1 = 0;
char volatile speed_2 = 0;

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
uTab MotorISRModuleTable = {&MotorISRInit,&MotorISRRelease,&MotorISRConfigure,"stmtr"}; //modName must be less or equal 8 characters
#pragma code

/** D E C L A R A T I O N S **************************************************/
#pragma code module

//Dada una entrada, devuleve la direccion en la que va el robot.
unsigned char getDireccion(char entrada) {
	unsigned char resultado;
	if (entrada > 0)
		resultado = 1;
	else
		resultado = 0;

	return resultado;
}

//Dada una entrada, devuelve la velocidad en la que va el robot.
unsigned char getVelocidad(char entrada) {
	unsigned char resultado;
	if (entrada < 0)
		resultado = - entrada;
	else
		resultado = entrada; 
	return resultado;	
}

///////////////////////////////////////////////////////////////////////////////
// la funcion invierte los 4 bits menos significativos
// de un byte que tiene los 4 bits mas significativos = 0
unsigned char invierto( unsigned char dato ){
	unsigned char respuesta = 0;

	dato = dato << 4;
	if (dato > 127)
		respuesta = 1;
	dato = dato << 1;
	if (dato > 127)
		respuesta = respuesta + 2;
	dato = dato << 1;
	if (dato > 127)
		respuesta = respuesta + 4;
	dato = dato << 1;
	if (dato > 127)
		respuesta = respuesta + 8;

	return respuesta;
}

//////////////////////////////////////////////////////////////////////////////
// setea la variable que corresponde a un motor u otro dependiendo 
// del valor que recibe por parametro.
// determina si se debe avanzar o retroceder el paso de un motor
// dependiendo de si la
// velocidad actual es mayor que la constante _VELZERO que es la que
// determina el valor de motor detenido.
void doyPaso( unsigned char motor ){

	if (motor == 1){
		if (dirActual_1 == _ADELANTE){
			if (paso_1 == MAXPASO_1){
				paso_1 = MINPASO_1;
				puerto1 = 1;
			}else{
				paso_1++;
				puerto1 = puerto1 * 2;
			}
		}else{ 
			if (paso_1 == MINPASO_1){
				paso_1 = MAXPASO_1;
				puerto1 = 8;
			}
			else{
				paso_1--;
				puerto1 = puerto1 / 2;
			}
		}
	}else {
		if (dirActual_2 == _ADELANTE){
			if (paso_2 == MAXPASO_2){
				paso_2 = MINPASO_2;
				puerto2 = 1;
			}else{
				paso_2++;
				puerto2 = puerto2 * 2;
			}
		}else{ 
			if (paso_2 == MINPASO_2){
				paso_2 = MAXPASO_2;
				puerto2 = 8;
			}
			else{
				paso_2--;
				puerto2 = puerto2 / 2;
			}
		}
	}
}



/////////////////////////////////////////////////////////////////
// el paso adecuado para que el motor energice las bobinas dobles.
void controlTorque(){

	//	MOTOR 1

	switch ( TORQUE_1 ) {
		case 1 :{
			if ( velActual_1 < VEL_TORQUE_BAJA ){
				TORQUE_1  = 3;
			}
			break;
				}
		case 3 :{
			if ( velActual_1 >= VEL_TORQUE_BAJA ){
				TORQUE_1  = 1;
			}
				}
	}

	//	MOTOR 2

	switch ( TORQUE_2 ) {
		case 1 :{
			if ( velActual_2 < VEL_TORQUE_BAJA ){
				TORQUE_2  = 3;
			}
			break;
				}
		case 3 :{
			if ( velActual_2 >= VEL_TORQUE_BAJA ){
				TORQUE_2  = 1;
			}
				}
	}
}

// Esta funcion relaciona el valor del paso en el que esta el calculo
// con el valor del puerto conectado al motor para lograr este paso
// segun el torque actual.
unsigned char pasoDoble( unsigned char paso, unsigned char torque ){
	unsigned char puerto;

	if ( torque == 3 ){
		switch ( paso ) {
			case 1 :{
				puerto = 0b00000001;
				break;
					}
			case 2 :{
				puerto = 0b00000010;
				break;		
					}
			case 3 :{
				puerto = 0b00000100;
				break;		
					}
			case 4 :{
				puerto = 0b00001000;
				break;		
					}
		}
	}
	else {	// torque es 1

		switch ( paso ) {
			case 1 :{
				puerto = 0b00000011;
				break;
					}
			case 2 :{
				puerto = 0b00000110;
				break;		
					}
			case 3 :{
				puerto = 0b00001100;
				break;		
					}
			case 4 :{
				puerto = 0b00001001;
				break;		
					}
		}
	}
	return puerto;
}

///////////////////////////////////////////////////////////////////////////////
// La rutina setea la velocidad nueva y objetivo del motor 1.
int setVelMotor1( unsigned char direccion, unsigned char velocidad ){

	if ( dirActual_1 == direccion ){ 			// dirObjetivo_1 ){
		// es la misma direccion, solo cambio la velocidad.
//		GIE = 0; TODO ver el GIE 
		removeT0Function(&MotorISRProcessIO);
		dirObjetivo_1 = direccion;
		velObjetivo_1 = velocidad;
		velNueva_1    = velocidad;
		addT0Function(&MotorISRProcessIO);
//		GIE = 1;
	} else {
		// se solicita un cambio de direccion, la nueva velocidad es detenerse.
//		GIE = 0;
		removeT0Function(&MotorISRProcessIO);
		dirObjetivo_1 = direccion;
		velObjetivo_1 = velocidad;
		velNueva_1    = _VELZERO;
		addT0Function(&MotorISRProcessIO);
//		GIE = 1;
	}
	return 0;
} 



// La rutina setea la velocidad nueva y objetivo del motor 2.
int setVelMotor2( unsigned char direccion, unsigned char velocidad ){

	if ( dirActual_2 == direccion ){		// dirObjetivo_2 ){
		// es la misma direccion, solo cambio la velocidad.
//		GIE = 0;
		removeT0Function(&MotorISRProcessIO);
		dirObjetivo_2 = direccion;
		velObjetivo_2 = velocidad;
		velNueva_2    = velocidad;
		addT0Function(&MotorISRProcessIO);
//		GIE = 1;
	} else {
		// se solicita un cambio de direccion, la nueva velocidad es detenerse.
//		GIE = 0;
		removeT0Function(&MotorISRProcessIO);
		dirObjetivo_2 = direccion;
		velObjetivo_2 = velocidad;
		velNueva_2    = _VELZERO;
		addT0Function(&MotorISRProcessIO);
//		GIE = 1;
	}
	return 0;
} 

//////////////////////////////////////////////////////////////////
// Mapea las velocidades a los tiempos asignados para cada una.
unsigned char mapeo( unsigned char dato ){
	unsigned char resultado;
	if (dato == 0) {
		resultado = _VELZERO;
	} else {
		resultado = 128 - dato;
	}
	return (resultado);
}


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
	initT0Functions();
	configT0(_TMR1_INI_L, _TMR1_INI_H); 
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
	// inicializo el valor de la aceleracion y del byte mas significativo del Timer1 ( INS )
	ACEL     = _ACEL_INI;

	VEL_TORQUE_BAJA = _VEL_TORQUE_BAJA_INI;

	MAXPASO_1 = _MAXPASO_INI;
	MINPASO_1 = _MINPASO_INI;
	MAXPASO_2 = _MAXPASO_INI;
	MINPASO_2 = _MINPASO_INI;

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
		ins --;

		// Si está habilitado la cuenta de timepo evalua la variable cuenta, sino no	

		// Controla movimiento de motores.
		if (ins == 0){
			ins = _INS;

			//////////////////////////////////////////////////////////////////
			// motor 1

			// ACA SE MANTIENE LA VELOCIDAD ACTUAL
			tiempoPaso_1--;
			if (tiempoPaso_1 == 0){
				tiempoPaso_1 = velActual_1;
				if (velActual_1 != _VELZERO){ 
					doyPaso(1);
				}
				else if (velObjetivo_1 != _VELZERO){
					dirActual_1 = dirObjetivo_1;
					velNueva_1  = velObjetivo_1;
				}
			} 

			// ACA SE CAMBIA LA VELOCIDAD SI CORRESPONDE...
			if (velActual_1 != velNueva_1){
				tiempoAcel_1--;
				if (tiempoAcel_1 == 0){
					if (velActual_1 > velNueva_1)
						velActual_1--;
					else 
						velActual_1++;
					tiempoAcel_1 = _ACEL_INI;
				}
			}


			//////////////////////////////////////////////////////////////////
			//  motor 2

			// ACA SE MANTIENE LA VELOCIDAD ACTUAL
			tiempoPaso_2--;
			if (tiempoPaso_2 == 0){
				tiempoPaso_2 = velActual_2;
				if (velActual_2 != _VELZERO) 
					doyPaso(2);
				else 
					if (velObjetivo_2 != _VELZERO){
						dirActual_2 = dirObjetivo_2;
						velNueva_2  = velObjetivo_2;
					}
			} 

			// ACA SE CAMBIA LA VELOCIDAD SI CORRESPONDE...
			if (velActual_2 != velNueva_2){
				tiempoAcel_2--;
				if (tiempoAcel_2 == 0){
					if (velActual_2 > velNueva_2)
						velActual_2--;
					else 
						velActual_2++;
					tiempoAcel_2 = _ACEL_INI;
				}
			}


			// cambia el valor del paso si cambio el torque.
			// 
			if(CONTROL_TORQUE==ADAPTATIVO) {	
				controlTorque();
			} else { // torque constante
				TORQUE_1=TORQUE_FIJO;
				TORQUE_2=TORQUE_FIJO;
			}

			// Acomoda el valor del puerto de acuerdo con el torque que tiene.

			paso_puerto1 = pasoDoble( paso_1, TORQUE_1 );
			paso_puerto2 = pasoDoble( paso_2, TORQUE_2 );		

			paso_puerto1 = invierto( paso_puerto1 );

			// Controlo si es necesario energizar los motores o los suelto.
			if (( velActual_1 == _VELZERO ) && ( velActual_2 == _VELZERO ) && ( _FRENO_SUELTO == 1 ))
				puertoAux = 0;
			else
				puertoAux = paso_puerto1 + (paso_puerto2 * 16);
		
			MotorPort = puertoAux;		
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
	  byte dir_1;
	  byte dir_2;
	  byte vel_1;
	  byte vel_2;
	  byte t0_l;
	  byte t0_h;
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
        
		case SPEED:
			 ((MOTORISR_DATA_PACKET*)sendBufferMotorISR)->_byte[0] = ((MOTORISR_DATA_PACKET*)recBuffPtr)->_byte[0]; 
			 speed_1 = ((MOTORISR_DATA_PACKET*)recBuffPtr)->_byte[1]; //low byte 
			 speed_2 = ((MOTORISR_DATA_PACKET*)recBuffPtr)->_byte[2]; //high byte
	         
			 // Calculo las direcciones y velocidades...
		     dir_1 = getDireccion(speed_1);
		     vel_1 = getVelocidad(speed_1);

		     dir_2 = getDireccion(speed_2);
		     vel_2 = getVelocidad(speed_2);

			 // Seteo de velocidades en los dos motores.
			 vel_1 = mapeo( vel_1 );
		     setVelMotor1( dir_1, vel_1 );	

			 // La velocidad debe ser mapeada al tiempo real.
		     vel_2 = mapeo( vel_2 );
		     setVelMotor2( dir_2, vel_2 );	
			 MotorISRCounter=0x1;
           	 break;
		case T0CFG:
			 ((MOTORISR_DATA_PACKET*)sendBufferMotorISR)->_byte[0] = ((MOTORISR_DATA_PACKET*)recBuffPtr)->_byte[0];
			 t0_l = ((MOTORISR_DATA_PACKET*)recBuffPtr)->_byte[1];	
			 t0_h = ((MOTORISR_DATA_PACKET*)recBuffPtr)->_byte[2];
			 configT0(t0_l, t0_h);
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
