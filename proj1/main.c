#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "applicationLayer.h"
#include "Cli.h"

int  main(int argc, char** argv){

  if(argc != 2){
    printf("Usage:./main flagMode (0 - send, 1 - receive) \n");
    exit(1);
  }
 	
  char port[20];
  choosePort(port);
  
  if(strcmp(argv[1], "0")==0)
  {
	  if(chooseFileToSend()==0) //file doesnt exist
		  return 0;
  }
  
  int nRetries = chooseNRetries();
  int time = chooseTimeout();
  int baudRate = chooseBaudRate();

  printf("port = %s \n", port);
  printf("nretries = %d \n", nRetries);	
  printf("time  = %d \n", time);
  printf("baud = %d \n", baudRate);

  configLinkLayer(port, nRetries, time, baudRate);

  int x;
  if(strcmp(argv[1], "0")==0){
    printf("TRANSMITTER\n");
   x =initAppLayer(port,TRANSMITTER);
  }else{
    printf("RECIEVER\n");
   x = initAppLayer(port,RECEIVER);
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
