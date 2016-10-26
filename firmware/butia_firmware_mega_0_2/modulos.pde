// esto se va a volver más parecido a USB4ALL
// entre el mensaje y los módulos va a haber una capa intermedia que son las "instancias" de los módulos
// con módulos que dependen de la autodetección, y otros que están siempre
// si viene un mensaje hacia un módulo que no está "instanciado" se devuelve un error


/********************************************************************************************************************************/
/*                                                  módulos "fijos" (no PnP)                                                    */
/********************************************************************************************************************************/

byte sistema (byte* buffer, byte largo, byte handler_id) {                     // este es el administrador del protocolo
                                                                               // "open" "close" etc. (es el "Admin" de USB4ALL)
  byte instruccion = buffer [0];
  switch (instruccion) {
    case 0:                                          // open 
      char name [8];
      strcpy (name, (char*)buffer+3);
      byte i;
      for (i=0; i<num_modules; i++) {
        if (!strcmp (name, handler[i+1].nombre)) {break;}          
      }
      if (i==num_modules) {i=254;}
      buffer [1] = i+1;
      sendMsg (handler_id, buffer, 2);     
      break;
    case 1:                                          // close
      sendMsg (handler_id, buffer, largo);
      break;
    case 7:                                          // close all
      sendMsg (handler_id, buffer, 1);
      break;
    case 5:                                          // get user modules size  
      buffer [1] = num_modules;
      sendMsg (handler_id, buffer, 2);
      break;
    case 6:{                                          // get user modules line
      byte respuesta [9];
      respuesta[0] = buffer[0];
      strcpy ((char*)respuesta+1, handler[buffer[1]+1].nombre);
      sendMsg (handler_id, respuesta, 9);
      break;
    }
    case 8:{                                       // get user modules type
      byte respuesta [2];
      respuesta[0] = buffer[0];
      if (handler[buffer[1]+1].num_conector != 255){
        respuesta[1]= conector[handler[buffer[1]+1].num_conector].get_type();
      }else{
        respuesta[1]= 0;
      }
      sendMsg (handler_id, respuesta, 2);
      break;
    }  
    case 9:{                                          // get user modules subType
      byte respuesta [2];
      respuesta[0] = buffer[0];
      if (handler[buffer[1]+1].num_conector != 255){
        respuesta[1] = conector[handler[buffer[1]+1].num_conector].get_subtype();
      }else{
        respuesta[1] = 0;
      }
      sendMsg (handler_id, respuesta, 2);
      break;
    }  
    case 10:{                                         // get user modules connector
       byte respuesta[2];
      respuesta[0] = buffer[0];
      respuesta[1] = handler[buffer[1]+1].num_conector;
      sendMsg (handler_id, respuesta, 2);
      break;
    }   
  }
  return 0;  // ACK
} 


//este módulo se encarga de implementar funcionalidades propias del robot
byte butia (byte* buffer, byte largo, byte handler_id) {                       // este es el administrador del robot
  byte instruccion = buffer [0];
  switch (instruccion) {      
    case 2:                                          // get_version
      buffer [1] = VERSION;
      sendMsg (1, buffer, 2);
      break;
    case 3:                                          // get_voltage
      motor[0].readInfo (PRESENT_VOLTAGE);
      buffer [1] = motor[0].status_data;
      sendMsg (handler_id, buffer, 2);
      break;
  }
  return 0;           // ACK
} 


byte tortuga (byte* buffer, byte largo, byte handler_id) {                         // este módulo permite hacer movimientos controlados  
  byte error;
  byte instruccion;
  word parametro1, parametro2;
  
  if (largo <3) {error = 4;} else {           // comprobación de error del handler: los mensajes tienen que ser de por lo menos 3 bytes
                                              // el significado de los errores sería universal, asi evitamos que el propio handler tenga
                                              // que responder a los mensajes en caso de error. El handler contesta en caso de éxito
                                              // de lo contrario contesta el sistema con un código de error universal
                                              // el handler también contesta si hay un tipo de error que es específico (p. ej. valor fuera de rango)
                                              // en ese caso el valor que retorna es "0" para evitar que el sistema vuelva a contestar
    // interpretación del mensaje
    instruccion = buffer[0];
    parametro1 = word (buffer[1], buffer[2]);
    switch (instruccion) {
      case 0:                                                 // adelante
      case 1:                                                 // atrás
      case 2:                                                 // izquierda
      case 3:                                                 // derecha
        if (largo == 5) {                                     // si hay un parámetro más, son los pasos (medio trucho el sistema)
          parametro2 = 3.413 * word (buffer[3], buffer[4]);   // conversión de grados a "pasos"
          pasos [0] = parametro2;
          pasos [1] = parametro2;
        }
        movimiento (instruccion, parametro1);    // el parámetro es la velocidad
        break;
      case 4:                                    // frenar
        movimiento (0, 0);
        break;
    }
    // contestazione: ACK
    error = 0;
    buffer[1] = error;            
    sendMsg (handler_id, buffer, 2);
  }
  return error;
} 


byte motores (byte* buffer, byte largo, byte handler_id) {             // módulo genérico para los motores del robot
  byte error = 0;
  byte instruccion = buffer [0];
  switch (instruccion) {
    case 0: {                                         // mover 1 motor con velocidad
      if (largo < 5) {error = 4;} else {
        byte motor_id = buffer [1];
        if (motor_id >= NUM_MOTORS) {error = 5;} else {
          char direccion = bin2sign (buffer[2]);
          word velocidad = word (buffer[3], buffer[4]);
          motor[motor_id].endlessTurn (direccion * velocidad);
        }
      }
      break;
    }
    case 1: {                                          // mover 2 motors con velocidad
      if (largo < 7) {error = 4;} else {
         char direccion1 = bin2sign (buffer[1]);
         word velocidad1 = word (buffer[2], buffer[3]);
         char direccion2 = bin2sign (buffer[4]);
         word velocidad2 = word (buffer[5], buffer[6]);
         motor[0].endlessTurn (direccion1 * velocidad1);
         motor[1].endlessTurn (direccion2 * velocidad2);
      }
      break;
    }
    
  }
  if (error==0) {sendMsg (handler_id, buffer, 1); }                         // ACK  
  return error;
} 


byte sensores (byte* buffer, byte largo, byte handler_id) {               // modulo genérico para leer pines de la Arduino así como vienen 
  byte instruccion = buffer [0];
  int  pinIn = buffer [1];
  switch (instruccion) {
    case 0: {                                                                   // get_value
      int value = analogRead (pinIn);
      byte data [3] = {instruccion, highByte(value), lowByte(value)}; 
      sendMsg (handler_id, data, 3);                                            // respuesta
      }      
      break;
    case 1: {
      int value = digitalRead(pinIn);                                           // capturo el valor de entrada del pin
      byte data [3] = {instruccion, highByte(value), lowByte(value)}; 
      sendMsg (handler_id, data, 3);                                            // respuesta     
      }
      break;
  }
  return 0;
} 


// falta: otro módulo genérico pero que permite leer los pines de un conector, haya lo que haya


byte lback (byte* buffer, byte largo, byte handler_id) {             // repite lo que llega, para testear la comunicación 
   sendMsg (handler_id, buffer, largo);
   return 0;   
}


byte ax12 (byte* buffer, byte largo, byte handler_id) {             // pasarela para los ax12 (no hace nada, por ahora, pero lo que debería hacer es mandar mensajes "textuales" hacia los motores)
   sendMsg (handler_id, buffer, largo);
   return 0;   
}

byte lcd_display (byte* buffer, byte largo, byte handler_id) {             // módulo para el display LCD 16x2 (no hace nada, por ahora)
   sendMsg (handler_id, buffer, largo);
   return 0;   
}

// falta: un módulo genérico para motores conectados al conector de potencia. Talvez un mismo módulo para steppers, DC y servos


/********************************************************************************************************************************/
/*                                                           módulos PnP                                                        */
/********************************************************************************************************************************/

byte genericSensorPnP (byte* buffer, byte largo, byte handler_id) {      // módulo genérico para sensor PnP
   byte instruccion = buffer [0];    
   switch (instruccion) {
    case 0: {                                                                   // get_value
      int value = conector[handler[handler_id].num_conector].input();
      byte data [3] = {instruccion, highByte(value), lowByte(value)}; 
      sendMsg (handler_id, data, 3);                                            // respuesta
      }      
      break;
    case 1: 
      break;
  }
  return 0;
}

byte genericActuatorPnP (byte* buffer, byte largo, byte handler_id) {
   byte instruccion = buffer [0];    
   byte value = buffer [1];
   switch (instruccion) {
    case 0: {                                                                   // get_value
      conector[handler[handler_id].num_conector].pwm_output(value);             // valores entre 0 y 255
      sendMsg (handler_id, buffer, 1);                                            // respuesta
      }      
      break;
    case 1: 
      break;
  }
  return 0;
}

// módulo de usuario que maneja el sensor "botón"
byte boton (byte* buffer, byte largo, byte handler_id) {
   byte instruccion = buffer [0];                                           // obtengo el opcode 
   switch (instruccion) {    
    case 0: {                                                               // get_value
      boolean value = conector[handler[handler_id].num_conector].input(0);  // leo de la entrada digital
      byte data [2] = {instruccion, !value};                                // armo el paquete de respuesta (opcode, data)
      sendMsg (handler_id, data, 2);                                        // respuesta
      }      
      break;   
  }
  return 0;
}

byte led (byte* buffer, byte largo, byte handler_id) {
   byte instruccion = buffer [0];    
   float value = 256 - pow (2.0, buffer [1]/32.0);           // fórmula para "linearizar" la respuesta del led
   switch (instruccion) {
    case 0: {     
      conector[handler[handler_id].num_conector].pwm_output(value); 
      sendMsg (handler_id, buffer, 1);                                            // respuesta
      }      
      break;
    case 1: 
      break;
  }
  return 0;
}

// falta: todo el resto de los dispositivos
