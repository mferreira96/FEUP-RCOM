#include "DataLink.h"

#define BLOCK 50

typedef struct{
	int fileDescriptor;

	int status;

}applicationLayer;

applicationLayer * application;
