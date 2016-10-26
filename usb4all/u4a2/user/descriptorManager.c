/* Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Andres Aguirre	   07/04/07 	Original
 ********************************************************************/

#include "user\descriptorManager.h"

/** V A R I A B L E S ********************************************************/
#pragma udata

/** P R I V A T E  P R O T O T Y P E S ***************************************/


/** D E C L A R A T I O N S **************************************************/
#pragma code sys

endpoint getAdminEndpoint(void){
	endpoint adminEndpoint;
	adminEndpoint.EPNum = 1;
	adminEndpoint.EPDir = 0;
	adminEndpoint.empty = 0;
	return adminEndpoint;
}


/** EOF descriptorManager.c ***************************************************************/
