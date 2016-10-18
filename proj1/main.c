#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "DataLink.h"

int  main(int argc, char** argv){

  if(argc != 2){
    printf("Usage:./main flagMode");
    exit(1);
  }

  configLinkLayer();

  int x;
  if(argv[1] == 0)
   x = llopen(linkLayer->port, TRANSMITER);
  else
   x = llopen(linkLayer->port, RECEIVER);

  if(x != 0)
    perror("Error on llopen");

  return 0;
}
