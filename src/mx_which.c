#include "../inc/ush.h"

void mx_wch_add_flag(t_wch_flags** flags, char flag) {

    switch (flag) { 

    case 's':
        (*flags)->s = 1; break;

    case 'a':
        (*flags)->a = 1; break;
    
    default: break;
    
    }

}

char* mx_get_dir_path(char* dir, const char* file_name) {

    char* path = mx_strnew(mx_strlen(dir) + mx_strlen(file_name) + 1);
    mx_strcat(path, dir);
    mx_strcat(path, "/");
    mx_strcat(path, file_name);
    return path;

}

bool mx_is_builtin_cmd(const char* cmd) {

    for (int i = 0; builtin_cmds[i] != NULL; ++i) {

        if (mx_strcmp(cmd, builtin_cmds[i]) == 0) {
            return true;
        }

    }
    return false;

}

int mx_which(t_cmd_utils* utils, char** args) {

    t_wch_flags* flags = malloc(sizeof(*flags));
    int arg_idx = 1;
    if (mx_wch_parse_flags(&flags, args ? args : utils->args, &arg_idx) != 0)
        return 1;

    for (int i = arg_idx; args[i] != NULL; ++i) {

        char* to_find = args[i];
        if (mx_is_builtin_cmd(to_find)) {
            mx_printerr(to_find);
            mx_printerr(": shell built-in command\n");
            if (!flags->a) {
                free(flags);
                return 0;
            }
        }

        char** paths = mx_get_exec_paths(to_find, NULL, !flags->a);

        if (paths[0] == NULL) {
            mx_printerr(to_find);
            mx_printerr(" not found\n");
            mx_del_strarr(&paths);
            return 1;
        } else {
            mx_print_strarr(paths, "\n");
        }

        mx_del_strarr(&paths);

    }

    free(flags);

    return 0;

}
