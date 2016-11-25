#include "url.h"

/*
char str[80]="ftp://[up201305016:BoraVanessa@]129.56.53.1/urlpath";

init(str,<url structure>);

tem de ser passado exclusivamente neste tipo de estrutura char str[N]=..., caso contrário temos uma visita da segmentation fault
*/

// ftp://[<user>:<password>@]<host>/<url-path>
void init(char * url_name, url * url){

  char * ftp = malloc(6*sizeof(char));
  memcpy(ftp, url_name, 6);

  if(strcmp(ftp, "ftp://") != 0){
    perror('error, the url should start with ftp:// \n');
    exit(1);
  }

 const char a[2] = "[";
	const char b[2] = ":";
	const char c[2] = "@";
	const char d[2] = "/";

	strtok(url_name, a);
	url->user = strtok(NULL, b);
	url->password = strtok(NULL, c);
	url->host = strtok(NULL, d);
	url->host++;
	url->url_patch = strtok(NULL, d);  //nao seria suposto ser url->path?
  
  /*
  TESTE para "ftp://[up201305016:BoraVanessa@]129.56.53.1/urlpath"
  
  printf( " %s\n", url->user );
printf( " %s\n", url->password);
printf( " %s\n", url->host);
printf( " %s\n",url->url_patch); 

funcionou bem
  
  */
}
