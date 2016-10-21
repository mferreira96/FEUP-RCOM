#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "DataLink.h"

int  main(int argc, char** argv){
////TESTE////TESTE////TESTE////TESTE////TESTE
  char n[10];
printf("\n aquii 0 \n");
llwrite(1, &n, 1);

//  printf("\n\n\n end end end \n\n\n");
  ////TESTE ////TESTE ////TESTE ////TESTE
  
  if(argc != 3){
    printf("Usage:./main flagMode");
    exit(1);
  }
  configLinkLayer(argv[1]);

  int x;
  if(strcmp(argv[1], "0")==0){
    printf("TRANSMITER\n");
   x = llopen(argv[2],TRANSMITER);
  }else{
    printf("RECIEVER\n");
   x = llopen(argv[2],RECEIVER);
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
