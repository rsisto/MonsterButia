/* Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Andrés Aguirre       01/04/07    Original.
 *****************************************************************************/

/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include <usart.h>
#include "system\typedefs.h"

#include "system\usb\usb.h"

#include "io_cfg.h"              // I/O pin mapping
#include "user\userTemp2.h"
#include "user\temperature.h"
#include "user\handlerManager.h"
#include "dynamicPolling.h"                              

/** V A R I A B L E S ********************************************************/
#pragma udata 
byte tempCounter2;
byte _trf_state2;
byte _temp_mode2;
byte tempHandler2;
byte old_sw32;

byte* sendBufferTemp2;
//DATA_PACKET tempDataPacket;

byte _pTemp2;                     // Pointer to current logging position, will
                                 // loop to zero once the max index is reached
byte _valid_temp2;                // Keeps count of the valid data points
word _temp_data2[1];              // 30 points of data

// Timer0 - 1 second interval setup.
// Fosc/4 = 12MHz
// Use /256 prescalar, this brings counter freq down to 46,875 Hz
// Timer0 should = 65536 - 46875 = 18661 or 0x48E5
#define TIMER0L_VAL2         0xE5
#define TIMER0H_VAL2         0x48

/** P R I V A T E  P R O T O T Y P E S ***************************************/

void TempProcessIO2(void);
void TempUserTempInit2(void);
void TempResetTempLog2(void);
void ServiceRequests2(void);
void TempUserTempConfigure2(void);

// For board testing purpose only
void PICDEMFSUSBDemoBoardTest(void);

// Tabla para mantener en un lugar fijo una referencia a los modulos que el usuario introduce en USB4all
/** USER MODULE REFERENCE*****************************************************/
#pragma romdata user
uTab user3 = {&TempUserTempInit2,&TempUserTempRelease2,&TempUserTempConfigure2,"2temp"};
#pragma code

/** D E C L A R A T I O N S **************************************************/
#pragma code module

void TempUserInit2(void)
{
    InitTempSensor();
    TempResetTempLog2();
    _temp_mode2 = TEMP_REAL_TIME;
    
    /* Init Timer0 for data logging interval (every 1 second) */
    T0CON = 0b10010111;
    //T0CONbits.T08BIT = 0;       // 16-bit mode
    //T0CONbits.T0CS = 0;         // Select Fosc/4
    //T0CONbits.PSA = 0;          // Assign prescalar (default is /256)
    /* Timer0 is already enabled by default */
	mLED_3_On();
}//end UserInit

void TempUserTempConfigure2(void){
	// Do the configure baby :P
}

BOOL Switch3IsPressed2(void)
{
    if(sw3 == 0)                    
    	return TRUE;                
    return FALSE;                       
}//end Switch3IsPressed


/******************************************************************************
 * Function:        void ProcessIO(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Esta funcion es la que se registra en el pooling, sirve para procesar cosas en el PIC.
 *				    Tambien se puede comunicar cosas mediante USB					
 *
 * Note:            None
 *****************************************************************************/

void TempProcessIO2(void){  
	char mensaje[8] = "Hola2!!";
    if((usb_device_state < CONFIGURED_STATE)||(UCONbits.SUSPND==1)) return;
	//pase el if para el handler module (lo deje aca por las dudas    
    //ServiceRequests();
	//hago como que el sistema invocara el handlerManager
	if (Switch3IsPressed2()){
		mLED_4_On();
		//sendMes(mensaje, sizeof(mensaje));
	}else
		mLED_4_Off();
		
}//end ProcessIO

void TempResetTempLog2(void)
{
    _pTemp2 = 0;
    _valid_temp2 = 0;
}//end ResetLog

void userTempInit2(byte i){
	BOOL res;
	mInitAllLEDs();
    mInitAllSwitches();
	old_sw32 = sw3;
	//inicializacion del sistema
	tempHandler2 = i;
	//agrego mi buffer en el handler module	
//	setHandlerReceiveBuffer(tempHandler, (byte*)&tempDataPacket);
	//agrego mi funcion de Receive en el handler module
	setHandlerReceiveFunction(tempHandler2,&userTempReceived2);
	//agrego la funcion de polling
	res = addPollingFunction(&TempProcessIO2);
	//inicializacion de modulo
	// seteo el buffer para mandar al PC
	sendBufferTemp2 = getSharedBuffer(tempHandler2);
	TempUserInit2();
	//TODO debo retornar res
}

void userTempRelease2(byte i){
	tempHandler2 = i;
	unsetHandlerReceiveBuffer(tempHandler2);
	unsetHandlerReceiveFunction(tempHandler2);
	removePoolingFunction(&TempProcessIO2);
}

void userTempReceived2(byte* recBuffPtr,byte len){
	  byte index;
	  char mens[9] = "UReceive";	
      tempCounter2 = 0;
      switch(((DATA_PACKET2*)recBuffPtr)->CMD)
      {
        case READ_VERSION:
              //dataPacket._byte[1] is len
			  ((DATA_PACKET2*)sendBufferTemp2)->_byte[0] = ((DATA_PACKET2*)recBuffPtr)->_byte[0]; // hecho medio en dope a las 3 am	
			  ((DATA_PACKET2*)sendBufferTemp2)->_byte[1] = ((DATA_PACKET2*)recBuffPtr)->_byte[1]; // hecho medio en dope a las 3 am despues de yuyos	
              ((DATA_PACKET2*)sendBufferTemp2)->_byte[2] = MINOR_VERSION2;
              ((DATA_PACKET2*)sendBufferTemp2)->_byte[3] = MAJOR_VERSION2;
              tempCounter2=0x04;
              break;  

		case SET_TEMP_REAL:
              _temp_mode2 = TEMP_REAL_TIME;
              TempResetTempLog2();
              tempCounter2 = 0x01;
              break;

          case RD_TEMP:
              if(AcquireTemperature()){
				  ((DATA_PACKET2*)sendBufferTemp2)->CMD = RD_TEMP;
                  ((DATA_PACKET2*)sendBufferTemp2)->word_data = temperature._word;
                  tempCounter2=0x03;
              }//end if
              break;

          case SET_TEMP_LOGGING:
              _temp_mode2 = TEMP_LOGGING;
              TempResetTempLog2();
              tempCounter2=0x01;
              break;

          case RD_TEMP_LOGGING:
              tempCounter2 = (_valid_temp2<<1)+2;  // Update count in byte
              ((DATA_PACKET2*)sendBufferTemp2)->len = (_valid_temp2<<1);

              for(index = _valid_temp2; index > 0; index--)
              {
                  if(_pTemp2 == 0)
                      _pTemp2 = 29;
                  else
                      _pTemp2--;
                  ((DATA_PACKET2*)sendBufferTemp2)->_word[index] = _temp_data2[_pTemp2];
              }//end for
              
              TempResetTempLog2();             // Once read, log will restart
              break;
              
          case RESET:
              Reset();
			  break;
		  
		 case MESS:
				sendMes(mens, sizeof(mens));
              break;
         
		 default:
			  mLED_3_Off();
              break;
      }//end switch(s)
      if(tempCounter2 != 0)
      {
          if(!mUSBGenTxIsBusy())
              USBGenWrite2(tempHandler2,/*sendBufferTemp,*/tempCounter2);
      }//end if  	  	
}//end userTempReceived

/** EOF user.c ***************************************************************/
