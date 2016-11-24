#include "url.h"


// ftp://[<user>:<password>@]<host>/<url-path>
void init(char * url_name, url * url){

  char * ftp = malloc(6*sizeof(char));
  memcpy(ftp, url_name, 6);

  if(strcmp(ftp, "ftp://") != 0){
    perror('error, the url should start with ftp:// \n');
    exit(1);
  }

 // e preciso dividir o url
}
