/* Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Andr�s Aguirre       01/04/07    Original.
 *****************************************************************************/

/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include <usart.h>
#include "system\typedefs.h"

#include "system\usb\usb.h"

#include "io_cfg.h"              // I/O pin mapping
#include "user\userTemp.h"
#include "user\temperature.h"
#include "user\handlerManager.h"
#include "dynamicPolling.h"                              

/** V A R I A B L E S ********************************************************/
#pragma udata 
byte tempCounter;
byte _trf_state;
byte _temp_mode;
byte tempHandler;
byte old_sw3;

byte* sendBufferTemp;
//DATA_PACKET tempDataPacket;

byte _pTemp;                     // Pointer to current logging position, will
                                 // loop to zero once the max index is reached
byte _valid_temp;                // Keeps count of the valid data points
word _temp_data[1];              // 30 points of data

// Timer0 - 1 second interval setup.
// Fosc/4 = 12MHz
// Use /256 prescalar, this brings counter freq down to 46,875 Hz
// Timer0 should = 65536 - 46875 = 18661 or 0x48E5
#define TIMER0L_VAL         0xE5
#define TIMER0H_VAL         0x48

/** P R I V A T E  P R O T O T Y P E S ***************************************/

void TempProcessIO(void);
void TempUserInit(void);
void TempResetTempLog(void);
void ServiceRequests(void);
void userTempConfigure(void);

// Tabla para mantener en un lugar fijo una referencia a los modulos que el usuario introduce en USB4all
/** USER MODULE REFERENCE*****************************************************/
#pragma romdata user
uTab user1 = {&userTempInit,&userTempRelease,&userTempConfigure,"temp"};
#pragma code

// For board testing purpose only
void PICDEMFSUSBDemoBoardTest(void);

/** D E C L A R A T I O N S **************************************************/
#pragma code module

void TempUserInit(void)
{
    InitTempSensor();
    TempResetTempLog();
    _temp_mode = TEMP_REAL_TIME;
    
    /* Init Timer0 for data logging interval (every 1 second) */
    T0CON = 0b10010111;
    //T0CONbits.T08BIT = 0;       // 16-bit mode
    //T0CONbits.T0CS = 0;         // Select Fosc/4
    //T0CONbits.PSA = 0;          // Assign prescalar (default is /256)
    /* Timer0 is already enabled by default */
	//mLED_3_On();
}//end UserInit


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

void TempProcessIO(void){  
    if((usb_device_state < CONFIGURED_STATE)||(UCONbits.SUSPND==1)) return;
}//end ProcessIO

/******************************************************************************
 * Function:        UserTempConfigure(void)
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
void userTempConfigure(void){
// Do the configuration
}

void TempResetTempLog(void)
{
    _pTemp = 0;	
    _valid_temp = 0;
}//end ResetLog

void userTempInit(byte i){
	BOOL res;
	//inicializacion del sistema
	tempHandler = i;
	//agrego mi buffer en el handler module	
//	setHandlerReceiveBuffer(tempHandler, (byte*)&tempDataPacket);
	//agrego mi funcion de Receive en el handler module
	setHandlerReceiveFunction(tempHandler,&userTempReceived);
	//agrego la funcion de polling
	//res = addPollingFunction(&TempProcessIO);
	//inicializacion de modulo
	// seteo el buffer para mandar al PC
	sendBufferTemp = getSharedBuffer(tempHandler);
	TempUserInit();
	//TODO debo retornar res
}

void userTempRelease(byte i){
	tempHandler = i;
	unsetHandlerReceiveBuffer(tempHandler);
	unsetHandlerReceiveFunction(tempHandler);
	removePoolingFunction(&TempProcessIO);
}

void userTempReceived(byte* recBuffPtr,byte len){
	  byte index;
	  char mens[9] = "UReceive";	
      tempCounter = 0;
      switch(((DATA_PACKET*)recBuffPtr)->CMD)
      {
        case READ_VERSION:
              //dataPacket._byte[1] is len
			  ((DATA_PACKET*)sendBufferTemp)->_byte[0] = ((DATA_PACKET*)recBuffPtr)->_byte[0]; // hecho medio en dope a las 3 am	
			  ((DATA_PACKET*)sendBufferTemp)->_byte[1] = ((DATA_PACKET*)recBuffPtr)->_byte[1]; // hecho medio en dope a las 3 am despues de yuyos	
              ((DATA_PACKET*)sendBufferTemp)->_byte[2] = MINOR_VERSION;
              ((DATA_PACKET*)sendBufferTemp)->_byte[3] = MAJOR_VERSION;
              tempCounter=0x04;
              break;  

		case SET_TEMP_REAL:
              _temp_mode = TEMP_REAL_TIME;
              TempResetTempLog();
              tempCounter = 0x01;
              break;

          case RD_TEMP:
              if(AcquireTemperature()){
				  ((DATA_PACKET*)sendBufferTemp)->CMD = RD_TEMP;
                  ((DATA_PACKET*)sendBufferTemp)->word_data = temperature._word;
                  tempCounter=0x03;
              }//end if
              break;

          case SET_TEMP_LOGGING:
              _temp_mode = TEMP_LOGGING;
              TempResetTempLog();
              tempCounter=0x01;
              break;

          case RD_TEMP_LOGGING:
              tempCounter = (_valid_temp<<1)+2;  // Update count in byte
              ((DATA_PACKET*)sendBufferTemp)->len = (_valid_temp<<1);

              for(index = _valid_temp; index > 0; index--)
              {
                  if(_pTemp == 0)
                      _pTemp = 29;
                  else
                      _pTemp--;
                  ((DATA_PACKET*)sendBufferTemp)->_word[index] = _temp_data[_pTemp];
              }//end for
              
              TempResetTempLog();             // Once read, log will restart
              break;
              
          case RESET:
              Reset();
			  break;
		  
		 case MESS:
				sendMes(mens, sizeof(mens));
              break;
         
		 default:
			  //mLED_3_Off();
              break;
      }//end switch()
      if(tempCounter != 0)
      {
          if(!mUSBGenTxIsBusy())
              USBGenWrite2(tempHandler,/*sendBufferTemp,*/tempCounter);
      }//end if  	  	
}//end userTempReceived

/** EOF user.c ***************************************************************/
