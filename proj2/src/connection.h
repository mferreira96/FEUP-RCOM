#ifndef CONNECTION_H
#define CONNECTION_H



typedef struct{
	int fileDescriptor;
	int port;
	char * ip;
}connection;

int get_ip();

int connect();

int login_host();

int passive();

int get_path();

int download();


#endif
