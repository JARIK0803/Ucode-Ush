#include "../inc/ush.h"

static int set_process_status(t_cmd_utils* utils, pid_t pid, int status) {

    if (pid > 0) {

        for (t_process* p = utils->processes; p; p = p->next) {

            if (p->pid == pid) {
                
                p->status = status;
                if (WIFSTOPPED (status)) {
                    p->stopped = true;
                    mx_created_process_push_back(&utils->stopped_jobs, p);
                    mx_printstr("\nush: suspended  ");
                    mx_printstr(p->cmd_line);
                    mx_printstr("\n");
                } else {
                    p->stopped = false;
                    p->completed = true;
                }
                return 0;
            }

        }
          
        return -1;
    } else if (pid == 0 || errno == ECHILD) {
        return -1;
    } 
    else {
        mx_print_cmd_err("waitpid", strerror(errno));
        mx_printerr("\n");
        return -1;
    }

}

void mx_wait_for_job(t_cmd_utils* utils, t_process* p) {

    int status;
    waitpid(p->pid, &status, WUNTRACED);
    while (!set_process_status(utils, p->pid, status) && !p->stopped && !p->completed) {
    
        waitpid(p->pid, &status, WUNTRACED);
        
    }
    utils->builtin_exit_code = MX_EXIT_NONBUILTIN;

}

void mx_foreground_job(t_cmd_utils* utils, t_process* p, bool to_continue) {

    if (tcsetpgrp(0, p->pid) == -1) {
        mx_print_cmd_err("tcsetpgrp", strerror(errno));
        mx_printerr("\n");
        exit(EXIT_FAILURE);
    }

    if (to_continue) {

        mx_printstr("[");
        mx_printint(p->node_id);
        mx_printstr("] - continued  ");
        mx_printstr(p->cmd_line);
        if (kill(- p->pid, SIGCONT) < 0) {
            mx_print_cmd_err("kill (SIGCONT)", strerror(errno));
            exit(EXIT_FAILURE);
        }

    }

    mx_wait_for_job(utils, p);
    if (tcsetpgrp (0, utils->ush_pgid) == -1) {
        mx_print_cmd_err("tcsetpgrp", strerror(errno));
        mx_printerr("\n");
        exit(EXIT_FAILURE);
    }

    if (tcsetattr (0, TCSADRAIN, &utils->shell_modes) == -1) {
        mx_print_cmd_err("tcsetattr", strerror(errno));
        mx_printerr("\n");
        exit(EXIT_FAILURE);
    }

}
