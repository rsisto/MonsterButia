/* 
	jvisca@fing.edu.uy
*/

#ifndef T0_SERVICE_H
#define T0_SERVICE_H

/** D E F I N I T I O N S ****************************************************/

//max number of registrations to hold
#define MAX_T0CALLS 10

//default clock initializacion (comment out if undesired)
#define DEFAULT_CLOCK_CONFIG	0x07

/** I N C L U D E S **********************************************************/

/** P U B L I C  P R O T O T Y P E S *****************************************/

/*
	Registers a new event callback, to be called t time units in the future.
	This function must not be called from a callback handler.
*/
BOOL registerT0event(unsigned int t, void (*callback)(void));

/*
	Registers a new event callback, to be called t time units in the future.
	This function only can be called from a callback handler.
*/
BOOL registerT0eventInEvent(unsigned int t, void (*callback)(void));

/*
	Unregister a event callback. No pending event(s) will invoke it.
*/
BOOL unregisterT0event(void (*callback)(void));

/*
	Initialize the service
*/
void initT0Service(void);

#endif


