#ifndef CONNECTION_H
#define CONNECTION_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>


typedef struct{
	int fileDescriptor;
	int port;
	char * ip;
}connection;


int get_ip(connection * connection, char * host	);

int connect(connection * connection);

int login_host(connection * connection, url * url);

int passive(connection * connection,connection * connection2);

int get_path();

int download();


#endif
