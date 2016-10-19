
#include "DataLink.h"

volatile int STOP=FALSE;
volatile int SEND=TRUE;

unsigned int counterNumOfAttempts;

LinkLayer * linkLayer;

void configLinkLayer(int flagMode){
  linkLayer = (LinkLayer*) malloc(sizeof(LinkLayer));

  char * port;
  if(flagMode == TRANSMITER){
    port = "/dev/ttyS0";
    strcpy(linkLayer->port,port);
    printf("Sou transmissor\n");
  }else{
    port = "/dev/ttyS1";
    strcpy(linkLayer->port, port);
    printf("Sou receptor\n");
  }

  linkLayer->numTransmissions = 3;
  linkLayer->baudRate = BAUDRATE;
  linkLayer->timeout = 3;

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

int llopen(int flagMode){
  int fd;

  fd = open(linkLayer->port, O_RDWR | O_NOCTTY );      // open SerialPort
  if (fd <0) {perror(linkLayer->port); exit(-1); }


  if (setNewTermios(fd) != 0){            // set new termios
    perror("Error setting new termios");
    exit(2);
  }

  signal(SIGALRM, sigalrm_handler);       //sigalrm_handler is the function  called when the alarm is fired

  int error;
  if(flagMode == TRANSMITER){
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
  SEND = TRUE;
  counterNumOfAttempts++;
}


// send frame
int llwrite(int fd, char *buffer, int length){

  return 0;
}

// recieve frame
int llread(int fd, char * buffer){

  return 0;
}


// end communication
int llclose(int fd){

  return 0;
}


int connectTransmitter(int fd){

  counterNumOfAttempts = 1;
  int r ;
  int state = 0;
  unsigned char t[1];
  unsigned char tmp[5];

  while(counterNumOfAttempts != linkLayer->numTransmissions && STOP == FALSE){
    if(SEND){
      setAndSendSET(fd);
      alarm(linkLayer->timeout);
      SEND = FALSE;
    }
      r =  read(fd,t,1);
      printf("c =  %c \n", t[0]);
      if(r > 0)
        state = stateMachine(t[0], state, tmp);

    printf("state %d \n", state);

  }
  if(STOP == TRUE) // Everything went well
    return 0;

  return -1; // deu erro
}


int connectReciever(int fd){

unsigned char tmp[5];
unsigned char t[1];
int state = 0;
int r = 0;

STOP=FALSE;
  while(STOP == FALSE){
  	r =  read(fd,t,1);
  	printf("c =  %c \n", t[0]);
  	if(r > 0)
  			state = stateMachine(t[0], state, tmp);
  	printf("state %d \n", state);
  }

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


int stateMachine(unsigned char c, int state, char tmp[]){


	switch(state){
	case 0:
		if(c == FLAG){
			tmp[state]	= c;
			state++;
		}
	break;
	case 1:
		if(c == A){
			tmp[state] = c;
			state++;
		}
		else if(c != FLAG){
			state = 0;
		}
	break;
	case 2:
		if(c == C_UA){
			tmp[state] = c;
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
			tmp[state] = c;
			state++;
		}
		else if(c == FLAG)
			state = 1;
		else
			state = 0;
	break;
	case 4:
		if(c == FLAG){
			tmp[state] = c;
			STOP = TRUE;
		}
		else
			state = 0;
	break;
	}

	return state;
}
