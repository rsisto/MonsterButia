/* Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Andres Aguirre       26/03/07    Original.
 ********************************************************************/
#ifndef ADMIN_MODULE_H
#define ADMIN_MODULE_H
/** I N C L U D E S **********************************************************/
#include "user\handlerManager.h"
#include "user\defines.h"
#include "user\descriptorManager.h"
#include "user\loaderModule.h"
#include "system\typedefs.h"
#include "system\usb\usb.h"
#include "io_cfg.h"   
#include <p18cxxx.h>
#include "user\userTemp.h"
#include "USB4all\boot\boot.h"
#include <string.h>

/** D E F I N I T I O N S ****************************************************/
#define HM_PACKET_SIZE 253
#define HM_PACKET_HEADER_SIZE 1
#define HM_PACKET_PAYLOAD_SIZE (HM_PACKET_SIZE - HM_PACKET_HEADER_SIZE)
/** S T R U C T U R E S ******************************************************/

typedef union _AM_PACKET {
	byte _byte[HM_PACKET_SIZE];
	struct {
		enum {
			OPEN    			   = 0x00,
			CLOSE  				   = 0x01,
			MESSAGE 			   = 0x02,
			LOAD    			   = 0x03,
			UNLOAD  			   = 0x04,
			GET_USER_MODULES_SIZE  = 0x05,
			GET_USER_MODULES_LINE  = 0x06,
			INIT				   = 0x07,
			CONFIGURE			   = 0x08, 
			RESET				   = 0xff
		}CMD;
		byte payload[HM_PACKET_PAYLOAD_SIZE];
	};
	struct { //Open, PC->PLACA
		unsigned :8;
		byte inEp;
		byte outEp;
		byte moduleId[8];
	};
	struct { //Open, PLACA->PC
		unsigned :8;
		byte handlerNumber;
		//byte epNumber; no devuelvo mas el endpoint, se administra en la pc
	};
	struct { //Close, PC->PLACA
		unsigned :8;
		byte handlerNumber;
	};
	struct { //Close, PLACA->PC
		unsigned :8;
		respType response; //respType en defines.h
	};
	struct { //Message, PLACA->PC y MODULO_USUARIO->ADMIN?
		unsigned :8;
		char texto[HM_PACKET_SIZE - HM_PACKET_HEADER_SIZE];
	};
	struct { //GET_USER_MODULES_SIZE, PC->PLACA
		unsigned :8;
	};
	struct { //GET_USER_MODULES_SIZE, PLACA->PC 
		unsigned :8;
		byte size;
	};
	struct { //GET_USER_MODULES_LINE, PC->PLACA 
		unsigned :8;
		byte line;
	};	
	struct { //GET_USER_MODULES_LINE, PLACA->PC
		unsigned :8;
		char lineName[8];
	};		
	struct { //INIT, PLACA->PC
		unsigned :8;
	};		
	struct { //INIT, PLACA->PC
		unsigned :8;
	};		
} AM_PACKET;

/** P U B L I C  P R O T O T Y P E S *****************************************/
void adminModuleInit(void);
void adminReceived(byte* recBuffPtr,byte len);
void sendMes(char* mensaje, byte len);
#endif //ADMIN_MODULE_H
