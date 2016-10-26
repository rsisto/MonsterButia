#ifndef _SERIALCOMM_H_
#define _SERIALCOMM_H_

#define BAUDRATE B9600
#define DEVICE "/dev/ttyUSB0"

int send_msg(int handler, unsigned char* data, int largo);
int read_msg (int handler, unsigned char *buffer, int timeout);
int send_data(int handler, unsigned char *data, char *device);
int receive_data(int handler, unsigned char *data, int len, int timeout);
int serialport_init(const char* serialport, int baud);
int serialport_write(int fd, unsigned char* str);
int serialport_read(int fd, unsigned char* str, int len, int timeout);
int serialport_close(int handler);
#endif // _SERIALCOMM_H_
