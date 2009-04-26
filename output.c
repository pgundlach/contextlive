#include <cgic.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <sys/errno.h>

int cgiMain() {
  int retry;
  int len,fd;
  char *cptr;
  char buf[1024];
  char dirname[1024];
  char *progname;
#ifdef SCHNEE
  const char *css="/main.css";
#else
  const char *css="http://static.contextgarden.net/min.css";
#endif
  
  progname=cgiScriptName;
  /* setbuf(stdout, NULL); */
  cgiHeaderContentType("text/html"); 
  fprintf(cgiOut, "<html><head>\n");
  fprintf(cgiOut, "<title>texexec output</title>\n");
  fprintf(cgiOut, 
	  "<link rel=\"stylesheet\" type=\"text/css\" href=\"%s\">",css);
  fprintf(cgiOut, "</head><body><pre class=\"output\">\n");
 
  cgiFormString("id", dirname, sizeof(dirname));
  /* fprintf(stderr,"id=%s, %p, len=%d\n",dirname,dirname,strlen(dirname));  */

  
  if (strlen(dirname) <3) {
    perror ("strange id");
    exit (-1); 
  }

  cptr=strstr(dirname,"/");
  if (cptr) {
    fprintf(stderr,"string has / in it, exiting\n");
    exit(-1);
  }

  strcpy(buf,"/tmp/texweb/"); 
  strncat(buf,dirname,(sizeof(buf) - strlen(buf)));
  strcat(buf,"/output"); 

/*   strcpy(buf,"/tmp/output2"); */
  fd=open(buf, O_NONBLOCK);
  if (fd==-1) {
    fprintf(stderr,"%s: open error: %s (%s)\n",progname, strerror(errno),buf);
    exit(-1);
  }
  
  retry=1;
  while (retry > -1) {
    len=read(fd,buf,sizeof(buf));
    if (len==-1) {
      perror("read");
      exit(-1);
    } else if (len==0) {
      /* eof? let us wait a few seconds and try again */
      retry = retry -1;
      if (retry == 0) { 
	/* ok, there is not much more we can do */
	sleep (2);
      }
    } else {  /* len > 0 */
      cgiHtmlEscapeData(buf,len);
      retry=1;
      /* printf ("bytes read: %d\n",ret); */
      /*       write(1,buf,ret); */
    }
  }
  fprintf(cgiOut, "</pre></body></html>\n");
  return 0;
}
