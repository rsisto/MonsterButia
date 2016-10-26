/* Author             									  Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Andrés Aguirre, Rafael Fernandez, Carlos Grossy       02/06/07    Original.
 *****************************************************************************/

/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include <usart.h>
#include "system\typedefs.h"
#include "system\usb\usb.h"
#include "user\manchester.h"
#include "io_cfg.h"              // I/O pin mapping
#include "user\handlerManager.h"
#include "dynamicPolling.h"                              
#define CLOCK_PATTERN 0xAAAA

/** V A R I A B L E S ********************************************************/
#pragma udata 

byte  usrManchesterHandler;	 // Handler number asigned to the module
byte* sendBufferUsrManchester; // buffer to send data
unsigned short byteManchester;
byte  sendingBit;

/** P R I V A T E  P R O T O T Y P E S ***************************************/
void UserManchesterProcessIO(void);
void UserManchesterInit(byte i);
void UserManchesterReceived(byte*, byte);
void UserManchesterRelease(byte i);
void UserManchesterConfigure(void);
// Table used by te framework to get a fixed reference point to the user module functions defined by the framework 
/** USER MODULE REFERENCE*****************************************************/
#pragma romdata user
uTab userManchesterModuleTable = {&UserManchesterInit,&UserManchesterRelease,&UserManchesterConfigure,"manch"}; //modName must be less or equal 8 characters
#pragma code

/** D E C L A R A T I O N S **************************************************/
#pragma code module

/******************************************************************************
 * Function:        UserManchesterInit(void)
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

void UserManchesterInit(byte i){
	BOOL res;
	usrManchesterHandler = i;
	// add my receive function to the handler module, to be called automatically when the pc sends data to the user module
	setHandlerReceiveFunction(usrManchesterHandler,&UserManchesterReceived);
	// add my receive pooling function to the dynamic pooling module, to be called periodically 
	res = addPollingFunction(&UserManchesterProcessIO);
	// initialize the send buffer, used to send data to the PC
	sendBufferUsrManchester = getSharedBuffer(usrManchesterHandler);
	
	//TODO seteo el pin de salida del manchester como out.
	mInitAllLEDs();
	TRISD &= (255-MANCHESTER_BIT);
	mLED_3_Off();
	mLED_4_Off();
			
	sendingBit=16;
	//TODO return res value 
}//end UserSkelInit

/******************************************************************************
 * Function:        UserManchesterConfigure(void)
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
void UserManchesterConfigure(void){
// Do the configuration
}

/******************************************************************************
 * Function:        UserManchesterProcessIO(void)
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



/*
void UserManchesterProcessIO(void){  

    if((usb_device_state < CONFIGURED_STATE)||(UCONbits.SUSPND==1)) return;
	
	if (INTCONbits.TMR0IF==1){
		//hago algo con el led
		if (sendingBit==0){
			sendingBit=1;
		}
		else {
			sendingBit=0;
		}
		manchesterOut(sendingBit);
		prendoTimer();
	}
	
}
*/

void UserManchesterProcessIO(void){  

    //if((usb_device_state < CONFIGURED_STATE)||(UCONbits.SUSPND==1)) return;
	// here enter the code that want to be called periodically, per example interaction with buttons and leds
	//chequeo si estoy enviando
	if (sendingBit<16){
		//empiezo a enviar o todavia estoy enviando un byteManchester
		if (sendingBit==0){
			mLED_3_On();
			mLED_4_Off();
				
			//envio el bit 0 de manchester y 
			sendCurrentBit(); //mando el bit sendingBit por la patita de out
			prendoTimer(); //prendemos el timer
			sendingBit++;
		}	
		else {
			if (INTCONbits.TMR0IF==1) {
				mLED_4_Toggle();
				mLED_3_Toggle();
				//sendingBit++;
				if (sendingBit<15){
					sendCurrentBit(); //mando el bit sendingBit por la patita de out
					sendingBit++;
					prendoTimer(); //prendemos el timer
				}
				else{
					//terminé de mandar
					sendCurrentBit();
					termineDeMandar();
				}
			} 
		}
	}
}//end ProcessIO


void sendCurrentBit(){
	//TODO envia el bit seteado en sendBit de la variable byteManchester por la pata x del pic
	if ((byteManchester & (0x8000>>sendingBit))>0) manchesterOut(1);
	else manchesterOut(0);
	
}

/******************************************************************************
 * Function:        UserManchesterRelease(byte i)
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

void UserManchesterRelease(byte i){
	unsetHandlerReceiveBuffer(i);
	unsetHandlerReceiveFunction(i);
	removePoolingFunction(&UserManchesterProcessIO);
	TRISD |= MANCHESTER_BIT;
}

void termineDeMandar(){
	apagoTimer();//apago timer
	//envio respuesta pq el modulo Manchester en la pc se queda bloqueado en un receive
	USBGenWrite2(usrManchesterHandler, 0x03);
	
}

void startSendByte(byte sbyte){
	/*IEE 802.3 Aplico metodo con xor que consiste en hacer clock xor mensaje_prop
	donde mensaje prop es construir un mensaje de 2 bytes a partir de uno de 1 byte repitiendo los 
	bits consecutivos, ver http://en.wikipedia.org/wiki/Manchester_code */
	BYTE* sbyte_manch;
    sbyte_manch = (BYTE*)&sbyte;
	((MANCHESTER_DATA_TO_SEND*)&byteManchester)->b0 = sbyte_manch->b0;
	((MANCHESTER_DATA_TO_SEND*)&byteManchester)->b1 = sbyte_manch->b0;
	((MANCHESTER_DATA_TO_SEND*)&byteManchester)->b2 = sbyte_manch->b1;
	((MANCHESTER_DATA_TO_SEND*)&byteManchester)->b3 = sbyte_manch->b1;
	((MANCHESTER_DATA_TO_SEND*)&byteManchester)->b4 = sbyte_manch->b2;
	((MANCHESTER_DATA_TO_SEND*)&byteManchester)->b5 = sbyte_manch->b2;
	((MANCHESTER_DATA_TO_SEND*)&byteManchester)->b6 = sbyte_manch->b3;
	((MANCHESTER_DATA_TO_SEND*)&byteManchester)->b7 = sbyte_manch->b3;
	((MANCHESTER_DATA_TO_SEND*)&byteManchester)->b8 = sbyte_manch->b4;
	((MANCHESTER_DATA_TO_SEND*)&byteManchester)->b9 = sbyte_manch->b4;
	((MANCHESTER_DATA_TO_SEND*)&byteManchester)->b10 = sbyte_manch->b5;
	((MANCHESTER_DATA_TO_SEND*)&byteManchester)->b11 = sbyte_manch->b5;
	((MANCHESTER_DATA_TO_SEND*)&byteManchester)->b12 = sbyte_manch->b6;
	((MANCHESTER_DATA_TO_SEND*)&byteManchester)->b13 = sbyte_manch->b6;
	((MANCHESTER_DATA_TO_SEND*)&byteManchester)->b14 = sbyte_manch->b7;
	((MANCHESTER_DATA_TO_SEND*)&byteManchester)->b15 = sbyte_manch->b7;
    ((MANCHESTER_DATA_TO_SEND*)&byteManchester)->m_data ^= CLOCK_PATTERN;
    //inicializo sendingBit
	sendingBit=0;
}

			  
void startSendByteThomas(byte sbyte){
	/*IEE 802.3 Aplico metodo con xor que consiste en hacer clock xor mensaje_prop
	donde mensaje prop es construir un mensaje de 2 bytes a partir de uno de 1 byte repitiendo los 
	bits consecutivos, ver http://en.wikipedia.org/wiki/Manchester_code */
	BYTE* sbyte_manch;
    sbyte_manch = (BYTE*)&sbyte;
	((MANCHESTER_DATA_TO_SEND*)&byteManchester)->b0 = sbyte_manch->b0;
	((MANCHESTER_DATA_TO_SEND*)&byteManchester)->b1 = sbyte_manch->b0;
	((MANCHESTER_DATA_TO_SEND*)&byteManchester)->b2 = sbyte_manch->b1;
	((MANCHESTER_DATA_TO_SEND*)&byteManchester)->b3 = sbyte_manch->b1;
	((MANCHESTER_DATA_TO_SEND*)&byteManchester)->b4 = sbyte_manch->b2;
	((MANCHESTER_DATA_TO_SEND*)&byteManchester)->b5 = sbyte_manch->b2;
	((MANCHESTER_DATA_TO_SEND*)&byteManchester)->b6 = sbyte_manch->b3;
	((MANCHESTER_DATA_TO_SEND*)&byteManchester)->b7 = sbyte_manch->b3;
	((MANCHESTER_DATA_TO_SEND*)&byteManchester)->b8 = sbyte_manch->b4;
	((MANCHESTER_DATA_TO_SEND*)&byteManchester)->b9 = sbyte_manch->b4;
	((MANCHESTER_DATA_TO_SEND*)&byteManchester)->b10 = sbyte_manch->b5;
	((MANCHESTER_DATA_TO_SEND*)&byteManchester)->b11 = sbyte_manch->b5;
	((MANCHESTER_DATA_TO_SEND*)&byteManchester)->b12 = sbyte_manch->b6;
	((MANCHESTER_DATA_TO_SEND*)&byteManchester)->b13 = sbyte_manch->b6;
	((MANCHESTER_DATA_TO_SEND*)&byteManchester)->b14 = sbyte_manch->b7;
	((MANCHESTER_DATA_TO_SEND*)&byteManchester)->b15 = sbyte_manch->b7;
    ((MANCHESTER_DATA_TO_SEND*)&byteManchester)->m_data ^= CLOCK_PATTERN;
	((MANCHESTER_DATA_TO_SEND*)&byteManchester)->m_data =  ~((MANCHESTER_DATA_TO_SEND*)&byteManchester)->m_data;
    //inicializo sendingBit
	sendingBit=0; 
}


/******************************************************************************
 * Function:        UserManchesterReceived(byte* recBuffPtr, byte len)
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

void UserManchesterReceived(byte* recBuffPtr, byte len){
	  byte index;
	  char mens[9] = "Is Alive";	
      byte userManchesterCounter = 0;
      switch(((MANCHESTER_DATA_PACKET*)recBuffPtr)->CMD)
      {
        case READ_VERSION:
              //dataPacket._byte[1] is len
			  ((MANCHESTER_DATA_PACKET*)sendBufferUsrManchester)->_byte[0] = ((MANCHESTER_DATA_PACKET*)recBuffPtr)->_byte[0]; 
			  ((MANCHESTER_DATA_PACKET*)sendBufferUsrManchester)->_byte[1] = ((MANCHESTER_DATA_PACKET*)recBuffPtr)->_byte[1]; 
              ((MANCHESTER_DATA_PACKET*)sendBufferUsrManchester)->_byte[2] = MANCHESTER_MINOR_VERSION;
              ((MANCHESTER_DATA_PACKET*)sendBufferUsrManchester)->_byte[3] = MANCHESTER_MAJOR_VERSION;
              userManchesterCounter=0x04;
              break;  
	          
        case SENDBYTE:
              //dejo preparado el paquete de respuesta
			  ((MANCHESTER_DATA_PACKET*)sendBufferUsrManchester)->_byte[0] = ((MANCHESTER_DATA_PACKET*)recBuffPtr)->_byte[0]; 
			  ((MANCHESTER_DATA_PACKET*)sendBufferUsrManchester)->_byte[1] = ((MANCHESTER_DATA_PACKET*)recBuffPtr)->_byte[1]; 
              ((MANCHESTER_DATA_PACKET*)sendBufferUsrManchester)->_byte[2] = ACK; 
			  startSendByte(((MANCHESTER_DATA_PACKET*)recBuffPtr)->_byte[2]);
			  userManchesterCounter=0x03;
              break;  
        case SENDBYTE_T:
              //dejo preparado el paquete de respuesta
			  ((MANCHESTER_DATA_PACKET*)sendBufferUsrManchester)->_byte[0] = ((MANCHESTER_DATA_PACKET*)recBuffPtr)->_byte[0]; 
			  ((MANCHESTER_DATA_PACKET*)sendBufferUsrManchester)->_byte[1] = ((MANCHESTER_DATA_PACKET*)recBuffPtr)->_byte[1]; 
              ((MANCHESTER_DATA_PACKET*)sendBufferUsrManchester)->_byte[2] = ACK; 
			  startSendByteThomas(((MANCHESTER_DATA_PACKET*)recBuffPtr)->_byte[2]);
			  userManchesterCounter=0x03;
              break;  
        case DEBUG:
			  //dejo preparado el paquete de respuesta
			  ((MANCHESTER_DATA_PACKET*)sendBufferUsrManchester)->_byte[0] = ((MANCHESTER_DATA_PACKET*)recBuffPtr)->_byte[0]; 
			  ((MANCHESTER_DATA_PACKET*)sendBufferUsrManchester)->_byte[1] = ((MANCHESTER_DATA_PACKET*)recBuffPtr)->_byte[1]; 
              manchesterOut(((MANCHESTER_DATA_PACKET*)recBuffPtr)->_byte[2]);
		 	  ((MANCHESTER_DATA_PACKET*)sendBufferUsrManchester)->_byte[2] = ACK; 
			  userManchesterCounter=0x03;
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
      if(userManchesterCounter != 0)
      {
          if(!mUSBGenTxIsBusy())
              USBGenWrite2(usrManchesterHandler, userManchesterCounter);
      }//end if  	  	
}//end UserManchesterReceived

void manchesterOut(byte valor){
    if (valor==0){
		MANCHESTER_PORT &= (255-MANCHESTER_BIT);
	}
	else {
		MANCHESTER_PORT |= MANCHESTER_BIT;
	} 
}

void prendoTimer(void){
	//inicializo timer
	T0CON = 0x07; 
	TMR0L = 1;
	TMR0H = 1;
	INTCONbits.TMR0IF = 0;
	T0CONbits.TMR0ON  = TRUE;
}

void apagoTimer(void){
	T0CONbits.TMR0ON  = FALSE;
}


/** EOF manchester.c ***************************************************************/
