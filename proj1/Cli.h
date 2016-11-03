#ifndef CLI_H
#define CLI_H


#define BAUDRATE_DEFAULT B38400
#define BAUDRATE_300 B300
#define BAUDRATE_600 B600
#define BAUDRATE_1200 B1200
#define BAUDRATE_2400 B2400
#define BAUDRATE_4800 B4800
#define BAUDRATE_9600 B9600
#define BAUDRATE_19200 B19200
#define BAUDRATE_38400 B38400
#define BAUDRATE_57600 B57600
#define BAUDRATE_115200 B115200


#define TIMEOUT_DEFAULT  3
#define NRETRIES_DEFAULT 3


void choosePort(char * name);

int chooseNRetries();

int chooseTimeout();

int chooseBaudRate();

int chooseMaxSize();

char* chooseFileToSend();


#endif
