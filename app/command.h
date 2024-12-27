#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "tokenizer.h"

extern bool is_running;
extern unsigned short exit_code;

void run_builtin(char *argv[]);

bool is_builtin(char *cmd);

void build_argument_list(Tokenizer *tokenizer, char *argv[]);

char *find_command(char *cmd);

void exec_command(char *argv[]);

void run_command(char *argv[]);

#endif // !__COMMAND_H__
