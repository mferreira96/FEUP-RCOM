
#include "DataLink.h"

volatile int STOP=FALSE;
volatile int SEND=TRUE;

unsigned int counterNumOfAttempts;

void configLinkLayer(){
  strcpy(linkLayer->port, "/dev/ttyS0");
  linkLayer->numTransmissions = 3;
  linkLayer->baudRate = BAUDRATE;
  linkLayer->timeout = 3;
}

int llopen(char *port, int flagMode){
  int fd;
  struct termios oldtio,newtio;

  fd = open(port, O_RDWR | O_NOCTTY );
  if (fd <0) {perror(port); exit(-1); }

  if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
    perror("tcgetattr");
    exit(-1);
  }

  bzero(&newtio, sizeof(newtio));
  newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR;
  newtio.c_oflag = 0;

  /* set input mode (non-canonical, no echo,...) */
  newtio.c_lflag = 0;

  newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
  newtio.c_cc[VMIN]     = 1;   /* blocking read until 1 chars received */



  tcflush(fd, TCIOFLUSH);

  if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
    perror("tcsetattr");
    exit(-1);
  }

  signal(SIGALRM, sigalrm_handler);
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

int llwrite(int fd, char *buffer, int length){

  return 0;
}


int llread(int fd, char * buffer){

  return 0;
}

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

      r =  read(fd,t,1);
      printf("c =  %c \n", t[0]);
      if(r > 0)
        state = stateMachine(t[0], state, tmp);

    printf("state %d \n", state);
    }
  }
  if(STOP == TRUE) // Everything went well
    return 0;

  return -1; // deu erro
}


int connectReciever(int fd){

  return -1; // deu erro
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
