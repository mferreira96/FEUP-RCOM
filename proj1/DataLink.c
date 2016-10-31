#include "DataLink.h"


volatile int STOP=FALSE;
volatile int SEND=TRUE;

unsigned int counterNumOfAttempts;

int contador=0;
LinkLayer * linkLayer;
TypeOfFrame typeOfFrame;
Stats* stats;
unsigned int timeout;
unsigned int alarm_n;

void printStats();
int connectTransmitter(int fd);


int configLinkLayer(char pt[], int numRetries, int time, int baudrate){
  linkLayer = (LinkLayer*) malloc(sizeof(LinkLayer));
  stats= (Stats*) malloc(sizeof(Stats));

  strcpy(linkLayer->port,pt);
  linkLayer->numTransmissions = numRetries;
  linkLayer->baudRate = baudrate;
  linkLayer->timeOut = time;
  linkLayer->sequenceNumber=0;

  stats->transmitedTramas=0;
  stats->receivedTramas=0;
  stats->timeoutOcurrences=0;
  stats->numRej=0;
  stats->numRR=0;

  return 0;

}

int openSerialPort(char serialPort[]){

	int fd = open(serialPort, O_RDWR | O_NOCTTY );      // open SerialPort
  	if (fd < 0){
		perror(serialPort); 
		exit(-1); 
	}
	
	return fd;

}


int setNewTermios(int fd, int flagMode){

  if ( tcgetattr(fd,&linkLayer->oldtio) == -1) { /* save current port settings */
    perror("tcgetattr");
    exit(-1);
  }

  bzero(&linkLayer->newtio, sizeof(linkLayer->newtio));
  linkLayer->newtio.c_cflag = linkLayer->baudRate | CS8 | CLOCAL | CREAD;
  linkLayer->newtio.c_iflag = IGNPAR;
  linkLayer->newtio.c_oflag = 0;

  /* set input mode (non-canonical, no echo,...) */
  linkLayer->newtio.c_lflag = 0;


	if(flagMode == RECEIVER){
	  linkLayer->newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
	  linkLayer->newtio.c_cc[VMIN]     = 1;   /* blocking read until 1 chars received */
	}
	else{
	  linkLayer->newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
	  linkLayer->newtio.c_cc[VMIN]     = 0;   /* blocking read until 1 chars received */
	}


  tcflush(fd, TCIOFLUSH);

  if ( tcsetattr(fd,TCSANOW,&linkLayer->newtio) == -1) {
    perror("tcsetattr");
    exit(-1);
  }

  return 0;
}


int llopen(int fd, int flagMode){
	alarm_n=1;
  signal(SIGALRM, sigalrm_handler);       //sigalrm_handler is the function  called when the alarm is fired
  printf("cheguei aqui \n");
  int error;
  if(flagMode == TRANSMITTER){
    error = connectTransmitter(fd); // Transmitter try to stablish communication
  }
  else{
    error = connectReciever(fd);    //Reciever try to stablish communication
  }

  if(error == -1){
    perror("Error trying to stablish communication");
    exit(1);
  }


  return 0;
}


void sigalrm_handler(){
	printf("Alarm shot!! \n");
	if(SEND==TRUE||counterNumOfAttempts == linkLayer->numTransmissions){
	 STOP=TRUE;
	}
	else SEND=TRUE;
	counterNumOfAttempts++;
 	stats->timeoutOcurrences++;
}



// send frame
//TUDO ainda por acabar
int llwrite(int fd, unsigned char *buffer, int length){

	timeout = 0;
  	alarm_n=2;
  	//signal(SIGALRM, sigalrm_handler);  so se precisa de instalar o alarme uma vez...

	SEND=TRUE;
	//sendMessage(fd, buffer, length);

	unsigned char t[1];
	int state = 0;
	int r = 0;
 	int pos=0;
 	STOP=FALSE;
	typeOfFrame=RR;
	unsigned char tmp[5];
	while(counterNumOfAttempts != linkLayer->numTransmissions && STOP == FALSE){
		if(SEND){
			//printf("enviei istoooo \n");
			sendMessage(fd, buffer, length);
			stats->transmitedTramas++;
			alarm(linkLayer->timeOut);
			SEND = FALSE;
		}
		r =  read(fd,t,1);
				if(r > 0) {
			state = stateMachine(t[0], state, tmp,typeOfFrame,pos);
			pos++;
		}
		if(pos==5){
			if((tmp[2]==((linkLayer->sequenceNumber<<7)+C_REJ))){
				printf("Receiver receives  wrong. send again! \n");
				SEND=TRUE;
				STOP=FALSE;
				pos=0;
				stats->numRej++;
			}
			else if((tmp[2]==((linkLayer->sequenceNumber<<7)+C_RR))){
				printf("Receiver receives repeated \n");
				STOP=TRUE;
				printf("vou sair \n");
				stats->numRR++;
			}

			else if((linkLayer->sequenceNumber==0 && tmp[2]==((1<<7)+C_RR))||(linkLayer->sequenceNumber==1 && tmp[2]==C_RR)){
				printf("Receiver receives correctly \n");
				STOP=TRUE;
				printf("vou sair \n");
			}
		}		
	}
	if(counterNumOfAttempts == linkLayer->numTransmissions){
				perror("Number max of tries achieved");
				exit(3);
			}
	if(linkLayer->sequenceNumber==0)
		linkLayer->sequenceNumber=1;
	else linkLayer->sequenceNumber=0;
	alarm(0);
	counterNumOfAttempts=0;
  return 0;
}

//return size of data in frame
int calculateDataSize(int size){
	return size-6;
}

int readingCycle(TypeOfFrame typeOfFrame,unsigned char * buffer,int fd){
	unsigned char t[1];
	int state = 0;
	int r = 0;
  	int pos=0;
	int lastState=0;
  	STOP=FALSE;
	alarm(linkLayer->timeOut * linkLayer->numTransmissions);
	while(STOP == FALSE){
		r =  read(fd,t,1);
		
		if(r > 0)
			{
				//printf("c =  %c \n", t[0]);
        state = stateMachine(t[0], state, buffer,typeOfFrame,pos);
		if(state==4){
			pos++;
		}else pos=state
		//printf("state %d \n", state);
      }
		
	}
	//printf("teste %d \n", pos);
	if(state != 4 || buffer[pos-1]!=FLAG){
		perror("Lost connection to sender \n");
		exit(3);
	}
	//printf("passei teste %d \n", state);
	alarm(0);
	counterNumOfAttempts=0;
	return pos;
}

// recieve frame
int llread(int fd,unsigned char * data){
	typeOfFrame = INF;
	unsigned char * buffer;
    buffer= (unsigned char *) malloc(2*BLOCK);
	int pos=readingCycle(typeOfFrame,buffer,fd);
	char send[5];
	
	stats->receivedTramas++;
	
	send[0]=FLAG;
	send[1]=A;
	/*int i=0;
	for(i;i<pos;i++){
		printf("recebi istooo %c \n",buffer[i]);	
	}*/
	
	if(pos <= 6){
   		send[2] = (linkLayer->sequenceNumber<<7) + C_REJ;
		sleep(2); 
     	tcflush(fd, TCIOFLUSH);
		send[3] = send[1]^send[2];
		send[4] = FLAG;
		write(fd,send,5);
		free(buffer);
		return -1;
   }



  	memcpy(data,&buffer[4],pos-6);
	int correctSize =  deByteStuffing(data,pos-5);
	int flagMal=0;


	if(blockCheckCharacter(data,correctSize)!=buffer[pos-2]){
		//printf("bcc2 = %d:::o q recebe= %d \n",blockCheckCharacter(data,correctSize),buffer[pos-2]);
    	send[2]=(linkLayer->sequenceNumber<<7)+C_REJ;
		sleep(2);
                tcflush(fd, TCIOFLUSH);
                flagMal=1;
		/*char oi[1024];
		int li=read(fd,oi,1024);
		for(i;i<li;i++){
			printf("recebi istooo %c \n",oi[i]);	
		}
		printf("liiiiiiii= %d \n",li);*/
	}
  	else if((buffer[2])!=(linkLayer->sequenceNumber << 6)){
  	  //printf("recebi repetido %c %c \n",(buffer[2]),(linkLayer->sequenceNumber << 6));
  		flagMal=1;
  		if(linkLayer->sequenceNumber==0)
      		send[2]=(0<<7)+C_RR;
    	else
      		send[2]=(1<<7)+C_RR;

   		 //printf("alalal %c \n", send[2]);
 	}
  else{
    //printf("recebi\n");
    if(linkLayer->sequenceNumber==0)
      linkLayer->sequenceNumber=1;
    else
      linkLayer->sequenceNumber=0;
    send[2]=(linkLayer->sequenceNumber<<7)+C_RR;
    //printf("alalal %c \n", send[2]);
  }
	send[3]=send[1]^send[2];
	send[4]=FLAG;
  write(fd,send,5);
	if(flagMal)
	{
		free(buffer);
		return -1;
	}

	free(buffer);

  return 0;
}


int byteStuffing(unsigned char packet[], int size){
  int i=0;
  for(;i< size;i++){
    if(packet[i]==FLAG||packet[i]==ESC){
      memmove(packet+i+1,packet+i,size-i);
      packet[i]=ESC;
      packet[i+1]=packet[i+1]^0x20;
      size++;
    }
  }
  return size;
}

int deByteStuffing(unsigned char packet[], int size){
  int i=0;
  for(;i< size;i++){
    if(packet[i]==ESC){
      memmove(packet+i,packet+i+1,size-i-1);
      printf("packet teste %d \n",packet[i]);
      packet[i]=packet[i]^0x20;
      printf("packet passei teste %d \n",packet[i]);
      size--;
    }
  }
  return size;
}



void setAndSendDisc(int fd){
  unsigned char DISC[5];

  DISC[0]= FLAG;	
  DISC[1]= A;
  DISC[2]= C_DISC;
  DISC[3]= A^C_DISC;
  DISC[4]= FLAG;
  int res = write(fd,DISC,5);

  if(res != 5){
    perror("error sending DISC");
  }


}



// close serial port
int closeSerialPort(int fd){

	if ( tcsetattr(fd,TCSANOW,&linkLayer->oldtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    close(fd);
    return 0;
}


// end communication
int llclose(int fd, int type){
 	counterNumOfAttempts = 0;
	typeOfFrame = DISC;
	int r = 0;
	unsigned char t[1];
	int state = 0;
	unsigned char tmp[5];
	int pos=0;
	STOP=FALSE;
	SEND=TRUE;
	switch(type){
		case TRANSMITTER:
			
			while(counterNumOfAttempts != linkLayer->numTransmissions && STOP == FALSE){
				if(SEND){
					setAndSendDisc(fd);
					alarm(linkLayer->timeOut);
					SEND = FALSE;
				}
				r =  read(fd,t,1);
    			if(r > 0) {
        			state = stateMachine(t[0], state, tmp,typeOfFrame,pos);
					pos++;
        		}		
			}

			if(counterNumOfAttempts == linkLayer->numTransmissions){
				perror("Number max of tries achieved");
				exit(3);
			}

			alarm(0);
			counterNumOfAttempts=0;
			
			if(STOP == TRUE){
				setAndSendUA(fd);
			}
			
		
	
		
		break;
		case RECEIVER:
	
				readingCycle(typeOfFrame,tmp,fd);
			  
			  if(STOP == TRUE){
			  	STOP = FALSE;
			  	SEND = TRUE; 
			  	bzero(tmp,5);
			  	typeOfFrame = UA;
			  	while(counterNumOfAttempts != linkLayer->numTransmissions && STOP == FALSE){
					if(SEND){
						setAndSendDisc(fd);
						alarm(linkLayer->timeOut);
						SEND = FALSE;
					}
					readingCycle(typeOfFrame,tmp,fd);		
				}
			  }
				if(counterNumOfAttempts == linkLayer->numTransmissions){
				perror("Number max of tries achieved");
				exit(3);
			}
			alarm(0);
			counterNumOfAttempts=0;
		
		
		break;
		}
	



	if(closeSerialPort(fd) != 0){
		perror("serial port with problems");
		exit(3);
	}
	else{
		printf("closed serial port successfully \n");
	}
	printStats();

  return 0;
}

unsigned char blockCheckCharacter(unsigned char buffer[], int size){
	int i=0;
	int bcc2=0;
	for(;i<size;i++){
		bcc2=bcc2^buffer[i];
	}
	return bcc2;
}


int connectTransmitter(int fd){
  typeOfFrame = UA;
  counterNumOfAttempts = 0;
  int r ;
  int state = 0;
  unsigned char t[1];
  unsigned char tmp[5];
  int pos=0;
  while(counterNumOfAttempts != linkLayer->numTransmissions && STOP == FALSE){
    if(SEND){
      setAndSendSET(fd);
      alarm(linkLayer->timeOut);
      SEND = FALSE;
    }
      r =  read(fd,t,1);
      if(r > 0)
        {
          state = stateMachine(t[0], state, tmp,typeOfFrame,pos);
          pos++;
        }
  }
  if(counterNumOfAttempts == linkLayer->numTransmissions){
				perror("Number max of tries achieved");
				exit(3);
	}
alarm(0);
counterNumOfAttempts=0;
  return 0;
}


int connectReciever(int fd){ //receiver esta mal escrito
typeOfFrame = SET;
unsigned char tmp[5];
readingCycle(typeOfFrame,tmp,fd);

  //	printf("tmp[0] = %c , tmp[1] = %c, tmp[2] = %c , tmp[3] = %c , tmp[4] = %c \n" , tmp[0], tmp[1], tmp[2], tmp[3], tmp[4]);

    if(STOP == TRUE){
      setAndSendUA(fd);
      return 0;
    }

  return -1; // someting wnet wrong
}



void setAndSendSET(int fd){
  unsigned char SET[5];
  SET[0]= FLAG;
  SET[1]= A;
  SET[2]= C_SET;
  SET[3]= A^C_SET;
  SET[4]= FLAG;

  int res = write(fd,SET,5);

  if(res != 5){
    perror("error sending SET");
  }

}

void setAndSendUA(int fd){
  unsigned char UA[5];
  UA[0] = FLAG;
  UA[1] = A;
  UA[2] = C_UA;
  UA[3] = A^C_UA;
  UA[4] = FLAG;

  int val = write(fd,UA,5);
  if(val != 5){
    perror("Error sending UA");
  }
}

int stateMachine(unsigned char c, int state, unsigned char tmp[],TypeOfFrame type,int pos){

	switch(state){
	case 0:
		if(c == FLAG){
			tmp[pos]	= c;
			state++;
		}
	break;
	case 1:
		if(c == A){
			tmp[pos] = c;
			state++;
		}
		else if(c != FLAG){
			state = 0;
		}
	break;
	case 2:
	if(c == FLAG){
			state = 1;
		}
		
		else if((c == C_UA && type==1)||(c == C_SET && type==0)||(type==2)||(c==C_DISC && type==3)||(type==4)||(type==5)){

			tmp[pos] = c;
			state++;
		}
		else{
		 state = 0;
		}
	break;
	case 3:
		if(c == (tmp[1]^tmp[2])){
			tmp[pos] = c;
			state++;
		}
		else if(c == FLAG)
			state = 1;
		else
			state = 0;
	break;
	case 4:
		if(c == FLAG){
			tmp[pos] = c;
			STOP = TRUE;
		}
		else if (type!=2)
			state = 0;
   		else if(type==2){
      		tmp[pos]=c;
    	}
	break;
	}

	return state;
}


/*void sendControlPackage(int control, int fd, char* filename, char* filesize)
{

	int sizeof_filesize = strlen(filesize);

	int packageSize = 5 + sizeof_filesize + strlen(filename);


	unsigned char controlPackage[packageSize];

	controlPackage[0] = control;
	controlPackage[1] = PARAM_FILESIZE;
	controlPackage[2] = sizeof_filesize;
	//Inserir o filesize
	int i=0;
	for(i; i < sizeof_filesize; i++)
	{
		controlPackage[3+i] = filesize[i];
	}
	int pos = 3+ sizeof_filesize;
	controlPackage[pos] = PARAM_FILENAME;
	controlPackage[++pos] = strlen(filename);
	//inserir o filename
	i=0;
	for(i; i < strlen(filename); i++)
	{
		controlPackage[++pos] = filename[i];
	}

}	*/

int sendMessage(int fd, unsigned char* buf, int buf_size)
{
	unsigned char* message = (unsigned char*)malloc(2*BLOCK);//createMessage(buf, buf_size);
	//buf_size = buf_size + (6 * sizeof(char));
	//buf_size = byteStuffing(message, buf_size); //vamos tentar enviar 1ยบ sem bytestuffing
	int size=createMessage(buf,message, buf_size);
	int num;
	tcflush(fd,TCIOFLUSH);
	num = write(fd, message, size);
	if(num != size)
	  printf("Error sending message \n");

	free(message);

	return size - 6;
}

int createMessage(const unsigned char* buf,unsigned char* message, int buf_size)
{

	unsigned char* newBuff=(unsigned char*)malloc(2*BLOCK);
	memcpy(newBuff,buf,buf_size);
	unsigned char controlCamp = linkLayer->sequenceNumber << 6;
	//unsigned char BCC1 = A ^ controlCamp;
	char BCC2=blockCheckCharacter(&newBuff[0],buf_size);
	buf[buf_size]=BCC2;
	/*unsigned char BCC2;
	int i;
	for(i = 0; i < buf_size; i++)
	  BCC2 ^= buf[i]; //eu ja tinha criado uma funcao q fazia isto...*/

	message[0] = FLAG;
	message[1] = A;
	message[2] = controlCamp;
	message[3] = A^controlCamp;

	int newSize;// = buf_size;
	//if(counterNumOfAttempts == 0){
	//printf("oioioioioioi %d",contador);
	//contador++;
	newSize = byteStuffing(newBuff, buf_size+1);
	//}
	memcpy(&message[4], newBuff, newSize);
	//message[4+newSize] = BCC2;
	message[4+newSize] = FLAG;
	free(newBuff);
	return newSize+6;
 }



void printStats(){

	printf("Numero de tramas transmitidas: %d \n",stats->transmitedTramas);
	printf("Numero de tramas recebidas: %d \n", stats->receivedTramas);
	printf("Numero de timeOuts: %d \n", stats->timeoutOcurrences);
	printf("Numero de tramas rejeitadas: %d \n", stats->numRej);
	printf("Numero de tramas repetidas: %d \n", stats->numRR);

}
