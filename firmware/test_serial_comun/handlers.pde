
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
    case 2:          // get_version
      buffer [1] = VERSION;
      //digitalWrite (13, HIGH);
      //buffer [1] = 'a';
      sendMsg(1,buffer,2);
      //sendMsg (1, buffer, 2);
      break;
    case 3:                                          // get_voltage
 //     motor[0].readInfo (PRESENT_VOLTAGE);
  //    buffer [1] = motor[0].status_data;
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
  byte instruccion = buffer [0];
  switch (instruccion) {
    case 0:                                          // set_velocidad
      
      break;
    case 1:                                          // set_angulo
      
      break;
  }
  sendMsg (3, buffer, 1);                            // ACK
  return 0;
} 


byte sensores (byte* buffer, byte largo) {               // modulo generico para sensor analogico (handler 4)
  byte instruccion = buffer [0];
  switch (instruccion) {
    case 0:                                                           // get_value
      int value = analogRead (0);
      byte data [3] = {instruccion, highByte(value), lowByte(value)}; 
      sendMsg (4, data, 3);                                           // respuesta
      break;
  }
  return 0;
} 

byte lback (byte* buffer, byte largo) {    // repite lo que llega, para testear la comunicacion (handler 5)
   sendMsg (5, buffer, largo);
   return 0;   
}
