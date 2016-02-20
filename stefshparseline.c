/*stefshparseline function implementation*/
#include "main.h"

#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>
/*reasonable constants that will never overflow.
We're not concerned about space complexity as negligible for this instance.*/
#define MAX_TOKENS 5
#define MAX_TOKEN_LEN 50
#define ERR_BUFFER_LEN 256

/*this regex doesn't capture escaped string literals with no meaning,
nor if:
-command includes escaped whitespace,
-a command begins with an escaped quotation.
-command includes two types of quotes and one is escaped
*/
static const PCRE2_SPTR pattern_pl = (PCRE2_SPTR)\
  "(?:[^\"\'\\s]+)|" //string literals not encased by quotes at beginning
  "([\"\'])(?:(?!\\1)[^\\\\]|\\\\\\1)*?\\1|" //encased by quotes
  "(?:[^\"\'\\s]+)"; //string literals not encased by quotes at end

int num_tokens = 0;

char ** stefshParseLine(char * line_to_parse){

  char ** tokens = NULL;
  int i = 0;

  int tokens_count = 0; //Offset by 1.

  /*pcre matching variables*/
  pcre2_match_data * match_data;
  int match_result_num = 0;
  const char * subject = (const char * )line_to_parse;
  int subject_length = strlen((const char *)subject);

  PCRE2_SIZE * ovector;

  int start_pos = 0;
  int end_pos = 0;

  pcre2_code * regexp;

  int err_num = 0;
  PCRE2_SIZE err_pos;
  PCRE2_UCHAR err_buffer[ERR_BUFFER_LEN];

  //mem allocation for multi-dimensional array
  if((tokens = malloc(MAX_TOKENS * sizeof(char *))) == NULL){
    perror("-stefsh: malloc error");
    return NULL;
  };
  for (i = 0; i < MAX_TOKENS; i++){
    if((tokens[i] = malloc(MAX_TOKEN_LEN * sizeof(char *))) == NULL){
      perror("-stefsh: malloc error");
      return NULL;
    }
  }

  /*Compiling*/
  regexp = pcre2_compile(pattern_pl,PCRE2_ZERO_TERMINATED,0,&err_num,&err_pos,
    NULL);

  if(regexp == NULL){
    //pcre2 does not support perror.
    pcre2_get_error_message(err_num,err_buffer,sizeof(err_buffer));//check if need (PCRE2_SIZE sizeof(buffer))
    fprintf(stderr,"-stefsh: PCRE compilation failed at pos = %d: %s\n",
      (int)err_pos,(char *) err_buffer);
    return NULL;
  }

  /*Matching*/
  match_data = pcre2_match_data_create_from_pattern(regexp,NULL);

  match_result_num = pcre2_match(regexp,subject,subject_length,0,
    0,match_data,NULL);

  //first time matching, if there's no match then there is an error.
  if(match_result_num < 0){
      pcre2_get_error_message(match_result_num,err_buffer,
        sizeof(err_buffer));
      fprintf(stderr,"-stefsh: PCRE matching error %d: %s",match_result_num,
        (char*) err_buffer);
      if(match_result_num == PCRE2_ERROR_NOMATCH){
        fputs("; First match should always match.\n",stderr);
      }
      else{
        fputs("\n",stderr);
      }
      pcre2_match_data_free(match_data);
      pcre2_code_free(regexp);
      return NULL; //check if ovector exists after match failure.
  }

  ovector = pcre2_get_ovector_pointer(match_data);

  start_pos = ovector[0];
  end_pos = ovector[1];

  //store in array
  strncpy(tokens[0], &subject[start_pos],
    end_pos - start_pos);
  tokens[0][end_pos] = '\0';

  //iterative matching for 2 or more commands
  while(match_result_num != PCRE2_ERROR_NOMATCH){

    //refresh match data
    //match_data = pcre2_match_data_create_from_pattern(regexp,NULL);

    //start from the end character of the last match
    match_result_num = pcre2_match(regexp,subject,subject_length,end_pos,
    0,match_data,NULL);

    if(match_result_num < 0){
      if(match_result_num == PCRE2_ERROR_NOMATCH){
        break;
      }
      else{ //critical matching error.
        pcre2_get_error_message(match_result_num,err_buffer,
          sizeof(err_buffer));
        fprintf(stderr,"-stefsh: PCRE matching error %d: %s\n",match_result_num,
        (char*) err_buffer);
        pcre2_match_data_free(match_data);
        pcre2_code_free(regexp);
        return NULL;
        }
    }

    ovector = pcre2_get_ovector_pointer(match_data);

    //check if this match is adjacent to prev match. If so, concatenate.
    if (ovector[0] == end_pos){
      strncpy(tokens[tokens_count], &subject[start_pos],
        ovector[1] - start_pos);
      tokens[tokens_count][ovector[1]] = '\0';

      end_pos = ovector[1];
    }
    else{
      tokens_count++;

      start_pos = ovector[0];
      end_pos = ovector[1];

      strncpy(tokens[tokens_count], &subject[start_pos],
        end_pos - start_pos);
      tokens[0][end_pos] = '\0';
    }
  }

  pcre2_match_data_free(match_data);
  pcre2_code_free(regexp);

  num_tokens = tokens_count + 1;

  tokens[num_tokens] = NULL;//null terminate list of tokens
  return tokens;
}
