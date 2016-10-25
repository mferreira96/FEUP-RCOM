#include <stdio.h>
#include <termios.h>
#include <string.h>

#include "Cli.h"



void choosePort(char * name){

  int option;

  printf("choose  port name: \n");
  printf(" 0 - /dev/ttyS0 \n");
  printf(" 1 - /dev/ttyS1 \n");
  scanf("%d\n",&option);

  while(option != 0 && option != 1 ){
    printf("choose a valid option\n");
    scanf("%d\n",&option);
  }

  switch (option) {
    case 0:
      strcpy(name,"/dev/ttyS0");
      return;
    break;
    case 1:
      strcpy(name,"/dev/ttyS1");
      return;
    break;
  }

}

int chooseNRetries(){
  int option;

  printf("write the max number of attempts: \n");
  scanf("%d\n",&option);

  if(option < 1){
    printf("Invalid option. By default the timeout will be: %d \n",NRETRIES_DEFAULT);
    return   NRETRIES_DEFAULT;
  }
  else
    return option;

}

int chooseBaudRate(){
  int option;

while(1){
  printf("choose baudRate: \n");
  printf(" 300  600 1200 2400  4800 9600 19200 38400 57600 115200\n");
  printf("0 is default \n");

  scanf("%d\n",&option);

  switch (option) {
    case 0:
      return BAUDRATE_DEFAULT;
    case 300:
      return BAUDRATE_300;
    case 600:
      return BAUDRATE_600;
    case 1200:
      return BAUDRATE_1200;
    case 2400:
      return BAUDRATE_2400;
    case 4800:
      return BAUDRATE_4800;
    case 9600:
      return BAUDRATE_9600;
    case 19200:
      return BAUDRATE_19200;
    case 38400:
      return BAUDRATE_38400;
    case 57600:
      return BAUDRATE_57600;
    case 115200:
      return BAUDRATE_115200;
    default:
      printf("Invalid option please try agaiIIn\n");
    }
  }

  return 0;
}

int chooseTimeout(){
  int option;

  printf("write time out: \n");
  scanf("%d\n",&option);

  if(option < 1){
    printf("Invalid option. By default the timeout will be: %d \n",TIMEOUT_DEFAULT);
    return   TIMEOUT_DEFAULT;
  }
  else
    return option;

}



int chooseMaxSize(){
  return 0;
}