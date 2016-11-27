#ifndef CONNECTION_H
#define CONNECTION_H



#include "url.h"
#include <unistd.h>


typedef struct{
	int fileDescriptor;
	int port;
	char * ip;
}connection;


int get_ip(connection * connection, char * host	);

int connectTo(connection * connection);

int login_host(connection * connection, url * url);

int passiveMode(connection * connection1,connection * connection2);

int get_path();

int download();


#endif
