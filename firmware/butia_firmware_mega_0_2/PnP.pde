// esto es en escencia el funcionamiento del PnP en el firmware del butiá

void init_conectores () {                            // define los 8 conectores
                                                     // ver: tabla de pines que está en la documentación del PnP
  conector [0] = Conector (42, 43, 4, 26, 0); 
  conector [1] = Conector (44, 45, 5, 27, 1);
  conector [2] = Conector (46, 47, 6, 28, 2);
  conector [3] = Conector (48, 49, 7, 29, 3);      
  conector [4] = Conector (50, 51, 8, 30, 4);        
  conector [5] = Conector (52, 53, 9, 31, 5);
  conector [6] = Conector (22, 23, 10, 32, 6);
  conector [7] = Conector (24, 25, 11, 33, 7);   
}

void init_handlers () {                  // define todos los módulos fijos o genéricos, o sea, los que no son PnP, los que siempre están.
                                         // después desde otro lado se van a agregar los que resulten de la auto-detección (módulos PnP)
                                         // obsérvese que a estos módulos no se les define el num_conector, porque nunca lo van a usar (queda seteado a 255 por defecto)
  
                                            handler[0].funcion = sistema;
  strcpy (handler[1].nombre, "butia");      handler[1].funcion = butia;
  strcpy (handler[2].nombre, "tortuga");    handler[2].funcion = tortuga;
  strcpy (handler[3].nombre, "motores");    handler[3].funcion = motores;
  strcpy (handler[4].nombre, "sensor");     handler[4].funcion = sensores;
  strcpy (handler[5].nombre, "lback");      handler[5].funcion = lback;
  strcpy (handler[6].nombre, "ax12");       handler[6].funcion = ax12;
  strcpy (handler[7].nombre, "lcd");        handler[7].funcion = lcd_display;

  num_modules = 7;  // 1 menos que la cantidad de handlers, porque para USB4ALL, el handler[0] (sistema) no se cuenta (y por eso no tiene nombre tampoco)
  
  for (byte i=0;i< num_modules +1;i++ ){
    handler[i].num_conector = 255;              // 255 significa que no tiene conector específico (sería como un "broadcast")
  }

}

// devuelve un nombre y una fución manejadora para el módulo que se está agregando, segun sea su 'globaltype' (tipo*10 + subtipo)
// acá van definidos todos los tipos de dispositivos soportados por el PnP
// cada 'case' tiene un contador de instancias que se retorna con la función 

byte get_config (byte globaltype, char* nombre, byte (** funcion) (byte*, byte, byte)) {
  byte num_instances;
  switch (globaltype) {
    
    case SENSOR_DISTANCIA:
    {
      static byte num = 0;
      strcpy (nombre, "dist");
      *funcion = genericSensorPnP; 
      num_instances = num++;
      break;
    }
    case SENSOR_TEMPERATURA:
    {
      static byte num = 0;
      strcpy (nombre, "temp");
      *funcion = genericSensorPnP; 
      num_instances = num++;
      break;
    }
    case SENSOR_LUZ:
    {
      static byte num = 0;
      strcpy (nombre, "luz");
      *funcion = genericSensorPnP; 
      num_instances = num++;
      break;
    }
    case SENSOR_GRISES:
    {
      static byte num = 0;
      strcpy (nombre, "grises");
      *funcion = genericSensorPnP; 
      num_instances = num++;
      break;
    }
    case SENSOR_BOTON:
    {
      static byte num = 0;
      strcpy (nombre, "boton");
      *funcion = boton; 
      num_instances = num++;
      break;
    }
    case SENSOR_CONTACTO:
    {
      static byte num = 0;
      strcpy (nombre, "ctouch");
      *funcion = genericSensorPnP; 
      num_instances = num++;
      break;
    }
    case SENSOR_TILT:
    {
      static byte num = 0;
      strcpy (nombre, "tilt");
      *funcion = genericSensorPnP; 
      num_instances = num++;
      break;
    }
    case SENSOR_VIBRACION:
    {
      static byte num = 0;
      strcpy (nombre, "vibra");
      *funcion = genericSensorPnP; 
      num_instances = num++;
      break;
    }
    case SENSOR_MAGNETICO:
    {
      static byte num = 0;
      strcpy (nombre, "magnet");
      *funcion = genericSensorPnP; 
      num_instances = num++;
      break;
    }
    case SENSOR_POTE:
    {
      static byte num = 0;
      strcpy (nombre, "pote");
      *funcion = genericSensorPnP; 
      num_instances = num++;
      break;
    }
    case ACTUADOR_LED:
    {
      static byte num = 0;
      strcpy (nombre, "led");
      *funcion = led; 
      num_instances = num++;
      break;
    }
    default:
    {
      static byte num = 0;
      strcpy (nombre, "unknown");
      *funcion = lback; 
      num_instances = num++;
      break;
    }
  }
  return num_instances; 
}


void add_module (byte num_conector) {       // agrega un handler para el dispositivo conectado a num_conector
  
  byte globaltype = 10*conector[num_conector].get_type() + conector[num_conector].get_subtype();
  char nombre[8];
  byte (* funcion) (byte*, byte, byte);
  byte instance = get_config (globaltype, nombre, &funcion);
  if (instance != 0) {                    // si la instancia es 0, se devuelve el nombre por defecto del módulo;
                                          // de lo contrario el nombre incluye el número de instancia.
    byte largo = strlen (nombre);
    if (largo > 6) {largo--;}
    nombre[largo] = '0' + instance;       // le agrega el ASCII del número de instancia
    nombre[largo+1] = 0;                  // terminación
  }
  // y ahora agregamos el módulo
  num_modules ++;
  strcpy (handler[num_modules].nombre,  nombre);
  handler[num_modules].funcion = funcion;
  handler[num_modules].num_conector = num_conector;

}

  

