
/* teoría del cuentavueltas:
   rango de presentP: 30º = 0; 330º = 1023
   muestreando a 25Hz, entre una lectura y otra no puede recorrer más de 12 grados (=41 pasos) 
*/

void sample () {                                        // rutina que se ejecuta periódicamente para actualizar la lectura de los sensores
  static byte estado = 0;                                                                             
  
  estado &= 1;  
  
  // lee posición, velocidad y carga de 1 motor
  int psl [2];
  motor [estado].presentPSL (psl);
  int lastP = presentP [estado];
  presentP [estado] = psl [0]; 
  presentS [estado] = psl [1];
  presentL [estado] = psl [2];
  
  // cuentapasos
  if (pasos [estado] > 0) {
    float delta = (lastP - presentP[estado]) * sign (presentS[estado]);                                 // esta fórmula es para no usar valor absoluto, así el error se promedia solo
    // si está en la "zona ciega" o si esta llegando al final, pasa al modo "velocidad"
    if (delta<1 || delta>41 || pasos[estado]<=60) {delta = abs (presentS[estado] * VEL_CONST);}         
    pasos[estado] = max (pasos[estado]-delta, 0);
  }
  
  // lectura de otros sensores
  // podría haber incluso una manera de activarlos o no, 
  // para que no esté perdiendo el tiempo con lecturas innecesarias
  
  estado ++;                                                             // incrementa estado
}

void motor_init () {
  
  for (int i=0; i<2; i++) {
    motor[i].writeInfo (TORQUE_ENABLE, 1);               // habilita el torque
    motor[i].setEndlessTurnMode(true);                   // lo pone en modo de rotación continua
   // setear la inercia y todo eso
    motor[i].writeInfo (CW_COMPLIANCE_MARGIN, 0);
    motor[i].writeInfo (CCW_COMPLIANCE_MARGIN, 0);
    motor[i].writeInfo (CW_COMPLIANCE_SLOPE, 95);
    motor[i].writeInfo (CCW_COMPLIANCE_SLOPE, 95);
    motor[i].writeInfo (PUNCH, 150);
    motor[i].writeInfo (MAX_TORQUE, 1023);
    motor[i].writeInfo (LIMIT_TEMPERATURE, 85);
    motor[i].writeInfo (DOWN_LIMIT_VOLTAGE, 60);
    motor[i].writeInfo (DOWN_LIMIT_VOLTAGE, 190);
    motor[i].writeInfo (RETURN_DELAY_TIME, 150);
   // test motor
    motor[i].endlessTurn (500); delay (500); 
    motor[i].endlessTurn (-500); delay (500);
    motor[i].endlessTurn (0);
  }
  
}

void display_init () {      // este scroll lo podríamos generalizar a cualquier texto
  lcd.begin (16, 2);
  lcd.clear (); 
  //TODO revisar como poner asyncronico, temporalmente sacado
  /*lcd.print (" PROYECTO BUTIA ");
  char * text = "www.fing.edu.uy/inco/proyectos/butia";
  char row [16];
  for (byte i=0; i<21; i++) {
    lcd.setCursor (0, 1);
    memcpy (row , text+i, 16);
    lcd.print (row);
    delay (260 + 800*(!i));
  }*/
}

void movimiento (byte direccion, int velocidad) {
  motor[0].endlessTurn (-velocidad*bin2sign(direccion&1));
  motor[1].endlessTurn (-velocidad*bin2sign(direccion>>1));      
} 

char sign (int numero) {
  char signo = 0;
  if (numero > 0) {signo = 1;}
  if (numero < 0) {signo = -1;}
  return signo;
}

