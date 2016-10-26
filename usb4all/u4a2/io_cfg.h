/*********************************************************************
 *
 *                Microchip USB C18 Firmware Version 1.0
 *
 *********************************************************************
 * FileName:        io_cfg.h
 * Dependencies:    See INCLUDES section below
 * Processor:       PIC18
 * Compiler:        C18 2.30.01+
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the �Company�) for its PICmicro� Microcontroller is intended and
 * supplied to you, the Company�s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN �AS IS� CONDITION. NO WARRANTIES,
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

#ifndef IO_CFG_H
#define IO_CFG_H

/** I N C L U D E S *************************************************/
#include "autofiles\usbcfg.h"

/** T R I S *********************************************************/
#define INPUT_PIN           1
#define OUTPUT_PIN          0

/** U S B ***********************************************************/
#define tris_usb_bus_sense  TRISAbits.TRISA1    // Input

#if defined(USE_USB_BUS_SENSE_IO)
#define usb_bus_sense       PORTAbits.RA1
#else
#define usb_bus_sense       1
#endif

#define tris_self_power     TRISAbits.TRISA2    // Input

#if defined(USE_SELF_POWER_SENSE_IO)
#define self_power          PORTAbits.RA2
#else
#define self_power          1
#endif


/** L E D ***********************************************************/
//OJO esto es una AND bit a bit y me resetea el resto de los bits!
//#define mInitAllLEDs()      LATB &= 0x00; TRISB &= 0b10111100; //modificado
//#define mInitAllLEDs() //TRISBbits.TRISB0=0;TRISBbits.TRISB1=0;TRISBbits.TRISB6=0;LATBbits.LATB0=0;LATBbits.LATB1=0;LATBbits.LATB6=0;
#define mLED_1              LATBbits.LATB0
#define mLED_2              LATBbits.LATB1
#define mLED_3              LATBbits.LATB6
//#define mLED_4              LATBbits.LATB7

#define mInitPortA() TRISA=0XFF; //is a good practice to set unused pins to input
#define mInitPortB() TRISB=0XFF; //is a good practice to set unused pins to input
#define mInitPortC() TRISC=0XFF; //is a good practice to set unused pins to input
#define mInitPortD() TRISD=0XFF; //is a good practice to set unused pins to input
#define mInitPortE() TRISE=0XFF; //is a good practice to set unused pins to input

#define mInitLed1()  TRISBbits.TRISB0=0;LATBbits.LATB0=0;
#define mInitLed2()  TRISBbits.TRISB1=0;LATBbits.LATB1=0;
#define mInitLed3()  TRISBbits.TRISB6=0;LATBbits.LATB6=0;

#define mLED_1_On()         mLED_1 = 1;
#define mLED_2_On()         mLED_2 = 1;
#define mLED_3_On()         mLED_3 = 1;
//#define mLED_4_On()         mLED_4 = 1;

#define mLED_1_Off()        mLED_1 = 0;
#define mLED_2_Off()        mLED_2 = 0;
#define mLED_3_Off()        mLED_3 = 0;
//#define mLED_4_Off()        mLED_4 = 0;

#define mLED_1_Toggle()     mLED_1 = !mLED_1;
#define mLED_2_Toggle()     mLED_2 = !mLED_2;
#define mLED_3_Toggle()     mLED_3 = !mLED_3;
//#define mLED_4_Toggle()     mLED_4 = !mLED_4;

/** P U E R T A *****************************************************/

#define mInitPuerta()		TRISBbits.TRISB7=0; 
#define puerta			    LATBbits.LATB7
#define puerta_on()		    puerta = 1;
#define puerta_off()		puerta = 0;

/** S W I T C H *****************************************************/
//#define mInitAllSwitches()  TRISBbits.TRISB4=1;/*TRISBbits.TRISB5=1;*/ /*Modificado*/
//#define mInitSwitch2()      TRISBbits.TRISB4=1;
/*#define mInitSwitch3()      TRISBbits.TRISB5=1;*/
//#define sw2                 PORTBbits.RB4
/*#define sw3                 PORTBbits.RB5*/

/** BUZZER *****************************************************/
//#define mInitBuzzer()  	    TRISBbits.TRISB5=0;LATBbits.LATB5=0;
#define mInitBuzzer()  	    TRISBbits.TRISB6=0;LATBbits.LATB6=0;

//#define buzzer	 	    LATBbits.LATB5
#define buzzer              LATBbits.LATB6

#define buzzer_on()	        buzzer = 1;
#define buzzer_off()        buzzer = 0;

/** BACK LIGHT *****************************************************/
#define mInitBKL()  	    TRISBbits.TRISB2=0;LATBbits.LATB2=0; 

#define backlight 	        LATBbits.LATB2

#define BACK_LIGHT_ON()     backlight = 1;
#define BACK_LIGHT_OFF()    backlight = 0;

/** SENSOR ***********************************************************/
#define mInitSensorIR()     TRISAbits.TRISA0=1;ADCON0=0x01;ADCON2=0x3C;ADCON2bits.ADFM=1; //ADC result right justified
#define MOVE_SENSOR         LATDbits.LATD7
#define mInitMoveSensor()   TRISBbits.TRISB7=1;

/** POTE ************************************************************/
#define mInitPOT()          TRISAbits.TRISA0=1;ADCON0=0x01;ADCON2=0x3C;

/** GAS ************************************************************/
#define mInitGAS()          TRISAbits.TRISA1=1;ADCON0=0x01;ADCON2=0x3C;

/** S P I : Chip Select Lines ***************************************/
#define tris_cs_temp_sensor TRISBbits.TRISB2    // Output
#define cs_temp_sensor      LATBbits.LATB2

#define tris_cs_sdmmc       TRISBbits.TRISB3    // Output
#define cs_sdmmc            LATBbits.LATB3

/** S D M M C *******************************************************/
#define TRIS_CARD_DETECT    TRISBbits.TRISB4    // Input
#define CARD_DETECT         PORTBbits.RB4

//#define TRIS_WRITE_DETECT   TRISAbits.TRISA4    // Input
//#define WRITE_DETECT        PORTAbits.RA4

#endif //IO_CFG_H
