#ifndef SHELL
#define SHELL

#include <stdbool.h>

#define HIS_SIZE 256
#define SIZE 256

extern char curr_dir[SIZE];
extern int his_cnt;
extern bool wrap;
extern char *history[HIS_SIZE];
extern char *homedir;

extern char* readline(const char*);
void execute(char *);

#endif
