#include "../inc/ush.h"

// Get arguments for the utility to be executed
char** mx_get_env_util_args(char** args, int util_arg_idx) {

    int arg_count = 1;
    for (int i = util_arg_idx; args[i] != NULL; ++arg_count, ++i);

    char** util_args = malloc(sizeof(char*) * (arg_count + 1));
    util_args[0] = mx_strdup(args[util_arg_idx - 1]);
    for (int i = 1, j = util_arg_idx; i < arg_count; ++i, ++j) {

        util_args[i] = mx_strdup(args[j]);

    }
    util_args[arg_count] = NULL;
    return util_args;

}

int mx_handle_new_process(t_cmd_utils* utils, char** args, t_env_flags* flags, int* util_arg_idx) {

    if (flags->u) {
        if (mx_remove_env_var(&utils, flags->u_param) != 0)
            return 1;
    }
    if (flags->i)
        mx_env_clear_list(&utils->env_vars);

    if (mx_set_env_vars(utils, args, util_arg_idx) != 0)
        return 1;
    
    if (args[*util_arg_idx] == NULL)
        mx_print_env_list(utils->env_vars, true);
    
    return 0;

}

// Execute the env's utility
void mx_exec_env_utility(t_cmd_utils* utils, char** args, int util_arg_idx, t_env_flags* flags) {

    char* custom_path = flags->P ? mx_strdup(flags->p_param) : NULL;
    t_process* chld_process = mx_create_process(args, custom_path);
    chld_process->cmd_line = mx_strdup(utils->cmd_line);
    mx_dfl_push_back(&utils->processes, chld_process);
    
    pid_t pid = fork();

    if (pid == -1) {
        mx_print_cmd_err("fork", strerror(errno));
        mx_printerr("\n");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) {
        
        if (utils->is_interactive) {
            pid_t cpid = getpid();
            setpgid(cpid, cpid);
            tcsetpgrp (0, cpid);
            mx_signals_init(SIG_DFL);
        }

        if (mx_handle_new_process(utils, args, flags, &util_arg_idx) != 0)
            exit(EXIT_FAILURE);

        char** env_vars = mx_get_env_array(utils->env_vars);
        mx_env_reset(&utils);
        
        if (args[util_arg_idx] == NULL) {
            mx_del_strarr(&env_vars);
            exit(EXIT_SUCCESS);
        }
        
        char* env_util = mx_strdup(args[util_arg_idx++]);
        char** util_args = mx_get_env_util_args(args, util_arg_idx);
        char** paths = mx_get_exec_paths(env_util, custom_path, true);
        if (execve(paths[0] ? paths[0] : env_util, util_args, env_vars) == -1) {

            int curr_errno = errno;
            if (curr_errno != ENOENT && curr_errno != EACCES) {

                mx_print_env_error(strerror(curr_errno), env_util);
                exit(EXIT_FAILURE);

            } else if (mx_strchr(env_util, '/') && curr_errno == EACCES) {
                
                mx_print_env_error(strerror(curr_errno), env_util);
                exit(MX_EXIT_EACCES);

            } else {

                mx_printerr("env: ");
                mx_printerr(env_util);
                mx_printerr(": No such file or directory\n");
                exit(MX_EXIT_ENOENT);

            }

        }
        mx_strdel(&env_util);
        mx_del_strarr(&env_vars);
        mx_del_strarr(&util_args);

    } else {

        chld_process->pid = pid;
        if (utils->is_interactive) {
            setpgid(pid, pid);
        }
    
    }

    if (!utils->is_interactive)
        mx_wait_for_job(utils, chld_process);
    else
        mx_foreground_job(utils, chld_process, 0);

}

// Get all possible (or just one of) paths to a binary file
char** mx_get_exec_paths(const char* to_find, const char* custom_path, bool single_search) {

    int path_count = 2;
    char** paths = malloc(sizeof(*paths) * path_count);
    paths[0] = paths[1] = NULL;

    DIR* curr_dir = NULL;
    struct dirent* dp = NULL; 

    const char* path_to_search_in = custom_path ? custom_path : getenv(PATH_STR);
    char** dirs = mx_strsplit(path_to_search_in, ':');
    int is_found = 0;
    for (int i = 0; !is_found && dirs[i] != NULL; ++i) {

        if ((curr_dir = opendir(dirs[i])) == NULL) 
            continue;

        while ((dp = readdir(curr_dir)) != NULL) {

            if (mx_strcmp(to_find, dp->d_name) == 0) {

                if (!single_search) {

                    if (paths[path_count - 2] != NULL) {
                        paths = mx_realloc(paths, sizeof(*paths) * (++path_count));
                    }
                    paths[path_count - 2] = mx_get_dir_path(dirs[i], to_find);
                    continue;
                
                }
                paths[0] = mx_get_dir_path(dirs[i], to_find);
                is_found = 1;
                break;

            }

        }
        closedir(curr_dir);

    }
    mx_del_strarr(&dirs);
    return paths;

}

void mx_env_reset(t_cmd_utils** utils) {

    mx_env_clear_list(&(*utils)->env_vars);
    for (int i = 0; environ && environ[i] != NULL; ++i) {

        // remove the LS_COLORS check later
        if (mx_strstr(environ[i], "LS_COLORS") != NULL)
            continue;

        mx_env_push_back(&(*utils)->env_vars, environ[i]);

    }

}
