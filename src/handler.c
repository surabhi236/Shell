#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h> 

#include <fcntl.h>

#include "handler.h"
#include "shell.h"
#include "parse.h"

void print_history(long int cnt){
  for(long int i = cnt; i>=1; i--){
    long int addr = (his_cnt - i) % HIS_SIZE;
    addr = addr<0 ? addr + HIS_SIZE : addr;

    printf("%ld %s\n", cnt - i + 1, history[addr]);
  }
}

/* Handles the history and history n commands. 
   Prints the most recent n commands issued by the numbers. 
   If n is omitted, prints all commands issued by the user. */
void history_action(char *cmd, int argc, char *argv[]){
  if(argv[1] != NULL){
    long int n = strtol(argv[1], NULL, 10);

    if(wrap)
      n = n<HIS_SIZE?n:HIS_SIZE;
    else
      n = n<his_cnt?n:his_cnt;

    print_history(n);
  }
  else{
    if(wrap)
      print_history(HIS_SIZE);
    else
      print_history(his_cnt);
  }
}

/* Handles the issue n command. */
void issue(char *cmd, int argc, char *argv[]){
  if(argv[1] != NULL){
    long int n = strtol(argv[1], NULL, 10);

    if(wrap)
      n = n<HIS_SIZE?n:HIS_SIZE-1;
    else
      n = n<his_cnt?n:his_cnt-1;

    long int addr = (his_cnt - n - 1) % HIS_SIZE;
    addr = addr<0 ? addr + HIS_SIZE : addr;

    char *cmd_to_run = (char*) malloc(sizeof(char) * strlen(history[addr]));
    strcpy (cmd_to_run, history[addr]);

    execute(cmd_to_run);
  }
}

/* Implements rmexcept. 
   Approach we move the exception files to a tmp_asp directory, then 
   recursively remove the whole directory and then restore the exception files.*/
void rmexcept(char *cmd, int argc, char *argv[]){
  pid_t child_pid;
  int status;
  
  char str[256];
  sprintf(str,"%s/tmp_asp", homedir);
  mkdir(str,0777);
  int i = 1;
  while(argv[i]!=NULL)
  {
    sprintf(str,"%s/tmp_asp/%s",homedir,argv[i]);
    //printf("\n%s %s\n",str,argv[i]);
    rename(argv[i],str);
    i++;
  }

  child_pid = fork();
    
  if(child_pid == 0)
  { 
    execlp("/bin/rm", "rm", "-r", curr_dir, NULL);
    exit(0);
  }
  else
  {
    waitpid(child_pid, &status, 0);
    sprintf(str, "%s/tmp_asp", homedir);
    rename(str, curr_dir);
    chdir(curr_dir);
  }
}

/* Implements <program_name> and <program_name> n (time in secs).
   This also supports I/O redirection. */
void default_action(char *cmd, int argc, char *argv[]){
  pid_t child_pid;
  int status;
  
  FILE *fp = NULL;
  fp = fopen(cmd, "rb");

  if(fp != NULL){
    fclose(fp);

    child_pid = fork();

    if(child_pid == 0) //Child
    {
      int o_red_index = -1, i_red_index = -1;
      get_redirection(argv, &o_red_index, &i_red_index);
        
      int ofd = -1;
      int ifd = -1;

      /* Set input and output files if redirection present. */
      if(o_red_index != -1)
      {
        ofd = open(argv[o_red_index],
                   O_CREAT|O_WRONLY|O_TRUNC, S_IRUSR | S_IWUSR);
        if(ofd < 0)
        {
          printf("Redirection to %s failed",argv[o_red_index]);
          exit(-1);
        }
        dup2(ofd,STDOUT_FILENO);
        close(ofd);
        argv[o_red_index - 1] = '\0';
        argv[o_red_index] = '\0';
      }
      if(i_red_index != -1)
      {
        ifd = open(argv[i_red_index], O_RDONLY);
        if( ifd < 0 )
        {
          printf("Redirection from %s failed",argv[o_red_index]);
          exit(-1);  
        }
        dup2(ifd,STDIN_FILENO);
        close(ifd);
                
        argv[i_red_index - 1] = '\0';
        argv[i_red_index] = '\0';
      }    

      int err = execv(cmd, argv);
      printf("Error occured in execution: %d",err);
      exit(err);
    }
    else{ //Parent
      /* When a second argument (integer type) of time is given. */
      if(argc > 1 && argv[1] != '\0' && isdigit(argv[1][0])){
        int time_to_run = atoi(argv[1]);
        sleep(time_to_run);
        int res = waitpid (child_pid, &status, WNOHANG);
        if(res == 0)
          kill(child_pid, SIGTERM);
        waitpid(child_pid, &status, 0);
      }
      else
        waitpid(child_pid, &status, 0);
    }
  }
  else{
    printf("\n%s: command not found", cmd);
  }
}
