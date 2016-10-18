#ifndef LINKDATA_H
#define LINKDATA_H


int llopen(int port, int flagMode);

int llwrite(int fd, char *buffer, int length);

int llread(int fd, char * buffer);

int llclose(int fd);

#endif
