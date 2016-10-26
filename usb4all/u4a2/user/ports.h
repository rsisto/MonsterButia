/************************************
 Definitions
 ************************************/

#define input 1
#define output 0

#define true 1
#define false 0

#define o0	0x00
#define o1	0x00
#define o2	0x00
#define o3	0x00
#define o4	0x00
#define o5	0x00
#define o6	0x00
#define o7	0x00
#define i0	0x01
#define i1	0x02
#define i2	0x04
#define i3	0x08
#define i4	0x10
#define i5	0x20
#define i6	0x40
#define i7	0x80

typedef union {
	struct {
		unsigned char Filler1:1;
		unsigned char E:1;
		unsigned char RW:1;
		unsigned char RS:1;
		unsigned char Filler2:4;
	} AsBits;
	unsigned char AsChar;
} BitsPORTA;

typedef union {
	struct {
		unsigned char S3:1;
		unsigned char Filler:7;
	} AsBits;
	unsigned char AsChar;
} BitsPORTB;

typedef union {
	struct {
		unsigned char Filler1:3;
		unsigned char SCK:1;
		unsigned char Filler2:4;
	} AsBits;
	unsigned char AsChar;
} BitsPORTC;

typedef union {
	struct {
		unsigned char Data:4;
		unsigned char Filler:4;
	} AsBits;
	unsigned char AsChar;
} BitsPORTD;

typedef union {
	struct {
		unsigned char Filler1:3;
	} AsBits;
	unsigned char AsChar;
} BitsPORTE;

#define DefaultPORTA (0x00)
#define DefaultPORTB (0x00)
#define DefaultPORTC (0x00)
#define DefaultPORTD (0x00)
#define DefaultPORTE (0x00)

#define DefaultTRISA (o5|i4|o3|o2|o1|o0)
#define DefaultTRISB (o7|o6|o5|o4|o3|o2|o1|i0)
#define DefaultTRISC (i7|o6|o5|i4|o3|o2|i1|i0)
#define DefaultTRISD (o7|o6|o5|o4|o3|o2|o1|o0)
#define DefaultTRISE (o2|o1|o0)

static volatile BitsPORTA bPORTA @ &(PORTA);
static volatile BitsPORTA bCopyPORTA;
static volatile BitsPORTA bTRISA @ &(TRISA);

static volatile BitsPORTB bPORTB @ &(PORTB);
static volatile BitsPORTB bCopyPORTB;
static volatile BitsPORTB bTRISB @ &(TRISB);

static volatile BitsPORTC bPORTC @ &(PORTC);
static volatile BitsPORTC bCopyPORTC;
static volatile BitsPORTC bTRISC @ &(TRISC);

static volatile BitsPORTD bPORTD @ &(PORTD);
static volatile BitsPORTD bCopyPORTD;
static volatile BitsPORTD bTRISD @ &(TRISD);

static volatile BitsPORTE bPORTE @ &(PORTE);
static volatile BitsPORTE bCopyPORTE;
static volatile BitsPORTE bTRISE @ &(TRISE);

