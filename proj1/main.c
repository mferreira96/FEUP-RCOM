#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "applicationLayer.h"

int  main(int argc, char** argv){

  if(argc != 3){
    printf("Usage:./main flagMode");
    exit(1);
  }
  configLinkLayer(argv[1]);

  int x;
  if(strcmp(argv[1], "0")==0){
    printf("TRANSMITTER\n");
   x =initAppLayer(argv[2],TRANSMITTER);//= llopen(argv[2],TRANSMITTER);
  }else{
    printf("RECIEVER\n");
   x = initAppLayer(argv[2],RECEIVER);//llopen(argv[2],RECEIVER);
 }
  if(x != 0){
    perror("Error on llopen");
    exit(3);
  }
  else{
    printf("Connection ended successfully\n");
  }

  return 0;
