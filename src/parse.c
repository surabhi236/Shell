#include <string.h>
#include <stdio.h>

/* Tokenizes the command given at the shell. */
char* parse_command(char *args, char *argv[], int *argc){
  char *token, *save_ptr;
  *argc = 0;
  for (token = strtok_r (args, " ", &save_ptr); token != NULL;
       token = strtok_r (NULL, " ", &save_ptr))
  {
    argv[*argc] = token;
    (*argc)++;
  }
  argv[*argc] = NULL;
  return argv[0];
}

/* Parses command line args for '<' and '>' and
   evaluates STDIN and STDOUT accordingly. */
void get_redirection(char *argv[], int *o_red_index, int *i_red_index){

  int i = 0;
  while(argv[i] != NULL){
    if( strcmp(">",argv[i]) == 0)
      *o_red_index = i+1;
    else if(strcmp("<",argv[i]) == 0)
      *i_red_index = i+1;
    i++;
  }
}

/* For debugging of parse_command output. */
void print_cmd(int argc, char *argv[]){
  int i;
  printf("\nargc: %d", argc);
  for(i = 0; i<argc; i++){
    printf("\nargv[%d]: %s", i, argv[i]);
  }
}
