#include "url.h"

// ftp://[<user>:<password>@]<host>/<url-path>
int init(char * url_name, url * url){

  char * ftp = malloc(6*sizeof(char));
  memcpy(ftp, url_name, 6);

  if(strcmp(ftp, "ftp://") != 0){
    perror("the url should start with ftp://");
    exit(1);
  }

	const char b[2] = ":";
	const char c[2] = "@";
	const char d[2] = "";
	const char e[2] = "/";
	url_name+=6;

  int i;
  int var = 0;
  printf("%s\n",url_name);

  for(i=0; i < strlen(url_name);i++){
      if(url_name[i] == ':'){
        var = 1;
        break;
      }
  }

url->user =(char*)malloc(100);

  if(var == 1){
  	if((url->user = strtok(url_name, b))==NULL)
  		return -1;
  	if((url->password = strtok(NULL, c))==NULL)
  		return -1;
  	if((url->host = strtok(NULL, e))==NULL)
  		return -1;
  	if((url->url_patch = strtok(NULL, d))==NULL) //nao seria suposto ser url->path?
  		return -1;
  }else{
      char *name=(char*)malloc(100);
 	char *pass=(char*)malloc(100);

      url->user = "anonymous";
      url->password = "oi";

      if((url->host = strtok(url_name, e))==NULL)
        return -1;
      if((url->url_patch = strtok(NULL, d))==NULL) //nao seria suposto ser url->path?
        return -1;
    }

  return 0;

}
