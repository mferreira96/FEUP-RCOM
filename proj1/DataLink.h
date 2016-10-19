#ifndef LINKDATA_H
#define LINKDATA_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <signal.h>

#define MAX_FRAME_SIZE 1024
#define BAUDRATE   B38400
#define TRANSMITER  0
#define RECEIVER    1
#define FLAG       0x7e
#define A          0x03
#define C_SET      0x03
#define C_UA       0x07
#define FALSE 0
#define TRUE 1


typedef struct {
  char port[20];
  int baudRate;
  unsigned int sequenceNumber;
  unsigned int timeout;
  unsigned int numTransmissions;
  char frame[MAX_FRAME_SIZE];
  struct termios oldtio,newtio;
}LinkLayer;

extern LinkLayer* linkLayer;

int setNewTermios(int fd);

void configLinkLayer(int flagMode);

int llopen(int flagMode);

int llwrite(int fd, char *buffer, int length);

int llread(int fd, char * buffer);

int llclose(int fd);

void setAndSendSET(int fd);

void setAndSendUA(int fd);

int connectTransmiter(int fd);

int connectReciever(int fd);

int stateMachine(unsigned char c, int state, char tmp[]);

void sigalrm_handler();

#endif
