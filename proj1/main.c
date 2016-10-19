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
  configLinkLayer(argv[1]);

  int x;
  if(argv[1] == 0){
    printf("TRANSMITER\n");
   x = llopen(TRANSMITER);
  }else{
    printf("RECIEVER\n");
   x = llopen(RECEIVER);
 }
  if(x != 0){
    perror("Error on llopen");
    exit(3);
  }
  else{
    printf("Connection ended successfully\n");
  }

  return 0;
}
