#ifndef _SERIALCOMM_H_
#define _SERIALCOMM_H_

#define BAUDRATE B9600
#define DEVICE "/dev/ttyUSB0"

int serialport_init(const char* serialport, int baud);
int serialport_write(int fd, unsigned char* str);
int serialport_write_len(int fd, unsigned char* str, int len);
int serialport_read(int fd, unsigned char* str, int len, int timeout);
int serialport_close(int handler);
#endif // _SERIALCOMM_H_
