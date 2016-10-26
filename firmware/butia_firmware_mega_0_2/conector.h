
// la clase 'Conector' abstrae los conectores butiá PnP, gracias a ella nos podemos olvidar de los pines de Arduino.
// hay que pasarle los pines al instanciar el objeto
// gestiona la auto-configuración
// dispone de métodos para obtener el tipo y subtipo del dispositivo conectado, y para leer y escribir valores en sus pines

#ifndef CONECTOR_H
#define CONECTOR_H

#define NUM_VALORES 13
#define TOLERANCIA 25
#define VAL_0 0
#define VAL_1 131
#define VAL_2 185
#define VAL_3 250
#define VAL_4 335
#define VAL_5 418
#define VAL_6 512
#define VAL_7 605
#define VAL_8 689
#define VAL_9 774
#define VAL_A 839
#define VAL_B 893
#define VAL_C 1023

typedef unsigned char byte;
typedef unsigned char boolean;

class Conector
{
  public:

    Conector (byte _pin_id0, byte _pin_id1, byte _pin_dig0, byte _pin_dig1, byte _pin_analog);
    Conector ();
    byte get_type ();
    byte get_subtype ();
    int input ();
    int input (boolean bit);
    boolean output (byte value);
    boolean output (boolean bit, boolean value);
    boolean dual_output (boolean value0, boolean value1);
    boolean pwm_output (byte value);
    void update_config ();   
    
  private:
    byte pin_id0;
    byte pin_id1;
    byte pin_dig0;
    byte pin_dig1;
    byte pin_analog;
    byte type;
    byte subtype;
    static const int values[];  // esto almacena los valores posibles de la red de resistencias
    
};


#endif
