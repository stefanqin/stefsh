#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/*Shell functions*/
int stefshLoad(void);
void stefshLoop(void);
int stefshTerminate(void);

/*stefshLoop functions*/
char * stefshReadLine(void);
char ** stefshParseLine(char * line_to_parse);
int stefshExecArgs(char * tokens[]);

extern int num_tokens; // req since multi-dimensional arrays can't find size
