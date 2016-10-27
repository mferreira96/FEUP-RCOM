#include <fcntl.h>
#include "applicationLayer.h"
int fd; 

int createfile(char* fileName);
int sendFile(int fd);

int receiveControlPacket(){

	char * data;
    data= (char *) malloc(100);
    llread(application->fileDescriptor,data);
	char fileSize[4];
	memcpy(fileSize,&data[3],4);
	int size;
	sscanf(fileSize,"%X",&size);
	char *fileName=malloc(sizeof(char)*20);
	memcpy(fileName,&data[9],(int)data[8]);
	printf("file size is: %d \n",size);
	printf("file name is: %s \n",fileName);
		
	fd = createfile(fileName);
   	free(data);
	free(fileName);
	return 0;
}

int createfile(char* fileName)
{

	int fdNew; 
	
	fdNew = open(fileName,O_CREAT|O_WRONLY | O_APPEND |O_TRUNC);
	if (fdNew == -1)
	{
		perror("Error creating file!");
		exit(1);
	}
	return fdNew;
	
}

void writeOnFile(char * data){
	int L2 = (int)data[2];
	int L1 = (int)data[3];
	int size = 256 * L2 + L1;
	write(fd, data + 4,size);	
	//printf("eu escrevi %d \n", sizeof(&data));
}

int sendControlPacket(int i){
	char size[4];
	sprintf(size,"%x",500);
	printf("TAMANHO : %X \n",500);
	char *data = (char*)malloc(BLOCK);
	data[0]=i;
	data[1]=0;
	data[2]=4;
	memmove(data+3,size,4);
	data[7]=1;
	data[8]=strlen("pinguim.gif");
	memmove(data+9,"pinguim.gif",(int)data[8]);

	llwrite(application->fileDescriptor,data,9+(int)data[8]);
	free(data);
return 0;
}

int readFile(){
	int STOP=FALSE;
	char * data;
    data= (char *) malloc(100);
    bzero(data,100);
    while(STOP==FALSE){
    	int value=llread(application->fileDescriptor,data);
		
    	if(data[0]==3 && value!=-1)
    		STOP=TRUE;
    	else if(value!=-1){
			printf("data %d \n",data[1]);
    		writeOnFile(data); 		
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

  application->status = type;
  if (setNewTermios(application->fileDescriptor, application->status) != 0){            // set new termios
    perror("Error setting new termios");
    exit(2);
  }

  
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
	sendFile(application->fileDescriptor);
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
	printf("received start packet! \n");
	readFile();
	llclose(application->fileDescriptor,application->status);
  }
  
  return 0;
}

int sendFile(int fd){
	FILE *f = fopen("pinguim.gif","r");

	if(f == NULL){
		perror("Error openning file to send \n");
		exit(1);
	}

    int size = 0;
	int blockSize = BLOCK +4;
    char buffer[blockSize];
	int counter = 0;
	char last[1];
	buffer[0] = 1;

	printf("vou começar a enviar \n");
    while(1){
		size = fread(buffer + 4,sizeof(char),BLOCK,f); 		
		if(size <= 0){
			break;			
		}		
		printf("size = %d \n",size);
		buffer[1] = counter;
		buffer[2] = size / 256;
		buffer[3] = size % 256;		
		llwrite(fd, buffer,size + 4);	
		
		counter++;
	}
	printf("sai do ciclo sendFile \n");
	last[0] = 3;

	llwrite(fd, last, strlen(last));
	printf("Enviei mensagem de terminação da trama de dados \n");
	
	 if(fclose(f)!= 0){
		perror("error closing file to be sent\n");	
		exit(2);
	}

	return 0;
}


