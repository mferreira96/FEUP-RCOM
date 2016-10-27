#include "DataLink.h"

typedef struct{
	int fileDescriptor;

	int status;

}applicationLayer;

applicationLayer * application;

int initAppLayer(char serialPort[], int type);
