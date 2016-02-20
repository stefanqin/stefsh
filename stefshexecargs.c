/*stefshExecArgs function implementation*/
#include "main.h"

static int stefshExecNonBuiltIns(char * tokens[]);

/*built-in commands*/
static int stefsh_echo(char * tokens[]);
static int stefsh_cd(char * tokens[]);
static int stefsh_exit(char * tokens[]);
static int stefsh_help(char * tokens[]);

const char * builtin_str[] = {
  "echo",
  "cd",
  "exit",
  "help"
};
const int num_builtins = sizeof(builtin_str)/sizeof(char *);

int (*stefshExecBuiltIns[])(char ** builtin_str) = {
  &stefsh_echo,
  &stefsh_cd,
  &stefsh_exit,
  &stefsh_help,
};

/*internal-to-built-in functions*/
static void stripQuotes(char * str);
static void stripBackslash(char * str);

int stefshExecArgs(char * tokens[]){

  int i = 0;

  if(num_tokens == 0){ //no commands entered
    return 0;
  }

  for(i = 0; i < num_tokens; i++){
    //tokens[i] is mutable.
    stripQuotes(tokens[i]);
    stripBackslash(tokens[i]);
  }

  for (i = 0; i < num_builtins; i++){
    if (strcmp(tokens[0],builtin_str[i]) == 0){
      return (*stefshExecBuiltIns[i])(tokens);
    }
  }

  return stefshExecNonBuiltIns(tokens);
}

int stefsh_echo(char * tokens[]){
  int i = 0;

  //num_tokens must be >= 1, since echo is inc.
  for (i = 1; i < num_tokens; i++){
    printf("%s",tokens[i]);
  }
  puts("");

  return 1;
}

int stefsh_cd(char * tokens[]){
  int i = 0;

  //must be of the form cd <dir> [<dir>...]
  if (num_tokens == 1){
      fputs("-stefsh: \"cd\" expects at least one argument.",stderr);
  }

  for(i = 1; i < num_tokens; i++){
    if(chdir(tokens[i]) != 0){
      return 0;
      perror("-stefsh: Directory change error");
    }
  }

  return 1;
}

int stefsh_help(char * tokens[]){
  //detailed and not detailed
  puts("Welcome to stefshv1.0!\nType \"help -d\" for detailed help.");
  puts("Usage: command [<arguments>]");
  puts("Note: Commands are the same as those in bash. "
          "Use \"man\" for more info");
  puts("Built-in commands:");
  puts(" echo [<arg>...]\n cd dir [<dir>...]\n help [-d]");

  return 1;
}

int stefsh_exit(char * tokens[]){
  return 0;
}

/*For non-built ins*/
int stefshExecNonBuiltIns(char ** tokens){

  pid_t process_id, wait_process_id;
  int proc_status = 0;//stores the status of process funcs

  //creates a new process same to the parent process
  if ((process_id = fork()) == 0){
    //child process functions
    if(execvp(tokens[0], tokens) == -1){//v = array, p = search PATH
      perror("-stefsh:Execution error");
    }
    return 0;
  }
  else if (process_id < 0){
    perror("-stefsh: Forking error");
  }
  else{
    //parent process functions - process_id is returned after child process runs
    do{
      wait_process_id = waitpid(process_id,&proc_status, WUNTRACED);
      if (wait_process_id < 0){
        perror("-stefsh: Process signal error");
        return 0;
      }
    } while(!WIFSIGNALED(&proc_status) && !WIFEXITED(&proc_status) && \
      !WIFSTOPPED(&proc_status));//evalutes to false if process has returned
  }

  return 1;
}

static void stripQuotes(char * str){

  /*subj_str existence necessary since str is used to maintiain
  starting position of str memory block. It's simply a copy of &token[i]*/
  char *quote_stripped_str = NULL, *subj_str = NULL;

  for(subj_str = quote_stripped_str = str; *subj_str != '\0';
    subj_str++){
    *quote_stripped_str = *subj_str;

    if(*quote_stripped_str != '\"' && *quote_stripped_str != '\''){
      quote_stripped_str++;
    }
    else{
      /*if position 1, don't check for backslash
      if not position 1, check for backslash*/
      if(quote_stripped_str == str){
        continue;
      }
      else if(*(quote_stripped_str - 1) == '\\'){
        quote_stripped_str++;
      }
    }
  }

  *quote_stripped_str = '\0';
}

static void stripBackslash(char * str){
  char * bl_stripped_str = NULL, * subj_str = NULL;

  for(subj_str = bl_stripped_str = str; *subj_str != '\0';
    subj_str++){
    *bl_stripped_str = *subj_str;
    if(*bl_stripped_str != '\\'){
      bl_stripped_str++;
    }
  }
}
