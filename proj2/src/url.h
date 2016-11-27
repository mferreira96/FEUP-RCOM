#ifndef URL_H
#define URL_H

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
	char * user;
	char * password;
	char * host;
	char * url_patch;
}url;

int init(char * url_name, url * url);


#endif
