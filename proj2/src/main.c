#include "connection.h"


int main (int argc, char *argv[]) {

  if(argc != 2){
    perror("Error on number of arguments");
    exit(1);
  }
  url * receivedUrl;
 receivedUrl = (url *) malloc(sizeof(url));

  if(fdp(argv[1],receivedUrl)!=0){
    perror("url was not in the corretc form - ftp://[<user>:<password>@]<host>/<url-path>");
    exit(1);
  }
  
  connection * connectionFTP;
  connectionFTP =(connection*) malloc(sizeof(connection));
  
  char *host; //este host é suposto ser igual ao url->host q é passado como argumento?
  host =(char*) malloc(50*sizeof(char));
  if(get_ip(connectionFTP,host)!=0){
    perror("Error getting ip");
    exit(1);
  }
  
  if(connectTo(connectionFTP)!=0){
    perror("Unable to connect");
    exit(1);
  }
  
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
  




return 0;
}
