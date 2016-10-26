/*********************************************************************
 *
 *      Microchip USB C18 Firmware -  USB Bootloader Version 1.00
 *
 *********************************************************************
 * FileName:        boot.c
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
 * Rawin Rojvanit       11/19/04    Original. USB Bootloader
 ********************************************************************/

/******************************************************************************
 * -boot.c-
 * This file contains functions necessary to carry out bootloading tasks.
 * The only 2 USB specific functions are BootInitEP() and BootService().
 * All other functions can be reused with other communication methods.
 *****************************************************************************/

/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include "system\typedefs.h"
#include "system\usb\usb.h"
#include "usb4all\boot\boot.h" //mio
#include "user\handlerManager.h"

#include "io_cfg.h"

/** V A R I A B L E S ********************************************************/

#pragma udata
byte boot_counter;
byte byteTemp;
byte boot_trf_state;
//BOOT_DATA_PACKET dataBootPacket;
word big_counter;
byte myBootHandler;
byte* sendBufferBoot;

/** P R I V A T E  P R O T O T Y P E S ***************************************/
void userBootConfigure(void);
void userBootRelease(byte i);
void userBootInit(byte i);
void bootReceived(byte* recBuffPtr,byte len);

/** USER MODULE REFERENCE*****************************************************/
#pragma romdata user 
uTab user2 = {&userBootInit,&userBootRelease,&userBootConfigure,"boot"};
#pragma code

/** D E C L A R A T I O N S **************************************************/
#pragma code module

/** C L A S S  S P E C I F I C  R E Q ****************************************/

/** U S E R  A P I ***********************************************************/

/******************************************************************************
 * Function:        void BootInitEP(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        BootInitEP initializes bootloader endpoints, buffer
 *                  descriptors, internal state-machine, and variables.
 *                  It should be called after the USB host has sent out a
 *                  SET_CONFIGURATION request.
 *                  See USBStdSetCfgHandler() in usb9.c for examples.
 *
 * Note:            None
 *****************************************************************************/
void userBootInit(byte i){
	BOOL res;
	//inicializacion del sistema
	myBootHandler=i;
	//agrego mi buffer en el handler module	
//	setHandlerReceiveBuffer(myBootHandler, (byte*)&dataBootPacket);
	//agrego mi funcion de Receive en el handler module
	setHandlerReceiveFunction(myBootHandler,&bootReceived);
	sendBufferBoot = getSharedBuffer(myBootHandler);
	//inicializacion de modulo
	//UserInit();
	//agrego la funcion de polling
	//res = addPollingFunction(&ProcessIO);
}

void userBootConfigure(void){
// Do the configuration
}

void userBootRelease(byte i){
	myBootHandler = i;
	unsetHandlerReceiveBuffer(myBootHandler);
	unsetHandlerReceiveFunction(myBootHandler);
}

void StartWrite(void)
{
    /*
     * A write command can be prematurely terminated by MCLR or WDT reset
     */
    EECON2 = 0x55;
    EECON2 = 0xAA;
    EECON1_WR = 1;
}//end StartWrite

void ReadVersion(byte* recBuffPtr) //TESTED: Passed
{
    ((BOOT_DATA_PACKET*)sendBufferBoot)->_byte[0] = ((BOOT_DATA_PACKET*)recBuffPtr)->_byte[0];
    ((BOOT_DATA_PACKET*)sendBufferBoot)->_byte[1] = ((BOOT_DATA_PACKET*)recBuffPtr)->_byte[1];;
    ((BOOT_DATA_PACKET*)sendBufferBoot)->_byte[2] = BOOT_MINOR_VERSION;
    ((BOOT_DATA_PACKET*)sendBufferBoot)->_byte[3] = BOOT_MAJOR_VERSION;
}//end ReadVersion

void ReadProgMem(byte* recBuffPtr) //TESTED: Passed
{
    //copio el header del comando que llego en el sendBufferBoot
	((BOOT_DATA_PACKET*)sendBufferBoot)->_byte[0] = ((BOOT_DATA_PACKET*)recBuffPtr)->_byte[0];
    ((BOOT_DATA_PACKET*)sendBufferBoot)->_byte[1] = ((BOOT_DATA_PACKET*)recBuffPtr)->_byte[1];
    ((BOOT_DATA_PACKET*)sendBufferBoot)->_byte[2] = ((BOOT_DATA_PACKET*)recBuffPtr)->_byte[2];
    ((BOOT_DATA_PACKET*)sendBufferBoot)->_byte[3] = ((BOOT_DATA_PACKET*)recBuffPtr)->_byte[3];
    ((BOOT_DATA_PACKET*)sendBufferBoot)->_byte[4] = ((BOOT_DATA_PACKET*)recBuffPtr)->_byte[4];
    
	for (boot_counter = 0; boot_counter < ((BOOT_DATA_PACKET*)recBuffPtr)->len; boot_counter++)
    {
        //2 separate inst prevents compiler from using RAM stack
        byteTemp = *((((BOOT_DATA_PACKET*)recBuffPtr)->ADR.pAdr)+boot_counter);
        ((BOOT_DATA_PACKET*)sendBufferBoot)->data[boot_counter] = byteTemp;
    }//end for
    
    TBLPTRU = 0x00;         // forces upper byte back to 0x00
                            // optional fix is to set large code model
}//end ReadProgMem

void WriteProgMem(byte* recBuffPtr) //TESTED: Passed
{
    /*
     * The write holding register for the 18F4550 family is
     * actually 32-byte. The code below only tries to write
     * 16-byte because the GUI program only sends out 16-byte
     * at a time.
     * This limitation will be fixed in the future version.
     */
    ((BOOT_DATA_PACKET*)recBuffPtr)->ADR.low &= 0b11110000;  //Force 16-byte boundary
    EECON1 = 0b10000100;        //Setup writes: EEPGD=1,WREN=1

    //LEN = # of byte to write

    for (boot_counter = 0; boot_counter < (((BOOT_DATA_PACKET*)recBuffPtr)->len); boot_counter++)
    {
        *((((BOOT_DATA_PACKET*)recBuffPtr)->ADR.pAdr)+boot_counter) = \
        ((BOOT_DATA_PACKET*)recBuffPtr)->data[boot_counter];
        if ((boot_counter & 0b00001111) == 0b00001111)
        {
            StartWrite();
        }//end if
    }//end for
}//end WriteProgMem

void EraseProgMem(byte* recBuffPtr) //TESTED: Passed
{
    //The most significant 16 bits of the address pointer points to the block
    //being erased. Bits5:0 are ignored. (In hardware).

    //LEN = # of 64-byte block to erase
    EECON1 = 0b10010100;     //Setup writes: EEPGD=1,FREE=1,WREN=1
    for(boot_counter=0; boot_counter < ((BOOT_DATA_PACKET*)recBuffPtr)->len; boot_counter++)
    {
        *(((BOOT_DATA_PACKET*)recBuffPtr)->ADR.pAdr+(((int)boot_counter) << 6));  //Load TBLPTR
        StartWrite();
    }//end for
    TBLPTRU = 0;            // forces upper byte back to 0x00
                            // optional fix is to set large code model
                            // (for USER ID 0x20 0x00 0x00)
}//end EraseProgMem

void ReadEE(byte* recBuffPtr) //TESTED: Passed
{
    EECON1 = 0x00;
    for(boot_counter=0; boot_counter < ((BOOT_DATA_PACKET*)recBuffPtr)->len; boot_counter++)
    {
        EEADR = (byte)((BOOT_DATA_PACKET*)recBuffPtr)->ADR.pAdr + boot_counter;
        //EEADRH = (BYTE)(((int)dataBootPacket.FIELD.ADDR.POINTER + boot_counter) >> 8);
        EECON1_RD = 1;
        ((BOOT_DATA_PACKET*)sendBufferBoot)->data[boot_counter] = EEDATA;
    }//end for
}//end ReadEE

void WriteEE(byte* recBuffPtr) //TESTED: Passed
{
    for(boot_counter=0; boot_counter < ((BOOT_DATA_PACKET*)recBuffPtr)->len; boot_counter++)
    {
        EEADR = (byte)((BOOT_DATA_PACKET*)recBuffPtr)->ADR.pAdr + boot_counter;
        //EEADRH = (BYTE)(((int)dataBootPacket.FIELD.ADDR.POINTER + boot_counter) >> 8);
        EEDATA = ((BOOT_DATA_PACKET*)recBuffPtr)->data[boot_counter];
        EECON1 = 0b00000100;    //Setup writes: EEPGD=0,WREN=1
        StartWrite();
        while(EECON1_WR);       //Wait till WR bit is clear
    }//end for
}//end WriteEE

//WriteConfig is different from WriteProgMem b/c it can write a byte
void WriteConfig(byte* recBuffPtr) //TESTED: Passed
{
    EECON1 = 0b11000100;        //Setup writes: EEPGD=1,CFGS=1,WREN=1
    for (boot_counter = 0; boot_counter < ((BOOT_DATA_PACKET*)recBuffPtr)->len; boot_counter++)
    {
        *((((BOOT_DATA_PACKET*)sendBufferBoot)->ADR.pAdr)+boot_counter) = \
        ((BOOT_DATA_PACKET*)recBuffPtr)->data[boot_counter];
        StartWrite();
    }//end for
    
    TBLPTRU = 0x00;         // forces upper byte back to 0x00
                            // optional fix is to set large code model
}//end WriteConfig

/*
void BootService(void)
{
    //BlinkUSBStatus();
    if((usb_device_state < CONFIGURED_STATE)||(UCONbits.SUSPND==1)) return;
    
    if(boot_trf_state == SENDING_RESP)
    {
        if(!mBootTxIsBusy())
        {
            BOOT_BD_OUT.Cnt = sizeof(dataBootPacket);
            mUSBBufferReady(BOOT_BD_OUT);
            boot_trf_state = WAIT_FOR_CMD;
        }//end if
        return;
    }//end if
    
    if(!mBootRxIsBusy())
    {
        boot_counter = 0;
        switch(dataBootPacket.CMD)
        {
            case READ_VERSION:
                ReadVersion();
                boot_counter=0x04;
                break;

            case READ_FLASH:
            case READ_CONFIG:
                ReadProgMem();
                boot_counter+=0x05;
                break;

            case WRITE_FLASH:
                WriteProgMem();
                boot_counter=0x01;
                break;

            case ERASE_FLASH:
                EraseProgMem();
                boot_counter=0x01;
                break;

            case READ_EEDATA:
                ReadEE();
                boot_counter+=0x05;
                break;

            case WRITE_EEDATA:
                WriteEE();
                boot_counter=0x01;
                break;

            case WRITE_CONFIG:
                WriteConfig();
                boot_counter=0x01;
                break;
            
            case RESET:
                //When resetting, make sure to drop the device off the bus
                //for a period of time. Helps when the device is suspended.
                UCONbits.USBEN = 0;
                big_counter = 0;
                while(--big_counter);
                
                Reset();
                break;
            
            case UPDATE_LED:
                if(dataBootPacket.led_num == 3)
                {
                    mLED_3 = dataBootPacket.led_status;
                    boot_counter = 0x01;
                }//end if
                if(dataBootPacket.led_num == 4)
                {
                    mLED_4 = dataBootPacket.led_status;
                    boot_counter = 0x01;
                }//end if
                break;
                
            default:
                break;
        }//end switch()
        boot_trf_state = SENDING_RESP;
        if(boot_counter != 0)
        {
            BOOT_BD_IN.Cnt = boot_counter;
            mUSBBufferReady(BOOT_BD_IN);
        }//end if
    }//end if
}//end BootService
*/

void bootReceived(byte* recBuffPtr,byte len)
{
    //BlinkUSBStatus();
	//TODO pasarla a la arquitectura
    //if((usb_device_state < CONFIGURED_STATE)||(UCONbits.SUSPND==1)) return;
    /*
    if(boot_trf_state == SENDING_RESP)
    {
        if(!mBootTxIsBusy())
       // if(!mUSBGenTxIsBusy())
		
		{
            USBGenWrite2(myBootHandler,boot_counter);
			boot_trf_state = WAIT_FOR_CMD;
        }//end if
        return;
    }//end if
    */
    //if(!mBootRxIsBusy())
    //if(!mUSBGenRxIsBusy())

	
	{
        boot_counter = 0;
        switch(((BOOT_DATA_PACKET*)recBuffPtr)->CMD)
        {
            case READ_VERSION:
                ReadVersion(recBuffPtr);
                boot_counter=0x04;
                break;

            case READ_FLASH:
            case READ_CONFIG:
                ReadProgMem(recBuffPtr);
                boot_counter+=0x05;
                break;

            case WRITE_FLASH:
                WriteProgMem(recBuffPtr);
                boot_counter=0x01;
                break;

            case ERASE_FLASH:
                EraseProgMem(recBuffPtr);
                boot_counter=0x01;
                break;

            case READ_EEDATA:
                ReadEE(recBuffPtr);
                boot_counter+=0x05;
                break;

            case WRITE_EEDATA:
                WriteEE(recBuffPtr);
                boot_counter=0x01;
                break;

            case WRITE_CONFIG:
                WriteConfig(recBuffPtr);
                boot_counter=0x01;
                break;
            
            case RESET:
                //When resetting, make sure to drop the device off the bus
                //for a period of time. Helps when the device is suspended.
                UCONbits.USBEN = 0;
                big_counter = 0;
                while(--big_counter);
                
                Reset();
                break;
            
            case UPDATE_LED:
                if(((BOOT_DATA_PACKET*)recBuffPtr)->led_num == 3)
                {
                    mLED_3 = ((BOOT_DATA_PACKET*)recBuffPtr)->led_status;
                    boot_counter = 0x01;
                }//end if
                if(((BOOT_DATA_PACKET*)recBuffPtr)->led_num == 4)
                {
                    mLED_4 = ((BOOT_DATA_PACKET*)recBuffPtr)->led_status;
                    boot_counter = 0x01;
                }//end if
                break;
                
            default:
                break;
        }//end switch()
        boot_trf_state = SENDING_RESP;
        if(boot_counter != 0)
        {
            USBGenWrite2(myBootHandler/*,sendBufferBoot*/,boot_counter);
			boot_trf_state = WAIT_FOR_CMD; //se lo agregue para testear pq le saque el if del principio que hacia write tb
        }//end if
    }//end if
}//end BootService


/*
void bootReceived(int len){
	byte index;
    
      boot_counter = 0;
      switch(dataBootPacket.CMD)
      {
          case READ_VERSION:
              //dataPacket._byte[1] is len
              dataBootPacket._byte[2] = BOOT_MINOR_VERSION;
              dataBootPacket._byte[3] = BOOT_MAJOR_VERSION;
              boot_counter=0x04;
              break;
		      
          default:
              break;
      }//end switch()
      if(boot_counter != 0)
      {
          if(!mBootTxIsBusy())
              USBGenWrite3(myBootHandler,(byte*)&dataBootPacket,boot_counter);
      }//end if
  
}//end ServiceRequests
*/

/** EOF boot.c ***************************************************************/
