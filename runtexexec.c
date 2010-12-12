#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>



int main(int argc, char *argv[]) {
#ifdef SCHNEE
  const char *texexec="/opt/context/2004.6.30/bin/texexec";
#else 
  const char *texmfstart="/opt/context/current/bin/texmfstart";
#endif
  int ret;
  int fd,len;
  char *cptr;
  pid_t  pid;
  char dirname[1024];
  char buf[1024];
  char *progname;
  struct rlimit limit;

  int maxfile=10*1024*1024;
  int maxtime=30; /* seconds - don't exceed this limit - perhaps an infinit loop? */
  
  if ( setpriority(PRIO_PROCESS, 0, 10) < 0) {
    fprintf (stderr,"%s: setpriority error %s, exiting\n",progname,strerror(errno));
    exit (-1);
  }

  limit.rlim_cur=maxtime;
  limit.rlim_max=maxtime;
  if ( (ret = setrlimit(RLIMIT_CPU, &limit)) < 0) {
    fprintf (stderr,"%s: setrlimit (CPU) error %s, exiting\n",progname,strerror(errno\
									  ));
    exit(-1);
  }

  limit.rlim_cur=maxfile;
  limit.rlim_max=maxfile;
  if ( (ret = setrlimit(RLIMIT_FSIZE, &limit)) < 0) {
    fprintf (stderr,"%s: setrlimit (FSIZE) error %s, exiting\n",progname,strerror(errno));
    exit(-1);
  }
  
  
  /* progname=argv[0]; */
#ifdef SCHNEE
  ret = 0;
#else
  ret  = putenv("PATH=/opt/context/current/bin:/opt/texlive2008/bin/x86_64-linux:/opt/local/bin:/usr/bin:/bin");
  /* 
     I (Mojca) replaced TEXMFCONTEXT because I had problems setting up the proper variable in the proper 
     texmf.cnf file. I also reshuffled things in PATH, since otherwise texexec from bin took precedence
     over th latest texexec.

     ret |= putenv("TEXMFCONTEXT=/opt/context/latest/texmf");
   */
  ret |= putenv("TEXMFHOME=/opt/context/current/texmf");
#endif
  if (ret) {
    fprintf (stderr,"%s: putenv error %s, exiting\n",progname,strerror(errno));
    exit(-1);
  }
  
  if (argc != 2) {
    fprintf(stderr,"argc=%d, usage: %s <id>\n",argc,progname);
    exit (-1);
  }

  if (strlen(argv[1]) <3) {
    fprintf (stderr,"%s: strange id, exiting\n",progname);
    exit(-1);
  }

  cptr=strstr(argv[1],"/");
  if (cptr) {
    fprintf (stderr,"%s: string has / in it, exiting\n",progname);
    exit(-1);
  }

  strcpy(dirname,"/tmp/texweb/");
  strncat(dirname,argv[1],(sizeof(dirname) - strlen(dirname)));
/*   strcat(dirname,"/output");  */

  /*   fprintf(stderr,"directory to use is: %s\n",argv[1]); */
  setbuf(stdout, NULL);

  ret=chdir(dirname);
  if (ret) {
    fprintf(stderr,"%s: chdir error: %s\n",progname, strerror(errno));
    exit (-1);
  }

  if (! getcwd(buf,sizeof(buf)) ) {
    fprintf(stderr,"%s: getcwd error: %s\n",progname, strerror(errno));
    exit(-1);
  }
/*   fprintf(stderr,"cwd=%s\n",buf); */
    
  
  if ( (fd=open("output",O_WRONLY|O_CREAT, 0666)) < 0) {
    fprintf(stderr,"%s: open error: %s\n",progname, strerror(errno));
    exit (-1);
  }
  
  if ( (pid = fork()) < 0) {
    fprintf(stderr,"%s: fork error: %s\n",progname, strerror(errno));
    exit (-1);
  } else if (pid==0) {
    /* child */

    dup2(fd,STDOUT_FILENO);
    /* dup2(fd,STDERR_FILENO); */
    //ret=execl ("/opt/ruby/1.8/bin/ruby", "ruby", texmfstart, "texexec", "--xtx", "texweb.tex",NULL);
    ret=execl ("/usr/bin/ruby", "ruby", texmfstart, "texexec", "texweb.tex",NULL);
    if (ret) {
      fprintf(stderr,"%s: execl error: %s\n",progname, strerror(errno));
      exit (-1);
    }
  }
  
  if (wait(&ret) <0) {
    perror("wait");
    exit (-1);
  }

  close(fd);

  if ( (fd=open("result",O_WRONLY|O_CREAT, 0666)) < 0) {
    fprintf(stderr,"%s: open error: %s\n",progname, strerror(errno));
    exit (-1);
  }

  sprintf(buf,"%d\n",WEXITSTATUS(ret));
  
  len=write(fd,buf,sizeof(buf));
  if (len==-1) {
    perror("write");
    exit(-1);
  }
  close (fd);
  return WEXITSTATUS(ret);
}
