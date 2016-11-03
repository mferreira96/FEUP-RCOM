#include "DataLink.h"

typedef struct{
	int fileDescriptor;

	int status;

	char* fname;

}applicationLayer;

applicationLayer * application;

int initAppLayer(char serialPort[], int type, char* fileName);

int createfile( char* fileName);

int sendFile(int fd);

int receiveControlPacket();

int createfile( char* fileName);

void writeOnFile(unsigned char * data);

int sendControlPacket(int i);

int readFile();
