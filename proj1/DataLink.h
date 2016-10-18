#ifndef LINKDATA_H
#define LINKDATA_H

#define BAUDRATE   B38400
#define TRANSMITER  0
#define RECEIVER    1

int llopen(char * port, int flagMode);

int llwrite(int fd, char *buffer, int length);

int llread(int fd, char * buffer);

int llclose(int fd);

#endif
