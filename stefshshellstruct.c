/*stefsh main implementation - structure of the shell*/
#include "main.h"
#define BUFFER_SIZE 1024

extern char * stefshReadLine(void);
extern char ** stefshParseLine(char * line);
extern int stefshExecArgs(char ** tokens);

/*Load init files*/
int stefshLoad(void){
  return 1;
}

/*Perform cleanup commands*/
int stefshTerminate(void){
  return 1;
}

void stefshLoop(void){
  int i = 0;

  char * line = NULL;
  char ** tokens = NULL;
  int status = 1;

  while(status){

    //get current working dir
    char cwd[BUFFER_SIZE] = {};
    if(getcwd(cwd,sizeof(cwd)) == NULL){
      perror("-stefsh: couldn't retrieve current directory");
    }

    fprintf(stdout,"stefsh ~%s\n$",cwd);
    if((line = stefshReadLine()) == NULL){
      status = 0;
    }
    if((tokens = stefshParseLine(line)) == NULL){
      status = 0;
    }
    status = stefshExecArgs(tokens);
    puts("");
  }
  free(line);

  for(i = 0; i < num_tokens; i++){
    free(tokens[i]);
  }
  free(tokens);
}
