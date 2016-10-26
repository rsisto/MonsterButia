/*
 * butiá arduino firmware
 * version:     0.2
 * date:        19-6-2010
 * language:    Arduino 0018
 * Authors:	Andrés Aguirre, Pablo Gindel, Jorge Visca, Gonzalo Tejera, Santiago Margni, Federico Andrade
 *
 * (c) MINA Group, Facultad de Ingeniería, Universidad de la República, Uruguay. 
*/

// includes
#include "ax12.h"                                   // clase para los motores Dynamixel AX-12+
#include "conector.h"                               // clase para el conector butiá PnP
#include <Wire.h>                                   // biblioteca para I2C en los pines analog 4 y 5 (en arduino 'classic') o digital 20 y 21 (en la 'mega') 
#include <LiquidCrystal.h>                          // biblioteca para el display LCD 16x2 
//#include <Servo.h>                                // para cuando pinten los servos
//#include <Stepper.h>                              // para cuando pinten los steppers
//#include <MsTimer2.h>                             // biblioteca para generar interrupciones periódicas

// defines
#define SAMPLE_CONST    20                            // período de muestreo (o polling) de sensores
#define VEL_CONST       0.07692                       // coeficiente de velocidad (depende del período y del voltaje)
#define MAX_MODULES     28                            // número máximo de módulos
#define VERSION         20                            // versión del firmware 
#define NUM_MOTORS      2                             // número de motores AX12
#define NUM_CONNECTORS  8                             // número de conectores butiá PnP

// defines de sensores y actuadores
#define SENSOR_DISTANCIA   10
#define SENSOR_TEMPERATURA 11
#define SENSOR_LUZ         12
#define SENSOR_GRISES      13
#define SENSOR_BOTON       30
#define SENSOR_CONTACTO    31
#define SENSOR_TILT        32
#define SENSOR_VIBRACION   33
#define SENSOR_MAGNETICO   34
#define ACTUADOR_LED       53
#define MAX_CALLBACKS      10
#define SENSOR_POTE        21


void (* callbacks[MAX_CALLBACKS]) () ;                   //   array de callbacks
byte next_callback;

// objetos
AX12 motor[NUM_MOTORS] = {AX12 (), AX12 ()};                 // define 2 motores AX12
Conector conector [NUM_CONNECTORS];                          // declara los 8 conectores
LiquidCrystal lcd (36, 255, 37, 38, 39, 40, 41);             // display harcodeado a los pines esos

// variables de los sensores y motores
volatile int presentP [] = {0, 0};
volatile int presentS [] = {0, 0};
volatile int presentL [] = {0, 0};
volatile float pasos[] = {-1, -1};

// la estructura 'H' consta de un nombre, un puntero a función (el handler) y un número de conector
struct H {
  char nombre [8];
  byte (* funcion) (byte*, byte, byte);
  byte num_conector;
};

// array de estructuras 'H'. Este array va tener la lista completa de módulos activos
// en cada entrada está el nombre del módulo, un puntero a la función manejadora (handler), y un número de conector asociado (para módulos PnP)
// obsérvese que nada impide definir varios módulos con el mismo handler, ya que...
// ...cuando llaman al handler le pasan el 'handler_id', que es lo que le permite buscar dentro del array su nombre y su conector asociado, si hace falta.

struct H handler [MAX_MODULES];

byte num_modules;                                   // variable global que indica cuántos módulos están activos

void setup() {   
  
  next_callback = 0;
  
  Serial.begin (115200);                            // inicializa el Serial a 115,2 Kb/s
  
  AX12::init (1000000);                             // inicializa los AX12 a 1 Mb/s
  int detect[2];                                    // array para detectar automáticamente las ID de 2 motores
  AX12::autoDetect (detect, 2);                     // detección de IDs
  for (byte i=0; i<2; i++) {
    motor[i].id = detect[i];                        // asigna las ID detectadas a los motores definidos previamente
  }
  motor[0].inverse = true;                          // asigna la propiedad "inverse" a uno de los motores
  
  display_init ();                                  // inicializa el display y manda el mensaje de bienvenida
  motor_init ();                                    // inicializa los motores (torque, rotación continua, etc.)
  init_handlers ();                                 // define los handlers fijos
  init_conectores ();                               // define e inicializa los conectores
  
  // explora los conectores
  for (byte f=0; f<NUM_CONNECTORS; f++) {
    if (conector[f].get_type() != 0) {add_module(f);}    // si hay algo en el conector, agrega 1 módulo PnP para él
  }  
  
  pinMode (13, OUTPUT);                             // LED onboard para diagnóstico
  Wire.begin();                                     // inicializa la comunicación I2C
  
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
      motor[i].writeInfo (MOVING_SPEED, 0);
      // aca vendría un sendMsg para avisar arriba que se frenó
      byte data[] = {20+i};
      sendMsg (2, data, 1);     // investigar qué pasa aca
      pasos [i] = -1;
    }
  }
  
     
}

// =====================================================================================================================================
// =====================================================================================================================================



  
