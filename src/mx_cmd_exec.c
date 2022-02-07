#include "../inc/ush.h"

static char* get_cmd_output(int in_stream) {

    char buffer[BUFFER_LEN];
    int i = 0;
    char ch ;
    while (read(in_stream, &ch, 1) > 0 && i < BUFFER_LEN - 1) {
        buffer[i++] = ch;
    } 
    buffer[i] = '\0';
    return mx_strndup(buffer, i - 1);

}

char* mx_cmd_exec(t_cmd_utils* utils, char** args) {

    int my_pipe[2];
    char* result = NULL;

    if (pipe(my_pipe) < 0) {
        mx_print_cmd_err("pipe", strerror(errno));
        mx_printerr("\n");
        exit(EXIT_FAILURE);
    }

    t_process* process = mx_create_process(args, NULL);
    process->cmd_line = mx_strdup(utils->cmd_line);
    mx_dfl_push_back(&utils->processes, process);

    pid_t pid = fork();
    if (pid < 0) {
        mx_print_cmd_err("fork", strerror(errno));
        mx_printerr("\n");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) {

        close(my_pipe[0]);
        dup2(my_pipe[1], STDOUT_FILENO);
        
        if (utils->is_interactive) {
            
            pid_t cpid = getpid();
            setpgid(cpid, cpid);
            tcsetpgrp (0, cpid);
            mx_signals_init(SIG_DFL);
        
        }
        char** env_var_array = mx_get_env_array(utils->exported_vars);

        if (mx_builtin_exec(utils, args) == -1) {
            
            if (execve(process->path, args, env_var_array) < 0) {
                
                mx_ush_err_handling(errno, args[0]);
                
            }
            mx_del_strarr(&env_var_array);
        
        }
        close(my_pipe[1]);

    } else {

        close(my_pipe[1]);

        process->pid = pid;
        if (utils->is_interactive) {
            setpgid(pid, pid);
        }
        if (!utils->is_interactive)
            mx_wait_for_job(utils, process);
        else
            mx_foreground_job(utils, process, 0);

        result = get_cmd_output(my_pipe[0]);
    
        close(my_pipe[0]);
        
    }
    return result;

}
