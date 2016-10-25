#include "DataLink.h"

volatile int STOP=FALSE;
volatile int SEND=TRUE;

unsigned int counterNumOfAttempts;
unsigned int timeout;
unsigned int alarm_n;

int contador=0;
LinkLayer * linkLayer;
TypeOfFrame typeOfFrame;



int configLinkLayer(int flagMode){
  linkLayer = (LinkLayer*) malloc(sizeof(LinkLayer));

  char * port;
  port = "/dev/ttyS0";
  strcpy(linkLayer->port,port);
  linkLayer->numTransmissions = 3;
  linkLayer->baudRate = BAUDRATE;
  linkLayer->timeOut = 3;

  return 1;

}

int setNewTermios(int fd){

  if ( tcgetattr(fd,&linkLayer->oldtio) == -1) { /* save current port settings */
    perror("tcgetattr");
    exit(-1);
  }

  bzero(&linkLayer->newtio, sizeof(linkLayer->newtio));
  linkLayer->newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
  linkLayer->newtio.c_iflag = IGNPAR;
  linkLayer->newtio.c_oflag = 0;

  /* set input mode (non-canonical, no echo,...) */
  linkLayer->newtio.c_lflag = 0;

  linkLayer->newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
  linkLayer->newtio.c_cc[VMIN]     = 1;   /* blocking read until 1 chars received */



  tcflush(fd, TCIOFLUSH);

  if ( tcsetattr(fd,TCSANOW,&linkLayer->newtio) == -1) {
    perror("tcsetattr");
    exit(-1);
  }

  return 0;
}

/*
int llopen(char *port, int flagMode){
  int fd;
  alarm_n=1;

  fd = open(port, O_RDWR | O_NOCTTY );      // open SerialPort
  if (fd <0) {perror(port); exit(-1); }

  if (setNewTermios(fd) != 0){            // set new termios
    perror("Error setting new termios");
    exit(2);
  }

  signal(SIGALRM, sigalrm_handler);       //sigalrm_handler is the function  called when the alarm is fired
  printf("cheguei aqui \n");
  int error;
  if(flagMode == TRANSMITTER){
    error = connectTransmitter(fd); // Transmitter try to stablish communication

char tamanho[4];
sprintf(tamanho,"%ld",500);
printf("tamanho : %s \n",tamanho);
    char Frame[20];
	Frame[0]=FLAG;
Frame[1]=A;
Frame[2]=contador;
Frame[3]=Frame[1]^Frame[2];
Frame[4]=2;
Frame[5]=0;
Frame[6]=4;
printf("cheguei aqui \n");
memmove(Frame+7,tamanho,4);
printf("cheguei aqui \n");
Frame[11]=1;
Frame[12]=strlen("ONOME");
memmove(Frame+13,"ONOME",5);
Frame[18]= blockCheckCharacter(&Frame[4],14);
Frame[19]=FLAG;

	write(fd,Frame,20);
  }
  else{
    error = connectReciever(fd);    //Reciever try to stablish communication
    char * buffer;
    buffer= (char *) malloc(50);
    llread(fd,buffer);
    free(buffer);
  }

  if(error == -1){
    perror("Error trying to stablish communication");
    exit(1);
  }


  return 0;
}

*/

int llopen(int fd, int flagMode){
  /*int fd;

  fd = open(port, O_RDWR | O_NOCTTY );      // open SerialPort
  if (fd <0) {perror(port); exit(-1); }

  if (setNewTermios(fd) != 0){            // set new termios
    perror("Error setting new termios");
    exit(2);
  }*/
	alarm_n=1;
  signal(SIGALRM, sigalrm_handler);       //sigalrm_handler is the function  called when the alarm is fired
  printf("cheguei aqui \n");
  int error;
  if(flagMode == TRANSMITTER){
    error = connectTransmitter(fd); // Transmitter try to stablish communication

/*char tamanho[4];
sprintf(tamanho,"%ld",500);
printf("tamanho : %s \n",tamanho);
    char Frame[20];
	Frame[0]=FLAG;
Frame[1]=A;
Frame[2]=contador;
Frame[3]=Frame[1]^Frame[2];
Frame[4]=2;
Frame[5]=0;
Frame[6]=4;
printf("cheguei aqui \n");
memmove(Frame+7,tamanho,4);
printf("cheguei aqui \n");
Frame[11]=1;
Frame[12]=strlen("ONOME");
memmove(Frame+13,"ONOME",5);
Frame[18]= blockCheckCharacter(&Frame[4],14);
Frame[19]=FLAG;

	write(fd,Frame,20);*/
  }
  else{
    error = connectReciever(fd);    //Reciever try to stablish communication
    /*char * buffer;
    buffer= (char *) malloc(50);
    llread(fd,buffer);
    free(buffer);*/
  }

  if(error == -1){
    perror("Error trying to stablish communication");
    exit(1);
  }


  return 0;
}


void sigalrm_handler(){
  
  if(alarm_n == 1)
  {
   SEND = TRUE;
   counterNumOfAttempts++;
  }
  else if (alarm_n == 2)
  {
      timeout++;
      printf("\n Timeout, llwrite n%d \n",timeout);

        /*if(timeout >=linkLayer->timeout)
          alarm(0);
        else
        alarm(linkLayer->timeout);*/ //ta a dar erro de compilacao...

  }
  else
    printf("\n erro alarm handler \n");
}




// send frame
//TUDO ainda por acabar
int llwrite(int fd, char *buffer, int length){

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
	while(counterNumOfAttempts != linkLayer->numTransmissions && STOP ==FALSE){
		if(SEND){
			//setAndSendDisc(fd);
			printf("enviei istoooo \n");
			sendMessage(fd, buffer, length);
			alarm(linkLayer->timeOut);
			SEND = FALSE;
		}
		r =  read(fd,t,1);
		printf("c =  %c \n", t[0]);
		if(r > 0) {
			state = stateMachine(t[0], state, tmp,typeOfFrame,pos);
			pos++;
		}
		if(pos==5){
			if((tmp[2]==((linkLayer->sequenceNumber<<7)+C_RR))||(tmp[2]==((linkLayer->sequenceNumber<<7)+C_REJ))){
				printf("Receiver receives same or wrong. send again! \n");
				SEND=FALSE;
				STOP=FALSE;
				pos=0;
			}
			else if((linkLayer->sequenceNumber==0 && tmp[2]==((1<<7)+C_RR))||(linkLayer->sequenceNumber==1 && tmp[2]==C_RR)){
				printf("Receiver receives correctly \n");
				STOP=TRUE;
				printf("vou sair \n");
			}
		}		
	}
  printf("\n \n Saiu do ciclo \n");

  return 0;
}
//return size of data in frame
int calculateDataSize(int size){
	return size-6;
}

/*int sendFile(int fd){
	FILE *f=fopen("teste.txt","r");
    int size;
    char buffer[50];
    // ...
    while((size=fread(buffer,BLOCK,sizeof(char),f)>0)
            //fwrite(buffer,size,sizeof(char),stdout); chamar llwrite
    fclose(f);
}*/

int readingCycle(TypeOfFrame typeOfFrame,char * buffer,int fd){
	unsigned char t[1];
	int state = 0;
	int r = 0;
  int pos=0;
  STOP=FALSE;
	while(STOP == FALSE){
		r =  read(fd,t,1);
		printf("c =  %c \n", t[0]);
		if(r > 0)
			{
        state = stateMachine(t[0], state, buffer,typeOfFrame,pos);
        pos++;
      }
		printf("state %d \n", state);
	}
	return pos;
}




/// recieve frame
int llread(int fd, char * data){
	typeOfFrame = INF;
	char * buffer;
    buffer= (char *) malloc(100);
	int pos=readingCycle(typeOfFrame,buffer,fd);
	/*unsigned char t[1];
	int state = 0;
	int r = 0;
  int pos=0;
	STOP=FALSE;
	while(STOP == FALSE){
		r =  read(fd,t,1);
		printf("c =  %c \n", t[0]);
		if(r > 0)
			{
        state = stateMachine(t[0], state, buffer,typeOfFrame,pos);
        pos++;
      }
		printf("state %d \n", state);
	}*/
	char send[5];
	send[0]=FLAG;
	send[1]=A;
  int i=0;

  char counter[1];
//  sprintf(counter,"%d",contador);
 // char *data;
 // data= (char *) malloc(50);

  memcpy(data,&buffer[4],calculateDataSize(pos));


	if(blockCheckCharacter(data,14)!=buffer[pos-2]){
    printf("recebi mal o bcc2\n");
    send[2]=(linkLayer->sequenceNumber<<7)+C_REJ;
    printf("alalal %c", send[2]);
	}
  else if((buffer[2])!=(linkLayer->sequenceNumber << 6)){
    printf("recebi repetido \n");
    if(linkLayer->sequenceNumber==0)
      send[2]=(0<<7)+C_RR;
    else
      send[2]=(1<<7)+C_RR;

    printf("alalal %c \n", send[2]);
  }
  else{
    printf("recebi\n");
    if(linkLayer->sequenceNumber==0)
      linkLayer->sequenceNumber=1;
    else
      linkLayer->sequenceNumber=0;
    send[2]=(linkLayer->sequenceNumber<<7)+C_RR;
    printf("alalal %c \n", send[2]);
  }
	send[3]=send[1]^send[2];
	send[4]=FLAG;

  write(fd,send,5);

free(buffer);
  return 0;
}


int byteStuffing(char packet[], int size){
  int i=0;
  for(i;i< size;i++){
    if(packet[i]==FLAG||packet[i]==ESC){
      memmove(packet+i+1,packet+i,size-i);
      packet[i]=ESC;
      packet[i+1]=packet[i+1]^0x20;
      size++;
    }
  }
  return size;
}

int deByteStuffing(char packet[], int size){
  int i=0;
  for(i;i< size;i++){
    if(packet[i]==ESC){
      memmove(packet+i,packet+i+1,size-i);
      packet[i]=packet[i]^0x20;
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

	switch(type){
		case TRANSMITTER:

			while(counterNumOfAttempts != linkLayer->numTransmissions && STOP == FALSE){
				if(SEND){
					setAndSendDisc(fd);
					alarm(linkLayer->timeOut);
					SEND = FALSE;
				}
				r =  read(fd,t,1);
     			printf("c =  %c \n", t[0]);
    			if(r > 0) {
        			state = stateMachine(t[0], state, tmp,typeOfFrame,pos);
        		  	pos++;
        		}
			}

			if(STOP == TRUE){
				setAndSendUA(fd);
			}

			if(counterNumOfAttempts == linkLayer->numTransmissions && STOP == FALSE){
				perror("Number max of tries achieved");
				exit(3);
			}


		break;
		case RECEIVER:
 			/* while(STOP == FALSE){
			  	r =  read(fd,t,1);
			  	printf("c =  %c \n", t[0]);
			  	if(r > 0){
				 state = stateMachine(t[0], state, tmp,typeOfFrame,pos);
				 pos++;
				}
			  	printf("state %d \n", state);
			  }*/

			  if(STOP == TRUE){
			  	STOP = FALSE;
			  	SEND = TRUE;
			  	typeOfFrame = UA;
			  	while(counterNumOfAttempts != linkLayer->numTransmissions && STOP == FALSE){
					if(SEND){
						setAndSendDisc(fd);
						alarm(linkLayer->timeOut);
						SEND = FALSE;
					}
					r =  read(fd,t,1);
			 		printf("c =  %c \n", t[0]);
					if(r > 0) {
						state = stateMachine(t[0], state, tmp,typeOfFrame,pos);
					  	pos++;
					}
				}
			  }


		break;
		}


	if(closeSerialPort(fd) != 0){
		perror("serial port with problems");
		exit(3);
	}
	else{
		printf("closed serial port successfully \n");
	}


  return 0;
}

char blockCheckCharacter(char buffer[], int size){
	int i=0;
	int bcc2=0;

	for(i;i<size;i++){
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
      printf("c =  %c \n", t[0]);
      if(r > 0)
        {
          state = stateMachine(t[0], state, tmp,typeOfFrame,pos);
          pos++;
        }

    printf("state %d \n", state);

  }
  if(STOP == TRUE) // Everything went well
    return 0;

  return -1; // deu erro
}


int connectReciever(int fd){
typeOfFrame = SET;
unsigned char tmp[5];
/*unsigned char t[1];
int state = 0;
int r = 0;
int pos=0;
STOP=FALSE;
  while(STOP == FALSE){
  	r =  read(fd,t,1);
  	printf("c =  %c \n", t[0]);
  	if(r > 0)
  			{
          state = stateMachine(t[0], state, tmp,typeOfFrame,pos);
          pos++;
        }
  	printf("state %d \n", state);
  }*/

  	printf("tmp[0] = %c , tmp[1] = %c, tmp[2] = %c , tmp[3] = %c , tmp[4] = %c \n" , tmp[0], tmp[1], tmp[2], tmp[3], tmp[4]);

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


//tyoe 0->SET, 1->UA, 2->INF, 3->DISC acrescentar as q faltam e substituir por um enum
int stateMachine(unsigned char c, int state, char tmp[],TypeOfFrame type,int pos){
	printf("entrei na state machine %d \n",type);

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
		if((c == C_UA && type==1)||(c == C_SET && type==0)||(type==2)||(c==C_DISC && type==3)||(type==4)||(type==5)){

			tmp[pos] = c;
			state++;
		}
		else if(c == FLAG){
			state = 1;
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

void sendControlPackage(int control, int fd, char* filename, char* filesize)
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
		controlPackage[3+i] = filesize[i];

	int pos = 3+ sizeof_filesize;
	controlPackage[pos] = PARAM_FILENAME;
	controlPackage[++pos] = strlen(filename);
	//inserir o filename
	i=0;
	for(i; i < strlen(filename); i++)
		controlPackage[++pos] = filename[i];





}


void sendMessage(int fd, unsigned char* buf, int buf_size)
{
	unsigned char* message = createMessage(buf, buf_size);
	buf_size = buf_size + (6 * sizeof(char));
	//buf_size = byteStuffing(message, buf_size); vamos tentar enviar 1º sem bytestuffing

	int num;
	num = write(fd, message, buf_size);
	if(num != buf_size)
	  printf("Error sending message \n");

	free(message);
}

unsigned char* createMessage(const unsigned char* buf, int buf_size)
{
	int msg_size = 6 * sizeof(char);
	unsigned char* message = malloc(msg_size + buf_size);

	unsigned char controlCamp = linkLayer->sequenceNumber << 6;
	unsigned char BCC1 = A ^ controlCamp;

	unsigned char BCC2;
	int i;
	for(i = 0; i < buf_size; i++)
	  BCC2 ^= buf[i]; //eu ja tinha criado uma funcao q fazia isto...

	message[0] = FLAG;
	message[1] = A;
	message[2] = controlCamp;
	message[3] = A^controlCamp;
	memcpy(&message[4], buf, buf_size);
	message[4+buf_size] = blockCheckCharacter(&buf[0],buf_size);;
	message[5+buf_size] = FLAG;

	return message;
 }

