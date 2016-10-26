/* 
	jvisca@fing.edu.uy
*/

#ifndef STEP_MOTOR_TIMER
#define STEP_MOTOR_TIMER

/** I N C L U D E S **********************************************************/


/** D E F I N I T I O N S ****************************************************/

#define MOTOR_TIMER_MINOR_VERSION   0x01
#define MOTOR_TIMER_MAJOR_VERSION   0x01

#define moutA1	PORTDbits.RD0
#define moutA2	PORTDbits.RD1
#define moutA3	PORTDbits.RD2
#define moutA4	PORTDbits.RD3

#define moutB1	PORTDbits.RD4
#define moutB2	PORTDbits.RD5
#define moutB3	PORTDbits.RD6
#define moutB4	PORTDbits.RD7

#define mout	PORTD

#define STEP_MIN 2000
#define STEP_MAX 10000
#define STEP_REGULATION_DELAY 4000
#define STEP_REGULATION_INCREMENT 100

#define DIR_FORWARD	1
#define DIR_BACKWARD	-1
#define DIR_STOP	0

#define WAVE_STEP	0
#define FULL_STEP	1
#define HALF_STEP	2

#define POWER_ON	1
#define POWER_OFF	0

//phaseArray[phasetype][phaseposition][outputnumber];
//phasetype:- 0 = wave ; 1 = full step ; 2 = half step
byte phaseArray[3][8]={
	{
		0b00001000,0b00001000,
		0b00000010,0b00000010,
		0b00000100,0b00000100,
		0b00000001,0b00000001
	},{
		0b00001010,0b00001010,
		0b00000110,0b00000110,
		0b00000101,0b00000101,
		0b00001001,0b00001001
	},{
		0b00001000,
		0b00001010,
		0b00000010,
		0b00000110,
		0b00000100,
		0b00000101,
		0b00000001,
		0b00001001
	}
};

unsigned int max_step_length=STEP_MAX;
unsigned int step_regulation_delay=STEP_REGULATION_DELAY;
unsigned int step_regulation_increment=STEP_REGULATION_INCREMENT;

/** S T R U C T U R E S ******************************************************/

typedef struct  {
	byte phasetype;
	signed char phaseposition;
	byte power_on;
	signed char direction;
	signed char targetdirection;
	unsigned int stepswaiting;
	unsigned int delaytime;
	unsigned int targetdelaytime;
} Motor;

typedef union MOTORTMR_DATA_PACKET
{
    byte _byte[USBGEN_EP_SIZE];  //For byte access
    word _word[USBGEN_EP_SIZE/2];//For word access(USBGEN_EP_SIZE msut be even)
    struct
    {
        enum
        { 
		READ_VERSION    = 0x00,
		T0CFG		= 0x01,
		STEP_A		= 0x02,
		STEPS_A		= 0x03,
		PHASETYPE_A	= 0x04,
		POWERON_A	= 0x06,
		DIR_A		= 0x07,
		GETSTATE_A	= 0x08,
		STEP_B		= 0x09,
		STEPS_B		= 0x10,
		PHASETYPE_B	= 0x11,
		POWERON_B	= 0x12,
		DIR_B		= 0x13,
		GETSTATE_B	= 0x14,
		MAX_STEP	= 0x15,
		REGULATION_DELAY= 0x16,
		REGULATION_INC	= 0x17,

		MESS		= 0x05,
		RESET		= 0xFF //backward compatibility
        }CMD;
        byte len;
    };
    struct
    {
        unsigned :8;
        byte ID;
    };
    struct
    {
        unsigned :8;
        byte led_num;
        byte led_status;
    };
    struct
    {
        unsigned :8;
        word word_data;
    };
} MOTORTMR_DATA_PACKET;

/** P U B L I C  P R O T O T Y P E S *****************************************/



#endif //
