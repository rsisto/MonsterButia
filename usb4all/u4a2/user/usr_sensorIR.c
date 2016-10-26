/* Author             									  Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *Andres Aguirre; Edgardo Vaz								11/06/10      Original.
 *****************************************************************************/

/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include <usart.h>
#include "system\typedefs.h"
#include "system\usb\usb.h"
#include "user\usr_sensorIR.h"
#include "io_cfg.h"              // I/O pin mapping
#include "user\handlerManager.h"
#include "dynamicPolling.h"                              


/** V A R I A B L E S ********************************************************/
#pragma udata

byte  usrSensorIRHandler;    // Handler number asigned to the module
byte* sendBufferUsrSensorIR; // buffer to send data

/** P R I V A T E  P R O T O T Y P E S ***************************************/
void UserSensorIRProcessIO(void);
void UserSensorIRInit(byte i);
void UserSensorIRReceived(byte*, byte);
void UserSensorIRRelease(byte i);
void UserSensorIRConfigure(void);
void ReadSensorIR(void);

// Table used by te framework to get a fixed reference point to the user module functions defined by the framework 
/** USER MODULE REFERENCE*****************************************************/
#pragma romdata user
uTab userSensorIRModuleTable = {&UserSensorIRInit,&UserSensorIRRelease,&UserSensorIRConfigure,"sensor"}; //modName must be less 8 characters
#pragma code

/** D E C L A R A T I O N S **************************************************/
#pragma code module

/******************************************************************************
 * Function:        UserSensorIRInit(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is initialices the resources that the user module needs to work, it is called by the    		    framework when the module is opened	
 *
 * Note:            None
 *****************************************************************************/

void UserSensorIRInit(byte i){
	BOOL res;
	usrSensorIRHandler = i;
	// add my receive function to the handler module, to be called automatically when the pc sends data to the user module
	setHandlerReceiveFunction(usrSensorIRHandler, &UserSensorIRReceived);
	// add my receive pooling function to the dynamic pooling module, to be called periodically 
	//res = addPollingFunction(&UserSensorIRProcessIO);
	// initialize the send buffer, used to send data to the PC
	sendBufferUsrSensorIR = getSharedBuffer(usrSensorIRHandler);
	//TODO return res value 
	mInitSensorIR();
}//end UserSensorIRInit

void ReadSensorIR(void)
{
    ADCON0bits.GO = 1;              // Start AD conversion
    while(ADCON0bits.NOT_DONE);     // Wait for conversion
    return;
}//end ReadSensorIR

/******************************************************************************
/* Function:        UserSensorIRConfigure(void)
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
 * Note:            None
 *****************************************************************************/
void UserSensorIRConfigure(void){
// Do the configuration
}

/******************************************************************************
 * Function:        UserSensorIRProcessIO(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is registered in the dinamic polling, who call ir periodically to process the IO                           interaction int the PIC, also it can comunicate things to the pc by the USB
 *
 * Note:            None
 *****************************************************************************/

void UserSensorIRProcessIO(void){

    if((usb_device_state < CONFIGURED_STATE)||(UCONbits.SUSPND==1)) return;
    // here enter the code that want to be called periodically, per example interaction with buttons and leds
}//end ProcessIO



/******************************************************************************
 * Function:        UserSensorIRRelease(byte i)
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
 *		    when the module is close	
 *
 * Note:            None
 *****************************************************************************/

void UserSensorIRRelease(byte i){
	unsetHandlerReceiveBuffer(i);
	unsetHandlerReceiveFunction(i);
	removePoolingFunction(&UserSensorIRProcessIO);
}


/******************************************************************************
 * Function:        UserSensorIRReceived(byte* recBuffPtr, byte len)
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

void UserSensorIRReceived(byte* recBuffPtr, byte len){
	byte index;
	char mens[9] = "User Skeleton is alive";	
	byte userSensorIRCounter = 0;
	switch(((SENSOR_IR_DATA_PACKET*)recBuffPtr)->CMD)
	{
        case READ_VERSION:
              //dataPacket._byte[1] is len
		((SENSOR_IR_DATA_PACKET*)sendBufferUsrSensorIR)->_byte[0] = ((SENSOR_IR_DATA_PACKET*)recBuffPtr)->_byte[0];
		((SENSOR_IR_DATA_PACKET*)sendBufferUsrSensorIR)->_byte[1] = ((SENSOR_IR_DATA_PACKET*)recBuffPtr)->_byte[1]; 
		((SENSOR_IR_DATA_PACKET*)sendBufferUsrSensorIR)->_byte[2] = SENSOR_IR_MINOR_VERSION;
		((SENSOR_IR_DATA_PACKET*)sendBufferUsrSensorIR)->_byte[3] = SENSOR_IR_MAJOR_VERSION;
		userSensorIRCounter = 0x04;
		break;
	
        case RD_SENSOR_IR:
		ReadSensorIR();
		((SENSOR_IR_DATA_PACKET*)sendBufferUsrSensorIR)->_byte[0] = ((SENSOR_IR_DATA_PACKET*)recBuffPtr)->_byte[0];
		((SENSOR_IR_DATA_PACKET*)sendBufferUsrSensorIR)->_byte[1] = ADRESL;
		((SENSOR_IR_DATA_PACKET*)sendBufferUsrSensorIR)->_byte[2] = ADRESH;
		userSensorIRCounter = 0x03;
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
      if(userSensorIRCounter != 0)
      {
		if(!mUSBGenTxIsBusy())
			USBGenWrite2(usrSensorIRHandler, userSensorIRCounter);
      }//end if
}//end UserSensorIRReceived

/** EOF usr_sensorIR.c ***************************************************************/