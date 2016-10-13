/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

#define FLAG 0x7e
#define A    0x03
#define C_SET    0x03
#define C_UA    0x07


volatile int STOP=FALSE;



int stateMachine(unsigned char c, int state, unsigned char tmp[]){


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
		if(c == C_SET){
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


int main(int argc, char** argv)
{
    int fd,c, res;
    struct termios oldtio,newtio;
    char buf[255];
 	unsigned char tmp[5];
	unsigned char UA[5];


	UA[0] = FLAG;
	UA[1] = A;
	UA[2] = C_UA;
	UA[3] = A^C_UA;
	UA[4] = FLAG;


    if ( (argc < 2) || 
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) && 
  	      (strcmp("/dev/ttyS1", argv[1])!=0) )) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }


  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */
  
    
    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd <0) {perror(argv[1]); exit(-1); }

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

    printf("New termios structure set\n");
/*
	int nr =0;
	int t = 0;
	while(t == 0){
		int msglen = 0;
		while(!msglen){
			int n, k;
			n = read(fd,buf + nr, sizeof(buf) - nr);
			if(n < 0)
				break;

			for(k = 0; k <n ; k++){
				if(buf[nr + k] == '\0'){
					printf("encontrei o 0\n");
					msglen = nr + k + 1;
					t = 1;
					break;
				}
				
			}
			nr += n;
			if(nr == sizeof(buf)){
				nr = 0;	
			}
				


			if(nr > msglen){
				memmove(buf, buf + msglen, nr - msglen);				
			}
			nr -= msglen;
		}

	}

		printf("%s \n", buf);
		printf("size : %d \n", strlen(buf));
		
   	int  i = write(fd,buf,strlen(buf) + 1); 		// write the content read 
	printf("%d \n", i);
*/

	int r = 0;
	unsigned char t[1];
	int state = 0;
	while(1){
	STOP=FALSE;
	while(STOP == FALSE){
		r =  read(fd,t,1);
		printf("c =  %c \n", t[0]);
		if(r > 0)
			state = stateMachine(t[0], state, tmp);
	printf("state %d \n", state);
	}
	
	printf("tmp[0] = %c , tmp[1] = %c, tmp[2] = %c , tmp[3] = %c , tmp[4] = %c \n" , tmp[0], tmp[1], tmp[2], tmp[3], tmp[4]);

	int val = write(fd,UA,5);
	
	if(val !=  5){
		perror("error reading!");		
	}
}

	sleep(1);

    tcsetattr(fd,TCSANOW,&oldtio);
    close(fd);
    return 0;
}
