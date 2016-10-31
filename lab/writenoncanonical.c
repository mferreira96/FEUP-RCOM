/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <signal.h>

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define FLAG 0x7e
#define A 0x03

#define C_SET 0x03
#define C_UA 0X07


#define CONTROL_DATA	0x01
#define CONTROL_START	0x02
#define CONTROL_END		0x03

#define C 0x00


volatile int STOP=FALSE;

char trama[5];
char trama_leitura[5];
int fd;
int timeout = 0;




void sigalrm_handler(int sig)
{
	printf("%d \n", timeout);
	sendSet();
	timeout++; // se = 3, parar de tentar
	if(timeout <3)
		alarm(3);
	else STOP=TRUE;

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
			printf("vou sair \n");
			STOP = TRUE;
			alarm(0);
		}
		else
			state = 0;
	break;
	}

	return state;
}


void sendSet(){

	printf("vou enviar! \n");
	trama[0]= FLAG;
	trama[1]= A;
	trama[2]= C_SET;
	trama[3]= A^C_SET;
	trama[4]= FLAG;

	int res = write(fd,trama,sizeof(trama));//envia a trama

	printf("escrevi: %d \n",res);
	//printf("tmp[0] = %c , tmp[1] = %c, tmp[2] = %c , tmp[3] = %c , tmp[4] = %c \n" , trama[0], trama[1], trama[2], trama[3], trama[4]);

}


void sendTramaStart()
{
	char start[12];

	start[0]=FLAG;
	start[1]=A;
	start[2]=C;
	start[3]=A^C;
	start[4]=CONTROL_START;
	start[5]=C;
	start[6]=1;
	start[7]=1;
	start[8]=1;
	start[9]=1;
	start[10]='A';
	start[11]=FLAG;


	int res = write(fd,start,sizeof(start));//envia a trama

	printf("escreviNEW: %d \n",res);



}

int main(int argc, char** argv)
{
    int c, res;
    struct termios oldtio,newtio;
    char buf[255];
    int i = 0, sum = 0, speed = 0;
	unsigned char tmp[5];

    if ( (argc < 2) ||
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) &&
  	      (strcmp("/dev/ttyS1", argv[1])!=0) )) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }




    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd <0) {perror(argv[1]); exit(-1); }

    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
      perror("tcgetattr");
      exit(-1);
				signal(SIGALRM, sigalrm_handler);

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 1;   /* blocking read until 1 chars received */



    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    printf("New termios structure set\n");


	int size = 5;  // contains the size of the text *******************************
	int wrote = 0;	// contains the size of the text that was wrote that time
	int text = 0;	//  contains the size of the text that was wrote






/*
	while(timeout<3){


 		//sleep(1);
		if(r=read(fd, &trama_leitura, 5)); //read one char by time
			printf("lii: %s \n",trama_leitura);
			rt+=r;
		if(rt==5)
			break;
	}
   	printf("\n");

*/

signal(SIGALRM, sigalrm_handler);
sendSet();
sendTramaStart();
alarm(3);


	unsigned char t[1];
	int state = 0;
	//sleep(2);
	int r = 0;

	while(STOP == FALSE){
		r =  read(fd,t,1);
		printf("c =  %c \n", t[0]);
		if(r > 0)
			state = stateMachine(t[0], state, tmp);
	printf("state %d \n", state);
	}




	sleep(1);
	printf("Sai do ciclo!!!");

	if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    close(fd);
    return 0;
}
