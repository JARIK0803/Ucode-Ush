#ifndef USH_H
#define USH_H

#include "../libmx/inc/libmx.h"
#include "const.h"
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <stdio.h>

extern char** environ;

typedef struct s_process {
    int status;
    bool stopped;
    bool completed;
    
    pid_t pid;
    int node_id;
    char* path;
    char* cmd_name;
    char* cmd_line;
    struct termios sh_modes;
    struct s_process* next;
}              t_process;

typedef struct s_env_var {
    char* name;
    char* value;
    int index;
    struct s_env_var* next;
}              t_env_var;

typedef struct s_cmd_utils {
    int ush_pgid;
    bool is_interactive;
    int builtin_exit_code;
    char** args;
    char* cmd_line;
    t_env_var* env_vars;
    t_env_var* exported_vars;
    t_env_var* shell_vars;
    t_process* processes;
    t_process* stopped_jobs;
    struct termios shell_modes;
}              t_cmd_utils;

typedef int (*t_cmd_func)(t_cmd_utils* utils, char** args);


// COMMANDS' FLAGS
typedef struct s_cd_flags {
    int s, P, prev;
}              t_cd_flags;

typedef struct s_which_flags {
    int s, a;
}              t_wch_flags;

typedef struct s_env_flags {
    int i, P, u;
    char* u_param;
    char* p_param;
}              t_env_flags;

typedef struct s_echo_flags {
    int n, e, E;
}              t_echo_flags;

typedef struct s_pwd_flags {
    int L, P;
}              t_pwd_flags;

// PARSING

char** mx_read_line();
int mx_parse_line(t_cmd_utils *utils, char *line, char*** args);
void mx_cd_parse_flags(t_cd_flags** flags, char** args, int* arg_idx);
int mx_wch_parse_flags(t_wch_flags** flags, char** args, int* arg_idx);
int mx_env_parse_flags(t_env_flags** flags, char** args, int* arg_idx);
void mx_echo_parse_flags(t_echo_flags** flags, char** args, int *flag_count);
int mx_parse_for_no_flags(char** args, const char* cmd);
int mx_pwd_parse_flags(t_pwd_flags** flags, char** args);
void mx_cd_add_flag(t_cd_flags** flags, char flag);
void mx_wch_add_flag(t_wch_flags** flags, char flag);
void mx_env_add_flag(t_env_flags** flags, char flag);
void mx_echo_add_flag(t_echo_flags** flags, char flag);
void mx_pwd_add_flag(t_pwd_flags **flags, char flag);
char *mx_normalize_path(char *pwd, char *point);
int mx_tilde_expansion(char **args);
int mx_param_expansion(t_cmd_utils* utils, char **args);
void mx_command_substitution(char ***args, t_cmd_utils* utils);
int get_close_extension_brackets_idx(char *str, const char opening, const char closing);

// COMMAND EXECUTION

char* mx_cmd_exec(t_cmd_utils* utils, char** args);
void mx_exec(t_cmd_utils* utils);
int mx_cd(t_cmd_utils* utils, char** args);
int mx_env(t_cmd_utils* utils, char** args);
int mx_echo(t_cmd_utils* utils, char** args);
int mx_which(t_cmd_utils* utils, char** args);
int mx_pwd(t_cmd_utils* utils, char** args);
int mx_export(t_cmd_utils* utils, char** args);
int mx_unset(t_cmd_utils* utils, char** args);
int mx_fg(t_cmd_utils* utils, char** args);
int mx_exit(t_cmd_utils* utils, char** args);
void mx_cleanup(t_cmd_utils* utils, char** args);
int mx_builtin_exec(t_cmd_utils* utils, char** args);

// ERROR HANDLING

void mx_ush_err_handling(int error_code, const char* cmd_name);
void mx_print_odd_quotes_err();
void mx_print_too_many_args_err(const char* cmd);
void mx_print_cmd_err(const char* cmd, const char* error);
void mx_print_flag_err(const char* cmd, char flag);
void mx_print_env_option_err(char flag, const char* cmd);
void mx_print_env_error(const char* error, const char* env_util);
void mx_print_env_arg_err(char flag);

// ENV COMMAND UTILS

void mx_env_reset(t_cmd_utils** utils);
int mx_handle_new_process(t_cmd_utils* utils, char** args, t_env_flags* flags, int* util_arg_idx);
int mx_set_env_vars(t_cmd_utils* utils, char** args, int* arg_idx);
int mx_remove_env_var(t_cmd_utils** utils, char* name);
char** mx_get_env_util_args(char** args, int util_arg_idx);
void mx_exec_env_utility(t_cmd_utils* utils, char** args, int util_arg_idx, t_env_flags* flags);
char** mx_get_exec_paths(const char* to_find, const char* custom_path, bool single_search);

void mx_export_reset(t_cmd_utils** utils);
char* mx_get_var_str(t_env_var* env_var);

t_env_var *mx_create_env_var(char* env_var);
void mx_env_dup_push_back(t_env_var** list, t_env_var* var);
void mx_add_shell_var(t_cmd_utils* utils, char* var_str);
void mx_env_push_back(t_env_var **list, char* env_var);
void mx_env_pop_index(t_env_var **list, int index);
void mx_env_clear_list(t_env_var **list);
int mx_env_list_size(t_env_var* list);
void mx_print_env_list(t_env_var* list, bool is_env_cmd);
void mx_env_sort_list(t_env_var** list, bool (*cmp)(char* a, char* b));
t_env_var* mx_find_env_var(t_env_var* list, char* name, int* index);
void mx_overwrite_env_var(t_env_var** env_var, const char* value);
char* mx_get_var_name(const char* var);
char* mx_get_var_value(const char* var);
char** mx_get_env_array(t_env_var* list);


// MISC UTILS

char* mx_get_dir_path(char* dir, const char* file_name);
int mx_util_arg_count(t_cmd_utils* utils);
char *mx_replace_substr_free(char *str, char *sub, char *replace);
void mx_ush_init(t_cmd_utils** utils);
bool mx_is_builtin_cmd(const char* cmd);


// PROCESS CONTROL

t_process *mx_create_process(char** args, const char* path);
void mx_dfl_push_back(t_process** list, t_process* new_node);
void mx_process_push_back(t_process **list, t_cmd_utils* utils, const char* path);
void mx_created_process_push_back(t_process **list, t_process* p);
void mx_process_pop_front(t_process **head);
void mx_process_pop_back(t_process **head);
void mx_process_pop_index(t_process **head, int index);
void mx_clear_process_list(t_process **list);
int mx_process_list_size(t_process* list);
t_process* mx_get_process_by_pid(t_process* list, pid_t pid, int* index);
void mx_print_process_list(t_process* list);
t_process* mx_top_process(t_process* list, int* index);
t_process* mx_get_process_by_name(t_process* list, const char* name, int* index);
t_process* mx_get_process_by_nodeid(t_process* list, int node_id, int* index);
void mx_foreground_job(t_cmd_utils* utils, t_process* p, bool to_continue);
void mx_background_job(t_process* p, bool to_continue);
void mx_wait_for_job(t_cmd_utils* utils, t_process* p);
void mx_signals_init(sig_t handler);
int mx_get_last_exit_code(t_cmd_utils* utils);

// Array of function pointers for commands
static const t_cmd_func builtin_funcs[] = {
    &mx_cd, &mx_env, &mx_echo, &mx_pwd, &mx_which, 
    &mx_export, &mx_unset, &mx_fg, &mx_exit, NULL 
};

#endif
