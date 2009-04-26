#include "cgic.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
int cgiMain() {
  int error;
  ssize_t len;
  ssize_t filesize;
  int fd;
  char *cptr;
  char msgbuf[1024];
  char buf[1024];
  char dirname[1024];
  struct stat statb;

  /* setbuf(stdout, NULL); */
 
  cgiFormString("id", dirname, sizeof(dirname));
  error=0;

  if (strlen(dirname) <3) {
    strcpy (msgbuf,"strange id, exiting\n");
    error=1;
  }

  cptr=strstr(dirname,"/");
  if (cptr) {
    strcpy (msgbuf,"string has / in it, exiting\n");
    error=1;
  }

  strcpy(buf,"/tmp/texweb/"); 
  strncat(buf,dirname,(sizeof(buf) - strlen(buf)));
  strcat(buf,"/texweb.pdf"); 
  fd=open(buf, O_RDONLY);
  if (fd==-1) {
    cgiHeaderLocation("/outerr.php");
    /*     perror ("open"); */
    /*     strcpy(msgbuf,"cannot open pdf file for reading\n"); */
    error=1;
    return 0;
  }

  /* obtain file size */
  if ( stat(buf,&statb) <0) {
    error=1;
    strcpy(msgbuf,"stat error\n");
  }
  filesize= (int) statb.st_size;

  if (error) {
    cgiHeaderStatus(503, "Service unavailable");
    printf("<html><head><title>503</title></head><body><p>%s</p></body></html>",msgbuf);
    close(fd);
    exit(-1);
  }


  printf("Content-Type: application/pdf\r\n"); 
  printf("Content-Length: %d\r\n",filesize); 
  printf("Content-disposition: inline; filename=result.pdf\r\n");
  printf("\r\n");
  fflush(stdout);		/* this took me some time to debug */
  while ( (len=read(fd,buf,sizeof(buf))) > 0) {
    if (len < 0) {
      perror("read");
      exit(-1);
    }
    if ( (len=write (STDOUT_FILENO, buf, len)) < 0) {
      perror("write");
      exit(-1);
    }
  }
  fflush(stdout);
  close(fd);
  return 0;
}
