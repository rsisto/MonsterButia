/*
 * butiá arduino firmware
 * version:     0.1.1
 * date:        8-3-2010
 * language:    Arduino 0018
 * Authors:	Andres Aguirre, Pablo Gindel, Jorge Visca, Gonzalo Tejera, Santiago Margni, Federico Andrade
 *
 * (c) MINA Group, Facultad de Ingeniería, Universidad de la República, Uruguay. 
*/

//#include <NewSoftSerial.h>                          // biblioteca para serial virtual (muy avanzada)
//#include <ax12.h>                                   // biblioteca para manejar motores dynamixel (bastante avanzada)
//#include <MsTimer2.h>                             // biblioteca para generar interrupciones periódicas

// defines
#define SYNC          0xFF                          // byte de comienzo de los mensajes seriales
#define ESCAPE        0xFE                          // byte de desambiguación en los mensajes seriales
#define SAMPLE_CONST  20                            // período de muestreo (o polling) de sensores
#define VEL_CONST     0.07692                       // coeficiente de velocidad (depende del período y del voltaje)
#define NUM_MODULES   5                             // numero de handlers
#define VERSION       11                            // version del firmware 

// objetos
//NewSoftSerial serial (2, 3);                        // asigna los pines 2,3 para recibir y mandar mensajes al PC
//AX12 motor[2] = {AX12 (), AX12 ()};                 // define 2 motores AX12

// variables de los sensores y motores
volatile int presentP [] = {0, 0};
volatile int presentS [] = {0, 0};
volatile int presentL [] = {0, 0};
volatile float pasos[] = {-1, -1};

// array de puneros a las funciones "handler"
byte (* handler[]) (byte* buffer, byte largo) = {sistema, butia, tortuga, motores, sensores, lback};    // funciones "handler" 

// storage class para la "máquina de estados" de recepción
enum ESTADO { BUSCAR, LEER_LARGO, LEER_MSG, CHECKSUM }; 

char* nombres [] = {"butia", "tortuga", "motor", "sensor", "lback"};


void setup() {   
  Serial.begin (57600);                             // inicializa el SoftSerial a 115,2 Kb/s
//  AX12::init (1000000);                             // inicializa los AX12 a 1 Mb/s
  int detect[2];                                    // array para detectar automáticamente las ID de 2 motores
//  AX12::autoDetect (detect, 2);                     // detección de IDs
  for (byte i=0; i<2; i++) {
//    motor[i].id = detect[i];                        // asigna las ID detectadas a los motores definidos previamente
  }
//  motor[0].inverse = true;                          // asigna la propiedad "inverse" a uno de los motores
//  motor_init ();                                    // inicializa los motores (torque, rotación continua, etc.)
  pinMode (13, OUTPUT);                             // LED onboard para diagnóstico
  
  //MsTimer2::set(50, sample);                        // lectura de sensores cada 50ms (a 300º/s de velocidad máxima, en 100ms no puede recorrer más de 30º)
  //MsTimer2::start();                                // inicializa las interrupciones en el timer2 (por ahora suspendido)
}

// =====================================================================================================================================
// =====================================================================================================================================

void loop () {
  
  static long time_last = 0;                   // esto hace las veces de "timer"

  leer_serial ();                              // lee e interpreta mensajes (de a 1 byte) 
   
  // esto sustituye a las interrupciones, por ahora
  long time_act = millis();
  if (time_act-time_last >= SAMPLE_CONST) {               // cada 20ms llamamos a la sample(). Ojo porque esto afecta al "cuentapasos"
    sample(); 
    time_last = time_act;
  }
  
  // timer para los movimientos por pasos (esto también podría estar activo o no)
  for (byte i=0; i<2; i++) {
    if (pasos [i] == 0) {
 //     motor[i].writeInfo (MOVING_SPEED, 0);
      // aca vendría un sendMsg para avisar arriba que se frenó
      byte data[] = {20+i};
      sendMsg (2, data, 1);     // investigar que pasa aca
      pasos [i] = -1;
    }
  }
  
     
}

// =====================================================================================================================================
// =====================================================================================================================================



  
