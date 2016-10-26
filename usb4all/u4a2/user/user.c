/*********************************************************************
 *
 *                Microchip USB C18 Firmware Version 1.0
 *
 *********************************************************************
 * FileName:        user.c
 * Dependencies:    See INCLUDES section below
 * Processor:       PIC18
 * Compiler:        C18 2.30.01+
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PICmicro® Microcontroller is intended and
 * supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Rawin Rojvanit       11/19/04    Original.
 ********************************************************************/

/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include <usart.h>
#include "system\typedefs.h"

#include "system\usb\usb.h"

#include "io_cfg.h"             // I/O pin mapping
#include "user\user.h"
#include "user\temperature.h"
#include "user\handlerManager.h"
#include "dynamicPolling.h"                              // Modifiable

/** V A R I A B L E S ********************************************************/
#pragma udata
byte old_sw2,old_sw3;
byte counter;
byte trf_state;
byte temp_mode;
byte myHandler;

DATA_PACKET dataPacket;

byte pTemp;                     // Pointer to current logging position, will
                                // loop to zero once the max index is reached
byte valid_temp;                // Keeps count of the valid data points
word* temp_data;             // 30 points of data

// Timer0 - 1 second interval setup.
// Fosc/4 = 12MHz
// Use /256 prescalar, this brings counter freq down to 46,875 Hz
// Timer0 should = 65536 - 46875 = 18661 or 0x48E5
#define TIMER0L_VAL         0xE5
#define TIMER0H_VAL         0x48

/** P R I V A T E  P R O T O T Y P E S ***************************************/

void ProcessIO(void);
void UserInit(void);
void BlinkUSBStatus(void);
BOOL Switch2IsPressed(void);
BOOL Switch3IsPressed(void);
void ResetTempLog(void);
void ReadPOT(void);
void ServiceRequests(void);

// For board testing purpose only
void PICDEMFSUSBDemoBoardTest(void);

/** D E C L A R A T I O N S **************************************************/
#pragma code module
void UserInit(void)
{
    mInitAllLEDs();
    mInitAllSwitches();
    old_sw2 = sw2;
    old_sw3 = sw3;
    
    InitTempSensor();
    mInitPOT();
    ADCON2bits.ADFM = 1;   // ADC result right justified
  
    ResetTempLog();
    temp_mode = TEMP_REAL_TIME;
    
    /* Init Timer0 for data logging interval (every 1 second) */
    T0CON = 0b10010111;
    //T0CONbits.T08BIT = 0;       // 16-bit mode
    //T0CONbits.T0CS = 0;         // Select Fosc/4
    //T0CONbits.PSA = 0;          // Assign prescalar (default is /256)
    /* Timer0 is already enabled by default */
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
 * Overview:        This function is a place holder for other user routines.
 *                  It is a mixture of both USB and non-USB tasks.
 *
 * Note:            None
 *****************************************************************************/
void ProcessIO(void)
{   
    BlinkUSBStatus();
    // User Application USB tasks
    if((usb_device_state < CONFIGURED_STATE)||(UCONbits.SUSPND==1)) return;
	//pase el if para el handler module (lo deje aca por las dudas    

    //ServiceRequests();
	//hago como que el sistema invocara el handlerManager
	

    if(temp_mode == TEMP_LOGGING)
    {
        if(INTCONbits.TMR0IF == 1)
        {
            INTCONbits.TMR0IF = 0;          // Clear flag
            TMR0H = TIMER0H_VAL;
            TMR0L = TIMER0L_VAL;            // Reinit timer value;

            if(AcquireTemperature())
            {
                temp_data[pTemp] = temperature._word;
                
                // First update valid_temp
                if(valid_temp < 30)         // 30 data points max
                    valid_temp++;
                    
                // Next update pTemp
                if(pTemp == 29)
                    pTemp = 0;
                else
                    pTemp++;
            }//end if
        }//end if
    }//end if
}//end ProcessIO

void ResetTempLog(void)
{
    pTemp = 0;
    valid_temp = 0;
}//end ResetLog

void ReadPOT(void)
{
    ADCON0bits.GO = 1;              // Start AD conversion
    while(ADCON0bits.NOT_DONE);     // Wait for conversion
    return;
}//end ReadPOT


void userInit(byte i){
	BOOL res;
	//inicializacion del sistema
	myHandler=i;
	//agrego mi buffer en el handler module	
	setHandlerReceiveBuffer(myHandler, (byte*)&dataPacket);
	//agrego mi funcion de Receive en el handler module
	setHandlerReceiveFunction(myHandler,&userReceived);

	//inicializacion de modulo
	UserInit();
	//agrego la funcion de polling
	res = addPollingFunction(&ProcessIO);
}

void userRelease(byte i){
	myHandler = i;
	unsetHandlerReceiveBuffer(myHandler);
	unsetHandlerReceiveFunction(myHandler);
	removePoolingFunction(&ProcessIO);
}

void userReceived(int len){
	byte index;
    
      counter = 0;
      switch(dataPacket.CMD)
      {
          case READ_VERSION:
              //dataPacket._byte[1] is len
              dataPacket._byte[2] = MINOR_VERSION;
              dataPacket._byte[3] = MAJOR_VERSION;
              counter=0x04;
              break;

          case ID_BOARD:
              counter = 0x01;
              if(dataPacket.ID == 0)
              {
                  mLED_3_Off();mLED_4_Off();
              }
              else if(dataPacket.ID == 1)
              {
                  mLED_3_Off();mLED_4_On();
              }
              else if(dataPacket.ID == 2)
              {
                  mLED_3_On();mLED_4_Off();
              }
              else if(dataPacket.ID == 3)
              {
                  mLED_3_On();mLED_4_On();
              }
              else
                  counter = 0x00;
              break;

          case UPDATE_LED:
              // LED1 & LED2 are used as USB event indicators.
              if(dataPacket.led_num == 3)
              {
                  mLED_3 = dataPacket.led_status;
                  counter = 0x01;
              }//end if
              else if(dataPacket.led_num == 4)
              {
                  mLED_4 = dataPacket.led_status;
                  counter = 0x01;
              }//end if else
              break;
              
          case SET_TEMP_REAL:
              temp_mode = TEMP_REAL_TIME;
              ResetTempLog();
              counter = 0x01;
              break;

          case RD_TEMP:
              if(AcquireTemperature())
              {
                  dataPacket.word_data = temperature._word;
                  counter=0x03;
              }//end if
              break;

          case SET_TEMP_LOGGING:
              temp_mode = TEMP_LOGGING;
              ResetTempLog();
              counter=0x01;
              break;

          case RD_TEMP_LOGGING:
              counter = (valid_temp<<1)+2;  // Update count in byte
              dataPacket.len = (valid_temp<<1);

              for(index = valid_temp; index > 0; index--)
              {
                  if(pTemp == 0)
                      pTemp = 29;
                  else
                      pTemp--;
                  dataPacket._word[index] = temp_data[pTemp];
              }//end for
              
              ResetTempLog();             // Once read, log will restart
              break;

          case RD_POT:
              ReadPOT();
              dataPacket._byte[1] = ADRESL;
              dataPacket._byte[2] = ADRESH;
              counter=0x03;
              break;
              
          case RESET:
              Reset();
              break;
              
          default:
              break;
      }//end switch()
      if(counter != 0)
      {
          if(!mUSBGenTxIsBusy())
              USBGenWrite2(myHandler,(byte*)&dataPacket,counter);
      }//end if
  
}//end ServiceRequests


/******************************************************************************
 * Function:        void BlinkUSBStatus(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        BlinkUSBStatus turns on and off LEDs corresponding to
 *                  the USB device state.
 *
 * Note:            mLED macros can be found in io_cfg.h
 *                  usb_device_state is declared in usbmmap.c and is modified
 *                  in usbdrv.c, usbctrltrf.c, and usb9.c
 *****************************************************************************/
void BlinkUSBStatus(void)
{
    static word led_count=0;
    
    if(led_count == 0)led_count = 10000U;
    led_count--;

    #define mLED_Both_Off()         {mLED_1_Off();mLED_2_Off();}
    #define mLED_Both_On()          {mLED_1_On();mLED_2_On();}
    #define mLED_Only_1_On()        {mLED_1_On();mLED_2_Off();}
    #define mLED_Only_2_On()        {mLED_1_Off();mLED_2_On();}

    if(UCONbits.SUSPND == 1)
    {
        if(led_count==0)
        {
            mLED_1_Toggle();
            mLED_2 = mLED_1;        // Both blink at the same time
        }//end if
    }
    else
    {
        if(usb_device_state == DETACHED_STATE)
        {
            mLED_Both_Off();
            
            PICDEMFSUSBDemoBoardTest();
        }
        else if(usb_device_state == ATTACHED_STATE)
        {
            mLED_Both_On();
        }
        else if(usb_device_state == POWERED_STATE)
        {
            mLED_Only_1_On();
        }
        else if(usb_device_state == DEFAULT_STATE)
        {
            mLED_Only_2_On();
        }
        else if(usb_device_state == ADDRESS_STATE)
        {
            if(led_count == 0)
            {
                mLED_1_Toggle();
                mLED_2_Off();
            }//end if
        }
        else if(usb_device_state == CONFIGURED_STATE)
        {
            if(led_count==0)
            {
                mLED_1_Toggle();
                mLED_2 = !mLED_1;       // Alternate blink                
            }//end if
        }//end if(...)
    }//end if(UCONbits.SUSPND...)

}//end BlinkUSBStatus

BOOL Switch2IsPressed(void)
{
    if(sw2 != old_sw2)
    {
        old_sw2 = sw2;                  // Save new value
        if(sw2 == 0)                    // If pressed
            return TRUE;                // Was pressed
    }//end if
    return FALSE;                       // Was not pressed
}//end Switch2IsPressed

BOOL Switch3IsPressed(void)
{
    if(sw3 != old_sw3)
    {
        old_sw3 = sw3;                  // Save new value
        if(sw3 == 0)                    // If pressed
            return TRUE;                // Was pressed
    }//end if
    return FALSE;                       // Was not pressed
}//end Switch3IsPressed

void TXbyte(byte data)
{
    while(TXSTAbits.TRMT==0);
    TXREG = data;
}//end TXbyte

void PICDEMFSUSBDemoBoardTest(void)
{
    byte temp;
    
    //PICDEM FS USB Demo Board Test Procedure:
    if(Switch2IsPressed())
    {
        //LEDs and push buttons testing
        mLED_1_On();
        while(!Switch2IsPressed());
        mLED_1_Off();
        mLED_2_On();
        while(!Switch3IsPressed());
        mLED_2_Off();
        mLED_3_On();
        while(!Switch3IsPressed());
        mLED_3_Off();
        mLED_4_On();
        while(!Switch3IsPressed());
        mLED_4_Off();
        
        //RS-232 Setup
        SSPCON1 = 0;        // Make sure SPI is disabled
        TRISCbits.TRISC7=1; // RX
        TRISCbits.TRISC6=0; // TX
        SPBRG = 0x71;
        SPBRGH = 0x02;      // 0x0271 for 48MHz -> 19200 baud
        TXSTA = 0x24;       // TX enable BRGH=1
        RCSTA = 0x90;       // continuous RX
        BAUDCON = 0x08;     // BRG16 = 1
        temp = RCREG;       // Empty buffer
        temp = RCREG;       // Empty buffer
        
        //RS-232 Tx & Rx Tests
        while(!Switch3IsPressed());
        TXbyte('R');
        TXbyte('S');
        TXbyte('-');
        TXbyte('2');
        TXbyte('3');
        TXbyte('2');
        TXbyte(' ');
        TXbyte('T');
        TXbyte('X');
        TXbyte(' ');
        TXbyte('T');
        TXbyte('e');
        TXbyte('s');
        TXbyte('t');
        TXbyte(' ');
        TXbyte('O');
        TXbyte('K');
        TXbyte(',');
        TXbyte(' ');
        TXbyte('P');
        TXbyte('r');
        TXbyte('e');
        TXbyte('s');
        TXbyte('s');
        TXbyte(' ');
        TXbyte('"');
        TXbyte('r');
        TXbyte('"');
        TXbyte(',');
        while(PIR1bits.RCIF==0);        //Wait for data from RS232
        if(RCREG == 'r')
        {
            TXbyte(' ');
            TXbyte('R');
            TXbyte('X');
            TXbyte(' ');
            TXbyte('T');
            TXbyte('e');
            TXbyte('s');
            TXbyte('t');
            TXbyte(' ');
            TXbyte('O');
            TXbyte('K');
        }//end if
        UserInit();                     //Re-initialize default user fw
        //Test phase 1 done
    }//end if
}//end PICDEMFSUSBDemoBoardTest()

/** EOF user.c ***************************************************************/
