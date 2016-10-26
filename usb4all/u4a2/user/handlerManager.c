/* Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Rafael Fernandez    10/03/07     Original.
 * Andres Aguirre      27/03/07 	
 ********************************************************************/

/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include <usart.h>
#include "system\typedefs.h"
#include "system\usb\usb.h"
#include "user\defines.h"
#include "user\loaderModule.h"
#include "io_cfg.h"             // I/O pin mapping
#include "user\adminModule.h"
#include "user\handlerManager.h"


/** V A R I A B L E S ********************************************************/
#pragma udata
unsigned char ram_max_ep_number;
epHandlerMapItem epHandlerMap[MAX_HANDLERS];
HM_DATA_PACKET_HEADER hmDataPacketHeader;
byte* HandlerReceiveBuffer[MAX_HANDLERS];
void (*handlerReceivedFuncion[MAX_HANDLERS]) (byte*,byte) ;//arreglo de punteros a las funcioens received de los modulos
					 //TODO hacer un struct junto con los buffers;
HANDLER_OPTYPE hn_opType;
/** P R I V A T E  P R O T O T Y P E S ***************************************/

/** D E C L A R A T I O N S **************************************************/
#pragma code

void setHandlerReceiveBuffer(byte handler, byte *rb){
	HandlerReceiveBuffer[handler] = rb;
}

void unsetHandlerReceiveBuffer(byte handler){
	HandlerReceiveBuffer[handler] = 0; //TODO poner NULL
}

void setHandlerReceiveFunction(byte handler,void (*pf) (byte*,byte)){
	handlerReceivedFuncion[handler] = pf;
}

void unsetHandlerReceiveFunction(byte handler){
	handlerReceivedFuncion[handler] = 0; //TODO poner NULL
}

void checkHandlerManagerIO(void){
    byte dataSize;
    //por ahora hago polling del endpoint, despues el modulo proxy usb
	//me va a llamar con los parametros adecuados luego de mirar el header
	/*
    
    if(USBGenRead((byte*)&hmDataPacket,sizeof(hmDataPacket))){
		//chequeo el header para ver a quien debo enviar la peticion
		//por ahora simplemente llamo a la funcion del user.c
		//despues hay que inicializar arreglo de punteros a funciones.
		dataSize = hmDataPacket.header.pLength;
		//userReceive(hmDataPacket.data,dataSize);
	}
	*/
	
}//end ServiceRequests

void USBGenRead2(void){
	byte usbgen_rx_len = 0;
	byte len; 
	epHandlerMapItem hmi;
	byte ep;
	//byte* buffer; 
	HM_DATA_PACKET_HEADER* dph;
	if((usb_device_state < CONFIGURED_STATE)||(UCONbits.SUSPND==1)) return;
	//TODO: len deberia ser el max mtu del endpoint a leer
	len = PACKET_MTU-1;
	for(ep=1;ep<=ram_max_ep_number;ep++){
		if(!EPOUT_IS_BUSY(ep)){
	        /*
	         * Adjust the expected number of bytes to equal
	         * the actual number of bytes received.
	         */
	        if(len > EPOUT_SIZE(ep))
	        	len = EPOUT_SIZE(ep);
		//antes de copiar el dato en el buffer tengo que mirar de que 
		//handler es y pedir el buffer de receive del modulo de usuario
		dph = (HM_DATA_PACKET_HEADER*)EPBUFFEROUT(ep);
		//TODO chequear que el handler number sea valido, sino hacer algo tipo error
		//buffer=HandlerReceiveBuffer[dph->handlerNumber];
	        //aviso al modulo de usuario respectivo que tengo datos
		//hmi = epHandlerMap[dph->handlerNumber];
		//ep = hmi.ep.EPNum;
		handlerReceivedFuncion[dph->handlerNumber](EPBUFFEROUT(ep)+SIZE__HM_DATA_PACKET_HEADER,len-SIZE__HM_DATA_PACKET_HEADER);
		/*
	         * Prepare dual-ram buffer for next OUT transaction
	         */
		EPOUT_SIZE(ep) = getEPSizeOUT(ep);
	        //mUSBBufferReady(USBGEN_BD_OUT);
		if (ep<3){
			mUSBBufferReady2(EPOUT_BDT(ep));
		}else{
			mUSBBufferReady3(EPOUT_BDT(ep));
		}
	    }//end if
	}    
}//end USBGenRead

void USBGenWrite2(byte handler, byte len) {
	byte i;
	epHandlerMapItem hmi;
	byte ep;
	hmi = epHandlerMap[handler];
	ep = hmi.ep.EPNum;
	/*
	* Value of len should be equal to or smaller than USBGEN_EP_SIZE.
	* This check forces the value of len to meet the precondition.
	*/
	if(len > getEPSizeIN(ep)-SIZE__HM_DATA_PACKET_HEADER)
	    len = getEPSizeIN(ep)-SIZE__HM_DATA_PACKET_HEADER;

	//seteo los datos del header
	hn_opType.handlerNumber=handler;
	hn_opType.operationType=SEND;
	//copio el header en la dual-ram buffer
	EPBUFFERIN(ep)[0] = hn_opType.hn_op;
	EPBUFFERIN(ep)[1] = len+SIZE__HM_DATA_PACKET_HEADER;
	EPBUFFERIN(ep)[2] = 0;
	//usbgen_in[0]=hn_opType.hn_op;
	/*
	* Copy data from user's buffer to dual-ram buffer
	*/
	EPIN_SIZE(ep) = len+SIZE__HM_DATA_PACKET_HEADER;
	//TODO if (!EPIN_IS_BUSY(ep)) { //testear deberia sustituir el if(!mUSBGenTxIsBusy()) en los modulos de user
	/*TODO deshardcodear la invocacion segun el numero de endpoint, hay que hacerlo segun el tipo de endpoint, 
	 *mUSBBufferReady3 solo debe de invocarse para los endpoints interrupt*/
 
	/*segun el tipo de transferencia es el BufferReady que tengo que invocar*/
	if(ep < 3){
		mUSBBufferReady2(EPIN_BDT(ep));
	}
	else{
		mUSBBufferReady3(EPIN_BDT(ep));
	}		
}//end USBGenWrite

byte newHandlerTableEntry(byte endPIn, rom near char* uTableDirection){
	byte i=0;
	while (i<MAX_HANDLERS){
		if (epHandlerMap[i].ep.empty == 1) {
			epHandlerMap[i].ep.endPoint = endPIn;
			epHandlerMap[i].ep.empty = 0;
			epHandlerMap[i].uTableDirection = uTableDirection;
			return i;			
		}
		i++;
	}
	return ERROR;
} 			

BOOL existsTableEntry(rom near char* uTableDirection){
	byte i=0;
	while (i<MAX_HANDLERS){
		if (epHandlerMap[i].uTableDirection == uTableDirection) {
			return TRUE;			
		}
		i++;
	}
	return FALSE;
} 	

void initHandlerTable(){
	byte i;
	endpoint adminEndpoint;
	adminEndpoint = getAdminEndpoint();
	for(i=0;i<MAX_HANDLERS;i++){
		epHandlerMap[i].ep.empty = 1;
		epHandlerMap[i].uTableDirection = 0; // Asumo 0 como NULL TODO revisar
	}
	epHandlerMap[0].ep = adminEndpoint;
	//cargo el ROM_MAX_EP_NUMBER en ram
	ram_max_ep_number = ROM_MAX_EP_NUMBER;
}

void initHandlerManager(void){
	//initHandlerBuffers();
	initHandlerTable();				//Inicializo tabla de mapeo de handler a endpoint en HandlerManager
	adminModuleInit();				//Inicializo el Admin Module 
}

respType removeHandlerTableEntry(byte handler){
	pUserFunc releaseFunction;
	if (handler < MAX_HANDLERS && epHandlerMap[handler].ep.empty == 0){
		epHandlerMap[handler].ep.empty = 1;
		releaseFunction = getModuleReleaseDirection(epHandlerMap[handler].uTableDirection);
		releaseFunction(handler);
		epHandlerMap[handler].uTableDirection = 0;
		return ACK;
	}
	else{
		return NACK;
	}
}

respType configureHandlerTableEntry(byte handler){
	pUserFunc configureFunction;
	if (handler < MAX_HANDLERS && epHandlerMap[handler].ep.empty == 0){
	    configureFunction = getModuleConfigureDirection(epHandlerMap[handler].uTableDirection);
		configureFunction(handler);
		return ACK;
	}
	else{
		return NACK;
	}
}

void removeAllOpenModules(void){
	pUserFunc releaseFunction;
	byte handler = 1;
	while(handler < MAX_HANDLERS){
		if (epHandlerMap[handler].ep.empty == 0){
			epHandlerMap[handler].ep.empty = 1;
		    	releaseFunction = getModuleReleaseDirection(epHandlerMap[handler].uTableDirection);
			releaseFunction(handler);
			epHandlerMap[handler].uTableDirection = 0;
			//return ACK; mal !!!! gracias jorge por darte cuenta
		}
		handler++;
	}
	return ACK;
}

byte* getSharedBuffer(byte handler){
	// esta hecho para modo 0 ping pong
	epHandlerMapItem hmi;
	byte ep;
	hmi = epHandlerMap[handler];
	ep = hmi.ep.EPNum;
	if (handler==0)
		//Se que el admin atiende el endpoint 1 y esta es la forma que tengo de que se pueda inicializar antes
		//el admin que la tabla de BDTs
		return &ep1_in_buffer[SIZE__HM_DATA_PACKET_HEADER];
	else
		return EPBUFFERIN(ep) + SIZE__HM_DATA_PACKET_HEADER;
}

byte getEPSizeOUT(byte ep){
	// TODO deshardcodear el 64 !!
	return 64;  
}


byte getEPSizeIN(byte ep){
	// TODO deshardcodear el 64 !!
	return 64;  
}

/** EOF handlerManager.c ***************************************************************/

