#include "../inc/ush.h"

int main() {

    t_cmd_utils* utils = malloc(sizeof(*utils));
    mx_ush_init(&utils);

    while (1) {

        if (isatty(0))
            mx_printstr("u$h> ");
            
        char** lines = mx_read_line();
        for (int i = 0; lines[i] != NULL; ++i) {

            mx_strdel(&utils->cmd_line);
            utils->cmd_line = mx_strdup(lines[i]);

            if (mx_parse_line(utils, lines[i], &utils->args) != 0) 
                continue;

            if (utils->args[0] && mx_strcmp(utils->args[0], "") != 0) {

                mx_exec(utils);

            }            

        }
        mx_del_strarr(&lines);

    }

}
