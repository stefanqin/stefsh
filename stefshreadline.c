/*stefshReadLine function implementation.*/
#include "main.h"

static struct Flags_rl {
  unsigned int bl_rl: 1;
  unsigned int dq_rl: 1;
  unsigned int sq_rl: 1;
} flags_rl;

static int num_input_lines = 0, num_input_chars = 0;

/* Readline Functions*/
static int startNewInputLine_rl(char ** lineptr_add);
static int storeLine_rl (const char ** const lineptr_add, char ** const input_read_add);
static void flagCheckLine_rl(const char * const lineptr);

char * stefshReadLine(void){

  char * lineptr = NULL;
  char * input_read = NULL;
  char * temp_ptr = NULL;

  //refresh with each ReadLine call
  num_input_lines = 0;
  num_input_chars = 0;

  do{
    if(flags_rl.bl_rl|| flags_rl.dq_rl || flags_rl.sq_rl){
      fputs(">",stdout);
    }

    flags_rl.bl_rl = 0;

    if(startNewInputLine_rl(&lineptr) == 0){
      perror("-stefsh: Coudn't start new line");
      return NULL;
    }
    num_input_chars += strlen(lineptr);
    num_input_lines++;

    if (storeLine_rl((const char ** const) &lineptr,
          (char ** const) &input_read) == 0){
      perror("-stefsh: Failed to add line to input");
      return NULL;
    }

    flagCheckLine_rl((const char *) lineptr);
  } while (flags_rl.bl_rl|| flags_rl.dq_rl || flags_rl.sq_rl);

  num_input_chars -= 1;//disregard final newline.

  free(lineptr);

  //delete final newline and append null terminator
  temp_ptr = (char *) realloc(input_read,sizeof(char) * num_input_chars + 1);
  if (temp_ptr == NULL){
    return 0;
  }
  else{
    input_read[num_input_chars] = '\0';
  }
  return input_read;
}

static int startNewInputLine_rl(char ** lineptr_add) {

  //return 0 if unsuccessful
  char * temp_ptr = NULL;

  char curr_char = NULL;
  int num_line_chars = 0;

  *lineptr_add = (char *) malloc(sizeof(char));
  if (*lineptr_add == NULL){
    perror("-stefsh: malloc error");
    return 0;
  }

  //Keep reallocating memory to save space. The alternative is fixed memory.
  while((curr_char = getchar()) != EOF){
    num_line_chars++;
    temp_ptr = (char *) realloc(*lineptr_add,sizeof(char) * num_line_chars);
    if (temp_ptr == NULL){
      return 0;
    }
    else{
      memcpy(*lineptr_add + num_line_chars - 1, &curr_char, sizeof(char));
      if(curr_char == '\n'){
        break;
      }
    }
  }
  temp_ptr = (char *) realloc(*lineptr_add,sizeof(char) * (num_line_chars + 1));
  if (temp_ptr == NULL){
    return 0;
  }
  else{
    (*lineptr_add)[num_line_chars] = '\0';
  }
  return 1;
}

static int storeLine_rl(const char ** const lineptr_add, char ** const input_read_add){

  *input_read_add = (char *) malloc(sizeof(char) * (num_input_chars));
  if(*input_read_add == NULL){
    perror("-stefsh: malloc error");
    return 0;
  }
  memcpy(*input_read_add,*lineptr_add,strlen(*lineptr_add) * sizeof(char));

  return 1;
}

static void flagCheckLine_rl(const char * lineptr){

  int i = 0;
  char char_read = NULL;

  for (i = 0; i < strlen(lineptr); i++){
    char_read = lineptr[i];
    if (char_read == '\\'){
      if (flags_rl.bl_rl == 0){
        flags_rl.bl_rl = 1;
      }
    }
    else if (char_read == '\"'){
        if (flags_rl.bl_rl == 1){
          continue;
        }
        else if (flags_rl.dq_rl == 1){
          flags_rl.dq_rl = 0;
        }
        else if (flags_rl.sq_rl == 1){
          continue; //essentially ignore
        }
        else {
          flags_rl.dq_rl = 1;
        }
      }
    else if (char_read == '\''){
      if (flags_rl.bl_rl == 1){
        continue;
      }
      else if (flags_rl.dq_rl == 1){
        continue;
      }
      else if (flags_rl.sq_rl == 1){
        flags_rl.sq_rl = 0;
      }
      else {
        flags_rl.sq_rl = 1;
      }
    }
  }
}
