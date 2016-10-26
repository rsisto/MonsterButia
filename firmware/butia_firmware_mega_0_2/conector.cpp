#include "conector.h"
#include "wiring.h"

const int Conector::values[NUM_VALORES] = {VAL_0, VAL_1, VAL_2, VAL_3, VAL_4, VAL_5, VAL_6, VAL_7, VAL_8, VAL_9, VAL_A, VAL_B, VAL_C};

Conector::Conector (byte _pin_id0, byte _pin_id1, byte _pin_dig0, byte _pin_dig1, byte _pin_analog) {
  pin_id0 = _pin_id0;
  pin_id1 = _pin_id1;
  pin_dig0 = _pin_dig0;
  pin_dig1 = _pin_dig1;
  pin_analog = _pin_analog;
  pinMode (pin_id0, INPUT); 
  pinMode (pin_id1, INPUT);
  digitalWrite (pin_id0, HIGH);          // activa los pull-ups
  digitalWrite (pin_id1, HIGH);          // activa los pull-ups
  update_config ();
}

Conector::Conector () { }

byte Conector::get_type () {
   return type;
}
    
byte Conector::get_subtype ()  {
   return subtype;
}

int Conector::input () {
   switch (type) {
     case 0: case 1: case 2:
       return analogRead(pin_analog);
       break;
     case 3:
       return digitalRead(pin_dig0)+2*digitalRead(pin_dig1);
       break;
     case 4:
       return digitalRead(pin_dig1);
       break;
     case 5:
       return -1;
       break;     
   }
}

int Conector::input (boolean bit) {
   switch (type) {
     case 1: case 2: case 5:
       return -1;
       break;
     case 0: case 3:
       if (bit) {return digitalRead(pin_dig1);}
       else {return digitalRead(pin_dig0);}
       break;
     case 4:
       if (bit) {return digitalRead(pin_dig1);}
       else {return -1;}
       break;    
   }
}

boolean Conector::output (byte value) {
   switch (type) {
     case 0: case 1: case 3:
       return false;
       break;
     case 2: case 4:
       if (value>1) {return false;} else {
         digitalWrite(pin_dig0, value);
         return true;
       }
       break;
     case 5:
       if (value>3) {return false;} else {
         digitalWrite(pin_dig0, value&1);
         digitalWrite(pin_dig1, (value<<1)&1);
       }
       return true;
       break;    
   }        
}

boolean Conector::output (boolean bit, boolean value) {  
   switch (type) {
     case 0: case 1: case 3:
       return false;
       break;
     case 2: case 4:
       if (bit) {return false;} else {
         digitalWrite(pin_dig0, value);
         return true;
       }
       break;
     case 5:
       if (bit) {digitalWrite(pin_dig1, value);}
       else {digitalWrite(pin_dig0, value);}
       return true;
       break;    
   }
}

boolean Conector::dual_output (boolean value0, boolean value1) {
   switch (type) {
     case 0: case 1: case 2: case 3: case 4:
       return false;
       break;
     case 5:
       digitalWrite(pin_dig1, value1);
       digitalWrite(pin_dig0, value0);
       return true;
       break;    
   }
}

boolean Conector::pwm_output (byte value) {  
   switch (type) {
     case 0: case 1: case 3:
       return false;
       break;
     case 2: case 4: case 5:
       analogWrite(pin_dig0, value);
       return true;
       break;
   }    
}


void Conector::update_config () {
  byte id = digitalRead (pin_id0) + 2*digitalRead (pin_id1);
  switch (id) {
    case 3:                                                      // NADA       
      pinMode (pin_dig0, INPUT); 
      pinMode (pin_dig1, INPUT);
      type = 0;
      subtype = 0;
      break;   
    case 2:                                                      // sensor analógico       
      pinMode (pin_dig0, INPUT); 
      pinMode (pin_dig1, INPUT);
      digitalWrite (pin_dig0, HIGH);             // activa los pull-ups
      digitalWrite (pin_dig1, HIGH);             // activa los pull-ups
      type = 1;
      subtype = digitalRead (pin_dig0) + 2*digitalRead (pin_dig1);
      break;         
    case 1:                                                      // sensor analógico c/pin de control       
      pinMode (pin_dig0, OUTPUT); 
      pinMode (pin_dig1, INPUT);
      digitalWrite (pin_dig1, HIGH);             // activa los pull-ups
      type = 2;
      subtype = digitalRead (pin_dig1);
      break;
    case 0:                                                     // sensor o actuador digital
    {  
      int analog_id = analogRead (pin_analog);   
      byte i;
      for (i=0; i<NUM_VALORES; i++) {
          if (abs(analog_id-values[i]) <= TOLERANCIA) {break;}
      } 
      switch (i) {
        case 0: case 1: case 2: case 3: case 4:
          pinMode (pin_dig0, INPUT); 
          pinMode (pin_dig1, INPUT);
          type = 3;                                             // sensor digital
          subtype = i;
          break;        
        case 5: case 6: case 7: case 8:
          pinMode (pin_dig0, OUTPUT); 
          pinMode (pin_dig1, INPUT);
          type = 4;
          subtype = i-5;
          break;
        case 9: case 10: case 11: case 12:
          pinMode (pin_dig0, OUTPUT); 
          pinMode (pin_dig1, OUTPUT);
          type = 5;                                            // sensor digital c/pin de control
          subtype = i-9;
          break;
        case NUM_VALORES:              // si la red de resistencias no coincide con ningun valor, se deja en modo manual
          pinMode (pin_dig0, INPUT); 
          pinMode (pin_dig1, INPUT);
          type = 0;
          subtype = 0;
          break;   
      }  
    }
  }     
}       
   
       
