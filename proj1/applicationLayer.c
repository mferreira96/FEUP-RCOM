#include "applicationLayer.h"
int fd; 

int createfile( char* fileName);
int sendFile(int fd);

//receives and process info from start packet
int receiveControlPacket(){

	unsigned char * data;
    data= (unsigned char *) malloc(50);
    
    llread(application->fileDescriptor,data);
	char fileSize[4];
	memcpy(fileSize,&data[3],4);
	int size;
	sscanf(fileSize,"%X",&size);
	 char *fileName=malloc(sizeof( char)*20);
	bzero(fileName,20);
	memcpy(fileName,&data[9],(int)data[8]);
	printf("file size is: %d \n",(int)data[8]);
	printf("file name is: %s \n",fileName);
		
	fd = createfile(fileName);
   	free(data);
	free(fileName);
	return 0;
}

//creates the file with name given from start packet
int createfile( char* fileName)
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

//writes data on the file
void writeOnFile(unsigned char * data){
	int L2 = (int)data[2];
	int L1 = (int)data[3];
	int size = 256 * L2 + L1;
	write(fd, data + 4,size);
}

//sends start control packet
int sendControlPacket(int i){

	struct stat buf;
	int fd=open("pinguim.gif", O_RDONLY);
	if(fd < 0){
		printf("Error opening file to be sent \n");	
	}

	fstat(fd, &buf);
	int sizeOfFile = buf.st_size;
	close(fd);
	printf("TAMANHO decimal : %d \n",sizeOfFile);
	char size[4];
	sprintf(size,"%X",sizeOfFile);
	printf("TAMANHO : %s \n",size);

	unsigned char *data = (unsigned char*)malloc(BLOCK);
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

//reads information frames and calls writeOnFile
int readFile(){
	int STOP=FALSE;
	unsigned char * data;
    data= (unsigned char *) malloc(2*BLOCK);
    bzero(data,2*BLOCK);
    while(STOP==FALSE){
    	int value=llread(application->fileDescriptor,data);
		
    	if(data[0]==3 && value!=-1)
    		STOP=TRUE;
    	else if(value!=-1){
			printf("data %d \n",data[1]);
			
    		writeOnFile(data); 		
    	}
    	bzero(data,2*BLOCK);
    }
    

    free(data);
    return 0;
}

//the application layer is initialized, sends and recevives data packets
int initAppLayer(char serialPort[], int type){

  application = (applicationLayer *) malloc(sizeof(applicationLayer));
  application->fileDescriptor = openSerialPort(serialPort);

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
	//sleep(5);
	printf("received start packet! \n");
	readFile();
	llclose(application->fileDescriptor,application->status);
  }
  
  return 0;
}

//reads from file and calls llwrite
int sendFile(int fd){
	FILE *f = fopen("pinguim.gif","r");

	if(f == NULL){
		perror("Error openning file to send \n");
		exit(1);
	}

    int size = 0;
	int blockSize = BLOCK +4;
    unsigned char buffer[blockSize];
	int counter = 0;
	unsigned char last[1];
	buffer[0] = 1;

	printf("vou começar a enviar \n");
    while(1){
		size = fread(buffer + 4,sizeof(char),BLOCK,f); 		
		if(size <= 0){
			break;			
		}		
		printf("size = %d \n",size);
		buffer[1] = counter;
		buffer[2] = (size) / 256;
		buffer[3] = (size) % 256;		
		llwrite(fd, buffer,size + 4);	
		
		counter++;
	}
	printf("sai do ciclo sendFile \n");
	last[0] = 3;

	llwrite(fd, last, 1);
	printf("Enviei mensagem de terminação da trama de dados \n");
	
	 if(fclose(f)!= 0){
		perror("error closing file to be sent\n");	
		exit(2);
	}

	return 0;
}


