#ifndef CONST_H
#define CONST_H

#define PATH_MAX 4096
#define BUFFER_LEN 10000

// EXIT STATUS CODES

#define MX_EXIT_ENOENT      127
#define MX_EXIT_EACCES      126
#define MX_EXIT_NONBUILTIN  -3

// KEYBOARD 

#define MX_CTRL_C      3
#define MX_CTRL_D      4
#define MX_TAB         9
#define MX_ENTER       10
#define MX_BACKSPACE   127

static const char* builtin_cmds[] = {
    "cd", "env", "echo", "pwd", "which", 
    "export", "unset", "fg", "exit", NULL
};

static const char* WHICH_STR = "which";

static const char* OLDPWD_STR = "OLDPWD";
static const char* PWD_STR = "PWD";
static const char* HOME_STR = "HOME";
static const char* PATH_STR = "PATH";

static const char* USH_STR = "ush: ";

static const char* ENV_USAGE = "usage: env [-i] [-P utilpath] [-u name]\n\t   [name=value ...] [utility [argument ...]]\n";

#endif
