/* Author             									  Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Andres Aguirre									    14/06/07    Original.
 *****************************************************************************/

#ifndef STEP_MOTOR_TIMER
#define STEP_MOTOR_TIMER

/** I N C L U D E S **********************************************************/
#define moutA1	PORTDbits.RD0
#define moutA2	PORTDbits.RD1
#define moutA3	PORTDbits.RD2
#define moutA4	PORTDbits.RD3

#define moutB1	PORTDbits.RD4
#define moutB2	PORTDbits.RD5
#define moutB3	PORTDbits.RD6
#define moutB4	PORTDbits.RD7

#define STEP_MIN 2000
#define STEP_MAX 10000
#define STEP_REGULATION_DELAY 2000
#define STEP_REGULATION_INCREMENT 50

/** D E F I N I T I O N S ****************************************************/

//phaseArray[phasetype][phaseposition][outputnumber];
//phasetype:- 0 = wave ; 1 = full step ; 2 = half step
char phaseArray[3][8][4]={
{{1,0,0,0},{1,0,0,0},
{0,0,1,0},{0,0,1,0},
{0,1,0,0},{0,1,0,0},
{0,0,0,1},{0,0,0,1}},

{{1,0,1,0},{1,0,1,0},
{0,1,1,0},{0,1,1,0},
{0,1,0,1},{0,1,0,1},
{1,0,0,1},{1,0,0,1}},

{
{1,0,0,0},
{1,0,1,0},
{0,0,1,0},
{0,1,1,0},
{0,1,0,0},
{0,1,0,1},
{0,0,0,1},
{1,0,0,1}
}
};


/** S T R U C T U R E S ******************************************************/

typedef struct  {
	byte phasetype;
	byte phaseposition;
	signed int stepswaiting;
	unsigned int delaytime;
	unsigned int targetdelaytime;
} Motor;

/** P U B L I C  P R O T O T Y P E S *****************************************/



#endif //
