#include "connection.h"


int main (int argc, char *argv[]) {

  if(argc != 2){
    perror("Error on number of arguments");
    exit(1);
  }
  url * receivedUrl;
 receivedUrl = (url *) malloc(sizeof(url));

  if(init(argv[1],receivedUrl)!=0){
    perror("url was not in the corretc form - ftp://[<user>:<password>@]<host>/<url-path>");
    exit(1);
  }

  connection * connectionFTP;
  connectionFTP =(connection*) malloc(sizeof(connection));
  
 // char *host; //este host é suposto ser igual ao url->host q é passado como argumento?
  //host =(char*) malloc(50*sizeof(char));
  printf("lolololol1 %s \n",receivedUrl->user);
  if(get_ip(connectionFTP,receivedUrl->host)!=0){
    perror("Error getting ip");
    exit(1);
  }
    printf("lolololol1 %s \n",receivedUrl->user);
  if(connectTo(connectionFTP)!=0){
    perror("Unable to connect");
    exit(1);
  }

  printf("lolololol1 %s \n",receivedUrl->user);
  	char response[100];
	bzero(response,100);
  	read(connectionFTP->fileDescriptor, response, 100);
	if(response[0]!='2' ||response[1]!='2' ||response[2]!='0')
		return -1;
	printf("uifhduifhsiuhm \n");
  printf("lolololol1 %s \n",receivedUrl->user);
	char responseSaid[100];
	bzero(responseSaid,100);
	strncpy ( responseSaid, response+4, 96);
	printf("%s \n",responseSaid);
  printf("lolololol1 %s \n",receivedUrl->user);

  if(login_host(connectionFTP,receivedUrl)!=0){
    perror("Unable to login");
    exit(1);
  }
  
  connection * connectionFTP2;
  connectionFTP2 =(connection*) malloc(sizeof(connection));
  if(passiveMode(connectionFTP,connectionFTP2)!=0){
    perror("Unable to passive");
    exit(1);
  }

	connectTo(connectionFTP2);

	download(connectionFTP->fileDescriptor,connectionFTP2, receivedUrl->url_patch);
 
	disconnect(connectionFTP,receivedUrl);




return 0;
}
