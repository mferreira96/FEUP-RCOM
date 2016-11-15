#include <string.h>
#include "connection.h"

int get_ip(){

  struct hostent *h;

  if ((h=gethostbyname(argv[1])) == NULL) {
      herror("gethostbyname");
      exit(1);
  }

  printf("Host name  : %s\n", h->h_name);
  printf("IP Address : %s\n",inet_ntoa(*((struct in_addr *)h->h_addr)));

  return 0;
}


int connect(){

  struct	sockaddr_in server_addr;
/*server address handling*/
  bzero((char*)&server_addr,sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);	/*32 bit Internet address network byte ordered*/
  server_addr.sin_port = htons(SERVER_PORT);		/*server TCP port must be network byte ordered */

/*open an TCP socket*/
if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
    perror("socket()");
    exit(0);
  }
/*connect to the server*/
  if(connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
    perror("connect()");
    exit(0);
  }
  return 0;
}


int login_host(){

  return 0;
}

int passive(){

  return 0;
}

int get_path(){

  return 0;
}


int download(){

  return 0;
}
