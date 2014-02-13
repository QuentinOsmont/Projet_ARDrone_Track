#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>


int serial_open(const char *port, int baudrate)
{
  struct termios  options;
  int bitrate;

  bitrate = 0;
  if (baudrate == 9600) bitrate = B9600;
  if (baudrate == 19200) bitrate = B19200;
  if (baudrate == 57600) bitrate = B57600;

  if (!bitrate)
    {
      return 0;
    }

  int tty_fd = open(port, O_RDWR | O_NOCTTY);// | O_NONBLOCK);

  if (tty_fd < 0)
    return 0;


  tcgetattr(tty_fd, &options);

  //cfmakeraw(&options);


  options.c_cflag = bitrate | CS8 | CREAD | CLOCAL;
  options.c_iflag = IGNPAR;
  options.c_oflag = 0;
  options.c_lflag = 0;

  options.c_cc[VMIN]  = 1;
  options.c_cc[VTIME] = 0;

  /*
  options.c_iflag &= ~(IGNBRK | BRKINT | ICRNL |
                      INLCR | PARMRK | INPCK | ISTRIP | IXON);
  //
  // Output flags - Turn off output processing
  // no CR to NL translation, no NL to CR-NL translation,
  // no NL to CR translation, no column 0 CR suppression,
  // no Ctrl-D suppression, no fill characters, no case mapping,
  // no local output processing
  //
  options.c_oflag &= ~(OCRNL | ONLCR | ONLRET |
                       ONOCR | OFILL | OLCUC | OPOST);
  //
  // No line processing:
  // echo off, echo newline off, canonical mode off,
  // extended input processing off, signal chars off
  //
  options.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
  //
  // Turn off character processing
  // clear current char size mask, no parity checking,
  // no output processing, force 8 bit input
  //
  options.c_cflag &= ~(CSIZE | PARENB);
  options.c_cflag |= bitrate | CS8;
  //
  // One input byte is enough to return from read()
  // Inter-character timer off
  //
  options.c_cc[VMIN]  = 1;
  options.c_cc[VTIME] = 0;
  */

  tcsetattr(tty_fd, TCSANOW, &options);

  return tty_fd;
}

void serial_close(int hSerial)
{
  close(hSerial);
}

int serial_write(int hSerial, char *buffer, int nNumberOfBytesToWrite, int *lpNumberOfBytesWritten)
{
  *lpNumberOfBytesWritten = write(hSerial, buffer, nNumberOfBytesToWrite);
  return (*lpNumberOfBytesWritten >= 0);
}

int serial_read(int hSerial, char *buffer, int nNumberOfBytesToRead, int *lpNumberOfBytesRead)
{
  errno = 0;
  *lpNumberOfBytesRead = read(hSerial, buffer, nNumberOfBytesToRead);
  if (errno == EAGAIN)
    *lpNumberOfBytesRead = 0;
  //  printf("=========%d\n", *lpNumberOfBytesRead);
  return (*lpNumberOfBytesRead >= 0);
}
