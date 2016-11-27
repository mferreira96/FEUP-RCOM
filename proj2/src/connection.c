
#include "connection.h"



int get_ip(connection * connection, char * host	){

  struct hostent *h;

  if ((h=gethostbyname(host)) == NULL) {
      herror("gethostbyname");
      exit(1);
  }

  //printf("Host name  : %s\n", h->h_name);
  connection->ip=inet_ntoa(*((struct in_addr *)h->h_addr));
  connection->port=21;
  
  return 0;
}


int connectTo(connection * connection){
  
  int sockfd;
  struct	sockaddr_in server_addr;
/*server address handling*/
  bzero((char*)&server_addr,sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(connection->ip);	/*32 bit Internet address network byte ordered*/
  server_addr.sin_port = htons(connection->port);		/*server TCP port must be network byte ordered */

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
  
  connection->fileDescriptor=sockfd;
  
  //falta a parte de confirmar com o servidor mandando mensagens
  
  return 0;
}


int login_host(connection * connection, url * url){
  char * user;
  user =(char*) malloc(50*sizeof(char));
  char * pass;
  pass =(char*) malloc(50*sizeof(char));
  char response[4];
  sprintf(user, "USER %s\n", url->user);
  write(connection->fileDescriptor, user, strlen(user));
  read(connection->fileDescriptor, response, 3);
  if(strcmp(response,"331")!=0)
    return -1;
  bzero(response,4);
  
  sprintf(pass, "PASS %s\n", url->password);
  write(connection->fileDescriptor, pass, strlen(pass));
  read(connection->fileDescriptor, response, 3);
  
  if(strcmp(response,"230")!=0)
    return -1;
  
  return 0;
}

int passiveMode(connection * connection1,connection * connection2){
  char response[1000];
  write(connection1->fileDescriptor, "pasv\n", strlen("pasv\n"));
  read(connection1->fileDescriptor, response, 1000);
  
  int ip1, ip2, ip3, ip4, p1, p2;
  sscanf(response, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)", &ip1,&ip2, &ip3, &ip4, &p1, &p2);

  sprintf(connection2->ip, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);

  connection2->port = p1*256+p2;

  return 0;
}

int get_path(){

  return 0;
}


int download(){

  return 0;
}
