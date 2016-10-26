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
#define TRANSMITTER  0
#define RECEIVER    1
#define FLAG       0x7e
#define A          0x03
#define C_SET      0x03
#define C_UA       0x07
#define C_RR       0x05
#define C_REJ      0x01
#define C_DISC     0x0b
#define ESC        0x7d
#define FALSE 0
#define TRUE 1

#define CONTROL_START 	2
#define CONTROL_END		3
#define CONTROL_DATA	1

#define PARAM_FILESIZE 	0
#define PARAM_FILENAME	1

typedef struct {
  char port[20];
  int baudRate;
  unsigned int sequenceNumber;
  unsigned int timeOut;
  unsigned int numTransmissions;
  char frame[MAX_FRAME_SIZE];
  struct termios oldtio,newtio;
}LinkLayer;

typedef enum{
	SET = 0,
	UA = 1,
	INF = 2,
	DISC = 3,
  	RR = 4,
  	REJ = 5
}TypeOfFrame;

extern LinkLayer* linkLayer;

int setNewTermios(int fd);

int configLinkLayer(int flagMode);

int llopen(int fd,int flagMode);

int llwrite(int fd, char *buffer, int length);

int llread(int fd, char * buffer);

int llclose(int fd, int type);

void setAndSendSET(int fd);

void setAndSendUA(int fd);

int connectTransmiter(int fd);

int connectReciever(int fd);

int stateMachine(unsigned char c, int state, char tmp[], TypeOfFrame type, int pos);

void sigalrm_handler();

char blockCheckCharacter(char buffer[], int size);

void sendControlPackage(int control, int fd, char* name, char* filesize);

void sendMessage(int fd, unsigned char* buf, int buf_size);

int createMessage(const unsigned char* buf,unsigned char* message, int buf_size);

int calculateDataSize(int size);

int byteStuffing(char packet[], int size);

int deByteStuffing(char packet[], int size);



#endif
