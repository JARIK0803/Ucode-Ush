#include "../inc/ush.h"

static void fg_not_found_err(const char* arg) {

    mx_printerr("fg: job not found: ");
    mx_printerr(arg);
    mx_printerr("\n");

}

static void fg_no_job_err(const char* arg) {

    mx_printerr("fg: ");
    mx_printerr(arg);
    mx_printerr(": no such job\n");

}

int mx_fg(t_cmd_utils* utils, char** args) {

    char* fg_arg = args[1] ? mx_strdup(args[1]) : mx_strdup("");

    t_process* proc_to_fg = NULL;
    int proc_idx = 0;
    if (mx_strlen(fg_arg) == 2 && mx_isdigit(fg_arg[1])) {

        proc_to_fg = mx_get_process_by_nodeid(utils->stopped_jobs, mx_atoi(fg_arg + 1), &proc_idx);
        if (proc_to_fg == NULL) {
            fg_no_job_err(fg_arg);
            mx_strdel(&fg_arg);
            return 1;
        }

    } else if (mx_strlen(fg_arg) > 0 && fg_arg[0] != '%') {

        proc_to_fg = mx_get_process_by_name(utils->stopped_jobs, fg_arg, &proc_idx);
        if (proc_to_fg == NULL) {
            fg_not_found_err(fg_arg);
            mx_strdel(&fg_arg);
            return 1;
        }

    } else {
        
        proc_to_fg = mx_top_process(utils->stopped_jobs, &proc_idx);
        if (proc_to_fg == NULL) {
            mx_printerr("fg: no current job\n");
            return 1;
        }

    }
    mx_strdel(&fg_arg);

    t_process* original = mx_get_process_by_pid(utils->processes, proc_to_fg->pid, NULL);
    mx_foreground_job(utils, original, 1);
    mx_process_pop_index(&utils->stopped_jobs, proc_idx);
    return 0;

}
