
void leer_serial () {                  // esta es la "máquina de estados" que recibe y decodifica los mensajes seriales 
  
  // variables locales estáticas
  static byte largo, cont, checksum;
  static byte anterior = 0;    
  static byte *buffer = NULL;  
  static ESTADO estado = BUSCAR;
    
  if (serial.available() > 0) {                                                     // si hay al menos 1 byte en el serial...
    byte b = serial.read();                                                         // ...lo lee
    if (b==SYNC && ((anterior!=ESCAPE && estado==BUSCAR) || anterior<128)) {        // chequea la presencia de un "TRUE SYNC"
      if (buffer != NULL) {                                                         
        free (buffer);                                                              // prepara para inicializar el buffer
        buffer = NULL;
      }
      estado = LEER_LARGO; 
    } else {
      switch (estado) {
        case LEER_LARGO:
          largo = b;
          buffer = (byte*)malloc(largo);                                    // inicializa el buffer segun el largo del mensaje
          checksum = 0;                                                     // se prepara para leer el cuerpo del mensaje
          cont = 0;
          estado=LEER_MSG; 
          break;
        case LEER_MSG:
          buffer[cont++] = b;                                               // va llenando el buffer
          checksum += b;                                                    // va efectuando la suma comprobatoria
          if (cont == largo) {                                              // ...hasta que termina el cuerpo del mensaje
            checksum &= 127;                                                // "recorta" el checksum
            estado = CHECKSUM;
          }
          break;
        case CHECKSUM:
          byte error;
          if (b == checksum) {                                              // comprueba el checksum
            digitalWrite (13, HIGH);                                        // prende led
            largo = unescape (buffer, largo);                               // esta rutina saca los ESCAPEs. El resultado queda en las mismas variables buffer y largo           
            error = dispatch (buffer, largo);                               // la "dispatch" se encarga de entregar el mensaje al handler adecuado, y devuelve su propio error o el del handler
            digitalWrite (13, LOW);                                         // apaga led
          } else {
            error = 1;                                                      // error de checksum
          }
          // senMsg responder notificando error
          // si el error es 0 no se envía, porque en ese caso el propio handler es el encargado de contestar
          if (error != 0) {
            buffer[0] = CODE_ERROR;                  // 255 es el codigo de operacion "error"
            buffer[1] = error;
            sendMsg (0, buffer, 2);  
          }
          estado = BUSCAR;                                                  // estado inicial
          break;  
      }
    }
    anterior = b;                                                           // se prepara para procesar el siguiente byte
  } 
}


void sendMsg (byte handler, byte *msg, byte largo) {      // formatea, encapsula y manda los mensajes al serial (o sea: el input es el mensaje propiamente dicho y su largo)
  
  // primera parte: formatea el mensaje USB4ALL
  byte mensaje [largo+3];
  memcpy (mensaje+3, msg, largo);
  largo += 3;
  mensaje [0] = handler<<3;
  mensaje [1] = largo;
  mensaje [2] = 0;
  
  // segunda parte: escapea  
  byte salida [largo*2];                                         // crea un array suficientemente largo para contener el string de salida
  byte pos = 0;                                                  // posición en el array de salida
  for (byte i=0; i<largo; i++) {                                 // recorre el array de entrada
    if (mensaje[i] < ESCAPE) {                                   // si no es un byte especial...
      salida[pos++] = mensaje[i];                                // ...lo mete como viene en el array de salida e incrementa posición
    } else {                                                     // de lo contrario...
      salida[pos++] = ESCAPE;                                    // mete primero un ESCAPE e incrementa
      salida[pos++] = mensaje[i];                                // y luego mete el byte e incrementa
    }
  }   
  
  // tercera parte: encapsula y manda al serial
  serial.print (SYNC, BYTE);                             // escribe un SYNC
  serial.print (pos);                                    // escribe el largo
  byte checksum = 0;                                     // inicializa el acumulador de suma comprobatoria
  for (byte i=0; i<pos; i++) {                           // recorre el array de salida
    serial.print (salida[i]);                            // escribe el byte
    checksum += salida[i];                               // suma
  }
  serial.print (checksum & 127, BYTE);                   // escribe la suma comprobatoria

}

byte unescape (byte* buffer, byte largo) {     // saca los ESCAPES y devuelve el mensaje original
  byte i=0, pos=0;                                       // inicializa posición del array de entrada y salida (que son el mismo)
  do {
    if (buffer[i] == ESCAPE) {i++;}                      // si encuentra un ESCAPE, lo saltea
    buffer[pos++] = buffer[i++];                         // copia el byte correcto a la nueva posición
  } while (i < largo);
  return pos;                                            // devuelve el nuevo largo
}

byte dispatch (byte *buffer, byte largo) {     // verifica la validez del mensaje USB4ALL y lo despacha al handler. Devuelve el código de error
  int error = 0;                               
  byte destino, len;                                   // variables para la decodificación 
  if (largo < 4) {error = 2;} else {                   // 1a. comprobación de error: el mensaje debe tener al menos 1 byte de información
    destino = buffer[0]>>3;                            // el shifteado reglamentario de USB4ALL
    len = buffer[1];   
    if (len != largo) {error = 3;} else {              // 2a. comprobación de error: el largo USB4ALL y el largo del mensaje "unescapeado" deben coincidir
      error = (*handler[destino]) (buffer+3, len-3);   // le pasa al handler el mensaje sin la cabecera
    }
  }
  return error;                                        // retorna el error interno o el regresado por el handler
}
      

