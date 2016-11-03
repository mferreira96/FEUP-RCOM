#include "DataLink.h"

typedef struct{
	int fileDescriptor;

	int status;

	char* fname;

}applicationLayer;

applicationLayer * application;

int initAppLayer(char serialPort[], int type, char* fileName);
