#include "url.h"

/*
char str[80]="ftp://[up201305016:BoraVanessa@]129.56.53.1/urlpath";

init(str,<url structure>);

tem de ser passado exclusivamente neste tipo de estrutura char str[N]=..., caso contrário temos uma visita da segmentation fault
*/

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
	if((url->user = strtok(url_name, b))==NULL)
		return -1;
	if((url->password = strtok(NULL, c))==NULL)
		return -1;
	if((url->host = strtok(NULL, e))==NULL)
		return -1;
	if((url->url_patch = strtok(NULL, d))==NULL) //nao seria suposto ser url->path?
		return -1;

printf("user: %s \n",url->user);
printf("pass: %s \n",url->password);
printf("host: %s \n",url->host);
printf("path: %s \n",url->url_patch);
	
  return 0;

}
