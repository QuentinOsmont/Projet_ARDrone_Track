#ifndef _SERIAL_H_
# define _SERIAL_H_

int serial_open(const char *port, int baudrate);
void serial_close(int hSerial);
int serial_write(int hSerial, char *buffer,
                 int nNumberOfBytesToWrite, int *lpNumberOfBytesWritten);
int serial_read(int hSerial, char *buffer,
                int nNumberOfBytesToRead, int *lpNumberOfBytesRead);

#endif
