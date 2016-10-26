#ifndef T0_PROXY_H
#define T0_PROXY_H

/** D E F I N I T I O N S ****************************************************/
#define MAX_T0_FUNCTIONS 0x05

typedef struct  {
	int timestamp;
	void (*callback)(void);
} Entry;

/** I N C L U D E S **********************************************************/

/** P U B L I C  P R O T O T Y P E S *****************************************/
BOOL addT0Function(volatile void (*ISRFun) (void));
BOOL removeT0Function(volatile void (*ISRFun) (void));
void initT0Functions(void);
void configT0(unsigned char, unsigned char);
void t0Interrupt(void);

#endif


