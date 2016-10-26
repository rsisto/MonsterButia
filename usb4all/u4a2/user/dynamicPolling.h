#ifndef DYNAMIC_POLLING_H
#define DYNAMIC_POLLING_H

/** D E F I N I T I O N S ****************************************************/
#define MAX_POLLING_FUNCTIONS 0x05

/** I N C L U D E S **********************************************************/

/** P U B L I C  P R O T O T Y P E S *****************************************/
BOOL addPollingFunction(void (*pollingFun) (void));
BOOL removePoolingFunction(void (*pollingFun) (void));
void initPollingFunctions(void);
void polling(void);

#endif


