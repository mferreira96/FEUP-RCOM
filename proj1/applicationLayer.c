#include "applicationLayer.h"

int receiveControlPacket(){

	printf("pedir control packet \n");
	char * data;
    data= (char *) malloc(100);
    llread(application->fileDescriptor,data);
	char fileSize[4];
	memcpy(fileSize,&data[3],4);
	int size;
	sscanf(fileSize,"%X",&size);
	char *fileName=malloc(sizeof(char)*20);
	memcpy(fileName,&data[9],(int)data[8]);
	printf("file size is: %d",size);
	printf("file name is: %s",fileName);
    free(data);
	free(fileName);
	return 0;
}

int sendControlPacket(int i){
	char size[4];
	sprintf(size,"%x",500);
	printf("TAMANHO : %X \n",500);
	char data[14];
	data[0]=i;
	data[1]=0;
	data[2]=4;
	memmove(data+3,size,4);
	data[7]=1;
	data[8]=strlen("ONOME");
	memmove(data+9,"ONOME",5);

	llwrite(application->fileDescriptor,data,14);
return 0;
}

int readFile(){
	int STOP=FALSE;
	char * data;
    data= (char *) malloc(100);
    bzero(data,100);
    while(STOP==FALSE){
    	llread(application->fileDescriptor,data);
    	if(data[0]==3)
    		STOP=TRUE;
    	else{
    		printf("TESTE:  \n");
    		int i=0;
    		int size=0;
    		printf("tamanho: %d \n",(int)(data[3]));
    		for(i;i<(int)(data[3])+4;i++){
    		printf("li isto: %c \n",data[i]);
    		}
    		
    	}
    	bzero(data,100);
    }
    
    free(data);
    return 0;
}

int initAppLayer(char serialPort[], int type){

  application = (applicationLayer *) malloc(sizeof(applicationLayer));
  application->fileDescriptor = open(serialPort, O_RDWR | O_NOCTTY );      // open SerialPort
  if (application->fileDescriptor <0) {perror(serialPort); exit(-1); }

  if (setNewTermios(application->fileDescriptor) != 0){            // set new termios
    perror("Error setting new termios");
    exit(2);
  }
  application->status = type;
  
  if(application->status == TRANSMITTER){

    printf("vou enviar \n");
    printf("%d \n",application->fileDescriptor);
    if(llopen(application->fileDescriptor,application->status)!=0){
    	printf("Error establishing connection! \n");
    	return 1;
    }
	printf("connection established \n");
	if(sendControlPacket(2)!=0){
		printf("Can't send start packet! \n");
	}
	printf("sent start packet! \n");
	llclose(application->fileDescriptor,application->status);
    
  }
  else if(application->status == RECEIVER){
    printf("vou receber \n");
    printf("%d \n",application->fileDescriptor);
    if(llopen(application->fileDescriptor,application->status)!=0){
    	printf("Error establishing connection! \n");
    	return 1;
    }
	printf("connection established \n");
	if(receiveControlPacket()!=0){
		printf("Can't receive start packet! \n");
	}
	readFile();
	printf("received start packet! \n");
	llclose(application->fileDescriptor,application->status);
  }
  
  return 0;
}
