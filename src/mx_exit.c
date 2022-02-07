#include "../inc/ush.h"

void mx_cleanup(t_cmd_utils* utils, char** args) {

    mx_env_clear_list(&utils->env_vars);
    mx_env_clear_list(&utils->exported_vars);
    mx_env_clear_list(&utils->shell_vars);
    mx_clear_process_list(&utils->processes);
    
    char** args_to_clean = args ? args : utils->args;
    if (args_to_clean)
        mx_del_strarr(&args_to_clean);

    mx_strdel(&utils->cmd_line);

}

static int handle_custom_exit(t_cmd_utils* utils, char** args, const char* exit_arg) {

    if (mx_strlen(exit_arg) < 3 && (mx_isdigit(exit_arg[0]) || mx_isdigit(exit_arg[1]))) {
        int exit_code = mx_atoi(exit_arg);
        mx_cleanup(utils, args);
        exit(exit_code);
    } else {
        mx_cleanup(utils, args);
        exit(EXIT_SUCCESS);
    }

}

int mx_get_last_exit_code(t_cmd_utils* utils) {

    int exit_code = 0;
    int dfl_exit_code = utils->builtin_exit_code;
    bool builtin_exited = dfl_exit_code != MX_EXIT_NONBUILTIN;
    t_process* p = mx_top_process(utils->processes, NULL);
    
    if (p && !builtin_exited) {

        exit_code = WEXITSTATUS(p->status);
    
    } else if (builtin_exited) {
        
        exit_code = builtin_exited ? dfl_exit_code : EXIT_SUCCESS;
    
    }
    return exit_code;

}

int mx_exit(t_cmd_utils* utils, char** args) {

    static int warning = 0;
    if (utils->stopped_jobs != NULL && warning != 1) {
        mx_printerr("ush: you have suspended jobs.\n");
        warning = 1;
        return 0;
    }

    unsigned char exit_code = EXIT_SUCCESS;
    if (utils->args[1]) {
        handle_custom_exit(utils, args, utils->args[1]);
    }

    exit_code = mx_get_last_exit_code(utils);

    mx_cleanup(utils, args);
    exit(exit_code);
    
}
