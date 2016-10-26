/* Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Andres Aguirre       07/04/07    Original.
 ********************************************************************/
#ifndef DESCRIPTOR_MANAGER_H
#define DESCRIPTOR_MANAGER_H
/** I N C L U D E S **********************************************************/
#include "system\typedefs.h"

/** D E F I N I T I O N S ****************************************************/

#define nullEP 0x11111111

/** S T R U C T U R E S ******************************************************/

typedef union _endpoint{
	byte endPoint;       // Acceso crudo del  byte
	struct {             // Acceso de a bits a cada campo
      unsigned EPNum:4;              
      unsigned EPDir:1;  // 1 1n 0 0ut            
	  unsigned empty:1;  // criptico pero eficiente ;) 1 byte ahorrado es un byte ganado
	  enum {
		INT  = 0x01,
		ISO  = 0x02,
		BULK = 0x03
	  }type;
	};
}endpoint;

/** P U B L I C  P R O T O T Y P E S *****************************************/
endpoint getAdminEndpoint(void);
#endif //ADMIN_MODULE_H
