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

#define null   0x00
#define TIMEOUT -2
#define DEBUG 0
#define VERBOUSE 0
#define VERBOUSE_TIMEOUT 0

int timeout_counter = 0;
struct termios oldoptions;


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
	printf("problemas en el send\n");
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
    timeout_struct.tv_sec  = 0;                /* seconds */
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
            printf("problemas en serialcomm:read\n");
            usleep(250000);
		}
		else{
			perror("read()");
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
    unsigned char* respuesta;
    int leidos = 0;
    int fd;
    fd = open(serialport, O_RDWR | O_NOCTTY | O_NONBLOCK);
	//printf("fd after open is %i %i", fd, errno);
    if (fd == -1)  {
            perror("init_serialport: Unable to open port ");
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

	//sin esto, en la maquina de jorge conecta 1 vez si 1 vez no.
	//también fue necesario para que funcionara en las maquinas del taller, en la xo no es necesario
	close(fd);
	fd = open(serialport, O_RDWR | O_NOCTTY | O_NONBLOCK);

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



/************************************************************************
*
* Basic serial access
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
