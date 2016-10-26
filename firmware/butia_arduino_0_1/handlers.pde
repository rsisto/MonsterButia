
byte sistema (byte* buffer, byte largo) {                     // "sistema operativo", es el handler 0
                                                              // "open" "close" etc. (es el "Admin" de USB4ALL)
  byte instruccion = buffer [0];
  switch (instruccion) {
    case 0:                                          // open 
      char name [8];
      strcpy (name, (char*)buffer+3);
      byte i;
      for (i=0; i<NUM_MODULES; i++) {
        if (!strcmp (name, nombres [i])) {break;}        
      }
      if (i==NUM_MODULES) {i=254;}
      buffer [1] = i+1;
      sendMsg (0, buffer, 2);     
      break;
    case 1:                                          // close
      sendMsg (0, buffer, largo);
      break;
    case 7:                                          // close all
      sendMsg (0, buffer, 1);
      break;
    case 5:                                          // get user modules size  
      buffer [1] = NUM_MODULES;
      sendMsg (0, buffer, 2);
      break;
    case 6:                                          // get user modules line
      byte respuesta [9];
      respuesta[0]=buffer[0];
      strcpy ((char*)respuesta+1, nombres[buffer[1]]);
      sendMsg (0, respuesta, 9);
      break;
  }
  return 0;  // ACK
} 


byte butia (byte* buffer, byte largo) {                     // administrador del robot, es el handler 1
  byte instruccion = buffer [0];
  switch (instruccion) {      
    case 2:                                          // get_version
      buffer [1] = VERSION;
      sendMsg (1, buffer, 2);
      break;
    case 3:                                          // get_voltage
      motor[0].readInfo (PRESENT_VOLTAGE);
      buffer [1] = motor[0].status_data;
      sendMsg (1, buffer, 2);
      break;
  }
  return 0;           // ACK
} 


byte tortuga (byte* buffer, byte largo) {                         // tortuga, es el handler 2
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
    sendMsg (2, buffer, 2);
  }
  return error;
} 


byte motores (byte* buffer, byte largo) {            // modulo generico para motor dynamixel (handler 3)
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
  if (error==0) {sendMsg (3, buffer, 1); }                         // ACK  
  return error;
} 


byte sensores (byte* buffer, byte largo) {               // modulo generico para sensor analogico (handler 4)
  byte instruccion = buffer [0];
  int  pinIn = buffer[1];
  switch (instruccion) {
    case 0: {                         // get_value
      int value = analogRead (pinIn);
      byte data [3] = {instruccion, highByte(value), lowByte(value)}; 
      sendMsg (4, data, 3);                                  // respuesta
      }      
      break;
      
    case 1:{
      int valdig = digitalRead(pinIn);                                      //capturo el valor de entrada del pin 7;
      byte data [3] = {instruccion, highByte(valdig), lowByte(valdig)}; 
      sendMsg (4, data, 3);                                            // respuesta     
      }
      break;
  }
  return 0;
} 

byte lback (byte* buffer, byte largo) {             // repite lo que llega, para testear la comunicacion (handler 5)
   sendMsg (5, buffer, largo);
   return 0;   
}

byte ax12 (byte* buffer, byte largo) {             // pasarela para los ax12 (handler 6)
   sendMsg (6, buffer, largo);
   return 0;   
}
