#include "../inc/ush.h"

void mx_signals_init(sig_t handler) {

    signal (SIGINT, handler);
    signal (SIGQUIT, handler);
    signal (SIGTSTP, handler);
    signal (SIGTTIN, handler);
    signal (SIGTTOU, handler);

}

static void set_shell_vars() {

    if (!getenv(PWD_STR)) {
        char* pwd = getcwd(NULL, 0);
        setenv(PWD_STR, pwd, 1);
        mx_strdel(&pwd);
    }
    if (!getenv(OLDPWD_STR)) {
        char* oldpwd = getcwd(NULL, 0);
        setenv(OLDPWD_STR, oldpwd, 1);
        mx_strdel(&oldpwd);
    }

    int shlvl = mx_atoi(getenv("SHLVL"));
    char* shlvl_str = mx_itoa(shlvl + 1);
    setenv("SHLVL", shlvl_str, 1);
    mx_strdel(&shlvl_str);

    // char* shell_path = realpath("ush", NULL);
    // setenv("SHELL", shell_path, 1);
    // mx_strdel(&shell_path);

}

static void shell_init(t_cmd_utils** utils) {

    int sh_instream = STDIN_FILENO;
    int ush_pgid;
    bool is_tty = isatty(sh_instream);
    if (is_tty) {
        
        while (tcgetpgrp(sh_instream) != (ush_pgid = getpgrp()))
            kill(- ush_pgid, SIGTTIN);

        mx_signals_init(SIG_IGN);

        ush_pgid = getpid();
        if (setpgid(ush_pgid, ush_pgid) < 0) {
            exit(EXIT_FAILURE);
        }

        (*utils)->ush_pgid = ush_pgid;
        tcsetpgrp(sh_instream, ush_pgid);
        tcgetattr(sh_instream, &(*utils)->shell_modes);
    
    }
    (*utils)->processes = (*utils)->stopped_jobs = NULL;
    (*utils)->is_interactive = is_tty;

}

void mx_ush_init(t_cmd_utils** utils) {

    shell_init(utils);

    (*utils)->builtin_exit_code = MX_EXIT_NONBUILTIN;
    (*utils)->args = NULL;
    (*utils)->cmd_line = NULL;
    (*utils)->env_vars = NULL;
    (*utils)->exported_vars = NULL;
    (*utils)->shell_vars = NULL;
    
    set_shell_vars();
    mx_env_reset(utils);
    mx_export_reset(utils);
    
}
