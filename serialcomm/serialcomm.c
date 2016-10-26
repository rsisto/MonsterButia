/*
 * Authors:	Andres Aguirre, Pablo Gindel, Jorge Visca, Gonzalo Tejera, Santiago Margini, Federico Andrade
 * MINA Group, Facultad de Ingenieria, Universidad de la Republica, Uruguay.
 *
 * License at file end.
 */

#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "serialcomm.h"
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define ESCAPE 0xFE
#define SYNC   0xFF
#define null   0x00
#define CAGATA_AVOID 2000
#define TIMEOUT -2
#define DEBUG 0
#define VERBOSE 0
#define VERBOSE_TIMEOUT 0
#define PACKET_LEN 10
#define MAX_RETRIES 5

/*constant for consume the buffer*/
#define MAX_BUFFER_RETRIES 30000

int timeout_counter = 0;
struct termios oldoptions;

/*
 *Sends data to a specific serial device, returns -1 if it could not send all the data
*/
int send_data(int handler, unsigned char *data, char *device){
	int rc=0;
	rc = serialport_write(handler, data);
	//printf("Sent: \"%s\"\t to device: %s\n",data,device);
	return rc;
}

/*
 *Sends len bytes of data to a specific serial device, returns -1 if it could not send all the data
*/
int receive_data(int handler, unsigned char *data, int len, int timeout){
	int rc=0;
	rc = serialport_read(handler, data, len, timeout);
	//printf("recibi %i caracteres y receive_data retorna %s \n", len, data);
	return rc;
}

/*
 * Write a string to the serial port, returns -1 in case of error
 *
*/
int serialport_write(int fd, unsigned char* str){
    int len = strlen((char*)str);
    int n = write(fd, str, len);
    if( n!=len )
        return -1;
    return 0;
}


/*
 * Write a string of length len to the serial port, returns -1 in case of error*/
int serialport_write_len(int fd, unsigned char* str, int len){
    int n = write(fd, str, len);
    //printf("se manda un largo %d y se pidio %d\n", n, len);
    if( n!=len ){
	//printf("problemas en el send\n");
        return -1;
    }
    return 0;
}

void timevalfix (struct timeval *tv); /*prototype only, function declared forward*/
/*
 * Read a string from the serial port with a timeout in miliseconds, returns -1 in case of error, -2 in case of tiemout
 *
*/
int serialport_read(int fd, unsigned char* str, int len, int timeout){
    //printf("timeout %i \n", timeout);
    int n;
    fd_set readfs;    /* file descriptor set */
    int retval;
    struct timeval timeout_struct;
    /* set timeout value */
    timeout_struct.tv_usec = timeout * 1000;   /* microseconds */
    timeout_struct.tv_sec  = 0;               /* seconds */
    timevalfix(&timeout_struct);
    FD_ZERO(&readfs);
    FD_SET(fd, &readfs);
    retval = select(fd+1, &readfs, null, null, &timeout_struct); /* es necesario porque se abre el serial con la flag O_NONBLOCK, esto nos permite implementar el TIMEOUT */
    if(retval == 0) {
	timeout_counter++;
	//printf("Ocurrio el timeout %d\n", timeout_counter);
	return TIMEOUT;
    } else if (retval==-1) {
        return -errno;
    }
    n=read(fd, str, len);
	//printf("serialport_read retorna como leidos %i bytes\n", n);
	if (n == -1){
        if (errno == EAGAIN){
            //printf("problemas en serialcomm:read\n");
            usleep(250000);
		}
		else{
			//perror("read()");
		}
	}
    if( n!=len )
        return -1;
    return 0;
}


/*
 *
 * Takes the string name of the serial port (e.g. "/dev/tty.usbserial","COM1")
 * and a baud rate (bps) and connects to that port at that speed and 8N1.
 * opens the port in fully raw mode so you can send binary data.
 * Returns valid fd, or -1 on error.
*/
int serialport_init(const char* serialport, int baud){
    unsigned char echo[] = {0x28, 0x04, 0x00, 0xCC}; //mensaje USB4all conocido, que responde un echo
    unsigned char* respuesta;
    int leidos = 0;
    int fd, retries;
    fd = open(serialport, O_RDWR | O_NOCTTY | O_NONBLOCK);
	//printf("fd after open is %i %i", fd, errno);
    if (fd == -1)  {
            //perror("init_serialport: Unable to open port ");
            return -1;
    }
    struct termios options;
    tcgetattr(fd,&oldoptions); //save last configuration
    options = oldoptions;
    speed_t brate = baud;      //let you override switch below if needed
    switch(baud) {
        case 4800:   brate=B4800;   break;
        case 9600:   brate=B9600;   break;
        #ifdef B14400
        case 14400:  brate=B14400;  break;
        #endif
        case 19200:  brate=B19200;  break;
        #ifdef B28800
        case 28800:  brate=B28800;  break;
        #endif
        case 38400:  brate=B38400;  break;
        case 57600:  brate=B57600;  break;		
        case 115200: brate=B115200; break;
    }

    cfsetispeed(&options, brate);
    cfsetospeed(&options, brate);
    options.c_cflag |= (CLOCAL | CREAD | CS8 | HUPCL ); // CREAD: enable receiving characters CLOCAL: local connection, no modem contol, 8 bits data, 1 bit stop
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag &= ~CRTSCTS;
    
    options.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl
    options.c_iflag &= INPCK; 		  	// enable parity check

    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // make raw
    options.c_oflag &= ~OPOST; // make raw
    
    // see: http://unixwiz.net/techtips/termios-vmin-vtime.html
    options.c_cc[VMIN]  = 0;
    options.c_cc[VTIME] = 0;

    tcflush(fd, TCIFLUSH);  //flush serial port input buffer
    tcflush(fd, TCOFLUSH);  //flush serial port output buffer
    tcsetattr(fd,TCSANOW,&options);

/*  tcsetattr(fd,TCSAFLUSH,&options);
    tcflush(fd, TCIFLUSH);  //flush serial port input buffer
    tcflush(fd, TCOFLUSH);  //flush serial port output buffer*/


	//TODO Verificar que esto funciona y explicar porque
	//sin esto, en la maquina de jorge conecta 1 vez si 1 vez no.
	//también fue necesario para que funcionara en las maquinas del taller, en la xo no es necesario
	close(fd);
	fd = open(serialport, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd == -1)  {
            //perror("init_serialport: Unable to open port ");
            return -1;
    }
    /* este codigo intenta hacer la espera necesaria luego de inicializar el puerto serial para que este quede funcional */    
    respuesta = (unsigned char*)malloc(6);
    unsigned char b;
    retries = MAX_RETRIES;
    do {
      //  tcsetattr(fd,TCSANOW,&options);
        send_msg(fd, echo, 4);
        leidos=read_msg(fd, respuesta, CAGATA_AVOID);
        //printf("serialcomm:TIMEOUT %i, trying to recover...\n", leidos);
        retries--;
    } while((leidos == -2) && (retries>0));
    if(leidos== -2){
      return -1; //abort because problems with the serial node
    }
    /*consumo en buffer de entrada, hubiera sido interesante usar fsync o tcdrain (posix)*/
    retries = MAX_BUFFER_RETRIES;
    do {
        b=0;
        leidos=receive_data(fd, &b, 1, 200);
        //printf ("serialcomm: purging buffer %02X, %i...\n", b, leidos);
        //leidos=read_msg(fd, respuesta, CAGATA_AVOID);
        retries--;
    }  while((leidos != -2) && (retries>0));
    #if VERBOSE    
    printf("se leyeron %d bytes y se llega a MAX_BUFFER_RETRIES - %d intentos\n", leidos, retries);
    int i;
    for(i=0; i<PACKET_LEN; i++){
        printf("respuesta pos[%d]=%02X \n", i, *(respuesta + i));
    }
    #endif
    tcflush(fd, TCIFLUSH);  //flush serial port input buffer
    tcflush(fd, TCOFLUSH);  //flush serial port output buffer
    /* a partir de este momento el serial esta operativo */
    return fd;
}

int serialport_close(int handler){
    int rc;
    tcsetattr(handler, TCSANOW, &oldoptions);
    rc = close(handler);
    return rc;
}

int unescape (unsigned char* buffer, int largo) {         // saca los ESCAPES y devuelve el mensaje original
    int i=0, pos=0;                                       // inicializa posición del array de entrada y salida (que son el mismo)
    do {
        if (buffer[i] == ESCAPE) {i++;}                   // si encuentra un ESCAPE, lo saltea
        buffer[pos++] = buffer[i++];                      // copia el byte correcto a la nueva posición
    } while (i < largo);
    return pos;                                           // devuelve el nuevo largo
}

void timevalfix (struct timeval *tv){
  while (tv->tv_usec < 0){
      tv->tv_sec--;
      tv->tv_usec += 1000000;
    }
  while (tv->tv_usec >= 1000000){
      tv->tv_sec++;
      tv->tv_usec -= 1000000;
    }
}

void ms2timeval(struct timeval* tv, int timeout){
    tv->tv_sec = 0;
    tv->tv_usec = timeout*1000;
    timevalfix (tv);
}

/*  Add a timeval.
    
    Add in \b to the time in \b val.

    @param to  result and value to add
    @param val value to add
    @see timevalsub  */
void timevaladd(struct timeval *to, struct timeval *val){
    to->tv_sec += val->tv_sec;
    to->tv_usec += val->tv_usec;
    timevalfix (to);
}

/*! Subtract a timeval.

    Subtract from \b to the time in \b val.  The result time can
    become negative.

    @param to  result and value from which to subtract
    @param val value to subtract
    @see timevaladd  */
void timevalsub(struct timeval *to, struct timeval *val){
    to->tv_sec -= val->tv_sec;
    to->tv_usec -= val->tv_usec;
    timevalfix (to);
}

int timeval2ms(struct timeval *tv){
    return (tv->tv_sec * 1000 + tv->tv_usec / 1000);
}
    
int read_msg (int handler, unsigned char *buffer, int timeout) {                  // esta es la "máquina de estados" que recibe y decodifica los mensajes seriales
	int largo, cont, leidos, newtimeout_ms;
	unsigned char checksum;
	//int anterior = ESCAPE;
	int anterior = 1;
	//static char *buffer = NULL;
	enum { BUSCAR = 0, LEER_LARGO, LEER_MSG, CHECKSUM } estado = BUSCAR;
	unsigned char b;
	struct timeval  now, end, newtimeout, timeout_timeval;
    ms2timeval(&timeout_timeval, timeout);
	gettimeofday(&now, NULL);
    timevaladd(&now, &timeout_timeval);
    end = now;    
    #if VERBOSE_TIMEOUT        
    printf("-----------------------nuevo mensaje a leer %d\n", timeout);
    #endif
    while (1) {
        gettimeofday(&now, NULL);    
        newtimeout = end;        
        timevalsub(&newtimeout, &now);
        newtimeout_ms = timeval2ms(&newtimeout);
        if (newtimeout_ms<0) {
            newtimeout_ms=0;
        }
        #if VERBOSE_TIMEOUT        
        printf("--------------------------voy a esperar %d milisegundos... y me pidieron para el mensaje %d\n", newtimeout_ms, timeout);
        #endif
        leidos=receive_data(handler, &b, 1, newtimeout_ms);
        if(leidos!=0){
            return leidos;
        }else
            {
            #if DEBUG
		        printf("el valor de b es %02X \n", b);
            #endif
		    if (b==SYNC && ((anterior!=ESCAPE && estado==BUSCAR) || anterior<128)) {        // chequea la presencia de un "TRUE SYNC"
			    estado = LEER_LARGO;
		    } else {
			    switch (estado) {
			    case BUSCAR:
				    #if DEBUG
                        printf("estoy en estado BUSCAR\n");
                    #endif
				    break;
			    case LEER_LARGO:
                    #if DEBUG                    
				        printf("estoy en estado LEER_LARGO\n");
                    #endif
				    largo = (int)b;
				    //buffer = (byte*)malloc(largo);	// inicializa el buffer segun el largo del mensaje
				    checksum = 0;                         // se prepara para leer el cuerpo del mensaje
				    cont = 0;
				    estado=LEER_MSG;
				    break;
			    case LEER_MSG:
                    #if DEBUG
                        printf("estoy en estado LEER_MSG\n");
                    #endif  
				      buffer[cont++] = b;                   // va llenando el buffer
				      checksum += b;                        // va efectuando la suma comprobatoria
				      if (cont == largo) {                  // ...hasta que termina el cuerpo del mensaje
				        checksum &= 127;                    // "recorta" el checksum
				        estado = CHECKSUM;
				      }
				      break;
			    case CHECKSUM:
                    #if DEBUG
					    printf("voy a checkear el checksum\n");
                    #endif
				    if (b == checksum) {                                              // comprueba el checksum
					    largo = unescape (buffer, largo);                               // esta rutina saca los ESCAPEs. El resultado queda en las mismas variables buffer y largo
                        #if DEBUG
						    printf("el checksum esta ok\n");
                        #endif
					    return largo;
					    //error = dispatch (buffer, largo);                               // la "dispatch" se encarga de entregar el mensaje al handler adecuado, y devuelve su propio error o el del handler;
				    }
				    estado = BUSCAR;                                                  // estado inicial
				    break;
			    }
	        }
	        anterior = b;                                                           // se prepara para procesar el siguiente byte
		}
	}
}

/* sends a message using the butia protocol and doing a workaround to avoid the fixed time waiting after initializing the serial port for use*/ 
int send_msg(int handler, unsigned char* data, int largo){
    unsigned char resultado [largo*2+1]; //todos los bytes se pueden llegar a escapear y ademas necesito el byte de SYNC
    unsigned char pos = 0;
    int res, i, j = 0;
    unsigned char checksum = 0;
    resultado[pos++] = SYNC;
    resultado[pos++] = 0x00; // aca va a ir el largo del mensaje cuando sepa luego de procesar el largo.
    for (i=0; i<largo; i++) {
        if (data[i] < ESCAPE) {
            resultado[pos++] = data[i];
        }
        else{
            resultado[pos++] = ESCAPE;
            resultado[pos++] = data[i];
        }
    }
    for (j=1; j<pos; j++) {
        checksum += resultado[j];
    }
    resultado[pos] = checksum & 127;
    resultado[1] = pos-2; //el 1 es la posicion donde se guarda el largo, el largo no incluye el SYNC, el propio largo ni el CRC
    res = serialport_write_len(handler, resultado, pos+1);
    //printf("el resultado de mandar el msg es %d \n", res);
    return (res);
}




/************************************************************************
*
* Basic serial access and butia protocol encapsulation
* Copyright 2010 MINA Group, Facultad de Ingenieria, Universidad de la
* Republica, Uruguay.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
************************************************************************/
