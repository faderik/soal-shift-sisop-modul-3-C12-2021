#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
  int status;

  int fd1[2]; // Used to store two ends of first pipe 
  int fd2[2]; // Used to store two ends of second pipe 
  pid_t p;

if (pipe(fd1)==-1) { 
	fprintf(stderr, "Pipe Failed" ); 
	return 1; 
	} 
	if (pipe(fd2)==-1) { 
   	 fprintf(stderr, "Pipe Failed" ); 
	 return 1; 
	} 

  p = fork();

  // buat ngefork (ps aux)
  if (p < 0) {
    exit(EXIT_FAILURE);
  }

  else if (p == 0) {

    dup2(fd1[1], STDOUT_FILENO);
    close(fd1[0]);
    close(fd1[1]);

    char *argv[] = {"ps", "aux", NULL};
    execv("/bin/ps", argv);
  } 

    else { //parent

    p = fork();
    if (p < 0) 
      exit(EXIT_FAILURE);
    
    if (p == 0) {
      dup2(fd1[0], STDIN_FILENO);
      dup2(fd2[1], STDOUT_FILENO);
	
      close(fd1[0]);
      close(fd1[1]);
      close(fd2[0]);
      close(fd2[1]);

      char *argv[] = {"sort", "-nrk", "3,3", NULL};
      execv("/usr/bin/sort", argv);
    } 
	
      else {

      dup2(fp2[0], STDIN_FILENO);

      close(fd1[0]);
      close(fd1[1]);
      close(fd2[0]);
      close(fd2[1]);

      char *argv[] = {"head", "-5", NULL};
      execv("/usr/bin/head", argv);
    }
  
}

