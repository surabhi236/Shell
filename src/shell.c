#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h> 
#include <pwd.h>

#include "parse.h"
#include "shell.h"
#include "handler.h"

static struct passwd *pw;
static char prompt[SIZE];

char curr_dir[SIZE];
int his_cnt = 0;
bool wrap;
char *history[HIS_SIZE];
char *homedir;

void set_prompt(){
  sprintf(prompt, "\n%s $> ", curr_dir);
}

void init(){
  printf("Welcome to ASP Shell\n");
  pw = getpwuid(getuid());
  homedir = pw->pw_dir;

  chdir(homedir);
  getcwd(curr_dir, SIZE);
  set_prompt();

  his_cnt = 0;
  wrap = false;
}

void execute(char *cmd_line){
  pid_t child_pid;
  char *argv[SIZE];
  int argc, status;

  char *cmd = parse_command(cmd_line, argv, &argc);
  //print_cmd(argc, argv);

  if (strcmp(cmd, "ls") == 0){
    child_pid = fork();
    
    if(child_pid == 0)
      execvp("/bin/ls", argv);
    else
      waitpid(child_pid, &status, 0);
  }
  else if(strcmp(cmd, "cd") == 0){
    chdir(argv[1]);
    getcwd(curr_dir, SIZE);
    set_prompt();
  }
  else if(strcmp(cmd, "rm") == 0){
    child_pid = fork();

    if(child_pid == 0)
      execvp("/bin/rm", argv);
    else
      waitpid(child_pid, &status, 0);
  }
  else if(strcmp(cmd, "history") == 0){
    history_action(cmd, argc, argv);
  }
  else if(strcmp(cmd, "issue") == 0){
    issue(cmd, argc, argv);
  }
  else if(strcmp(cmd, "exit") == 0){
    exit(0);
  } 
  else if(strcmp(cmd, "rmexcept") == 0){
    rmexcept(cmd, argc, argv);
  }
  else{
    default_action(cmd, argc, argv);
  }
  free(cmd_line);
}

int main(){

  init();

  /* Takes input from console, parses it, handles it and repeat. */
  while(1){
    char *cmd_line;

    cmd_line = readline(prompt);
    if (strcmp(cmd_line,"") != 0)
    {
      /* Save command to history. (Ring buffer of size HIS_CNT). */
      char *save_in_his = (char*) malloc(sizeof(char) * strlen(cmd_line));
      strcpy (save_in_his, cmd_line);
      
      history[his_cnt] = save_in_his;
      his_cnt = (his_cnt + 1) % HIS_SIZE;
      
      if(his_cnt == 0)
        wrap = true;
      
      execute(cmd_line);
    }
  }
  return 0;
}
