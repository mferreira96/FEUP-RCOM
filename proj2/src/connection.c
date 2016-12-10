
#include "connection.h"



int get_ip(connection * connection, char * host	){

  struct hostent *h;

	printf("host %s \n",host);

  if ((h=gethostbyname(host)) == NULL) {
      herror("gethostbyname");
      exit(1);
  }

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

printf("ip: %d \n",connection->port);

/*connect to the server*/
  if(connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
    perror("connect()");
    exit(0);
  }

  connection->fileDescriptor=sockfd;

  	char response[40];
	bzero(response,40);
  	read(connection->fileDescriptor, response, 40);
	if(response[0]!='2' ||response[1]!='2' ||response[2]!='0')
		return -1;
	
	char responseSaid[40];
	bzero(responseSaid,40);
	strncpy ( responseSaid, response+4, 36);
	printf("%s",responseSaid);


  return 0;
}


int login_host(connection * connection, url * url){
  char * user;
  user =(char*) malloc(50*sizeof(char));
  char * pass;
  pass =(char*) malloc(50*sizeof(char));
  
	char response[40];

 sprintf(user, "USER %s\n", url->user);

  write(connection->fileDescriptor, user, strlen(user));
  read(connection->fileDescriptor, response, 40);
  printf("%s",response);
char responseSaid[4];
	bzero(responseSaid,4);
	strncpy ( responseSaid, response, 3);

if(strcmp(responseSaid,"331")!=0){
    return -1;}

  bzero(response,4);
	char responsePass[40];
bzero(responsePass,40);
  sprintf(pass, "PASS %s\n", url->password);
  write(connection->fileDescriptor, pass, strlen(pass));
  read(connection->fileDescriptor, responsePass, 40);

  printf("%s",responsePass);

bzero(responseSaid,4);
	strncpy ( responseSaid, responsePass, 3);

  if(strcmp(responseSaid,"230")!=0)
    return -1;


  return 0;
}

int passiveMode(connection * connection1,connection * connection2){

  char response[1000];
bzero(response,1000);
  write(connection1->fileDescriptor, "pasv\n", strlen("pasv\n"));
  read(connection1->fileDescriptor, response, 1000);
	
	char responseSaid[4];
	bzero(responseSaid,4);
	strncpy ( responseSaid, response, 3);
	if(strcmp(responseSaid,"227")!=0)
		return -1;

  int ip1, ip2, ip3, ip4, p1, p2;
  sscanf(response, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)", &ip1,&ip2, &ip3, &ip4, &p1, &p2);
	
	printf("%s",response);

	connection2->ip=(char*)malloc(sizeof( char)*50);

  sprintf(connection2->ip, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);



  connection2->port = p1*256+p2;



  return 0;
}

int get_path(){

  return 0;
}


int download(connection * connection, char * path){

  char * fileName = basename(path);
  char data[BUF_SIZE];

  int fd = open(fileName, O_RDWR | O_CREAT | O_TRUNC);

  if(fd == -1){
    perror("open");
    close(connection->fileDescriptor);
    exit(1);
  }


  size_t nr;
  size_t nw;
  while((nr = read(connection->fileDescriptor,data,BUF_SIZE)) > 0){

    nw = write(fd,data,nr);

    if(nw == -1){
      perror("write on file");
      close(fd);
      close(connection->fileDescriptor);
      exit(1);
    }

  }

  if(nr == -1){
    perror("reading file");
    close(fd);
    close(connection->fileDescriptor);
    exit(1);
  }

  close(fd);
  close(connection->fileDescriptor);

  return 0;
}
