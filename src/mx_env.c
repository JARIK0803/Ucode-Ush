#include "../inc/ush.h"

void mx_env_add_flag(t_env_flags** flags, char flag) {

    switch (flag) { 

    case 'i':
        (*flags)->i = 1; 
        (*flags)->P = 0; 
        break;

    case 'P':
        (*flags)->P = 1; break;

    case 'u':
        (*flags)->u = 1; break;
    
    default: break;
    
    }

}

int mx_set_env_vars(t_cmd_utils* utils, char** args, int* arg_idx) {

    for (int i = *arg_idx; args[i] != NULL; ++i) {

        char* arg = args[i];
        if (mx_strstr(arg, "=") != NULL) {
            char* var_name = mx_get_var_name(arg);
            char* var_value = mx_get_var_value(arg);
            if (mx_strcmp(var_name, "") == 0) {
                mx_printerr("ush: ");
                mx_printerr(var_value);
                mx_printerr(" not found\n");
                return 1;
            }
            t_env_var* env_var = mx_find_env_var(utils->env_vars, var_name, NULL);
            
            if (env_var == NULL) {

                mx_env_push_back(&utils->env_vars, arg);

            } else {

                mx_overwrite_env_var(&env_var, var_value);

            }
            mx_strdel(&var_name);
            mx_strdel(&var_value);

        } else break;
        ++(*arg_idx);

    }
    return 0;

}

void mx_add_shell_var(t_cmd_utils* utils, char* var_str) {

    char* name = mx_get_var_name(var_str);
    t_env_var* shell_var = mx_find_env_var(utils->shell_vars, name, NULL);
    
    if (shell_var == NULL) {

        mx_env_push_back(&utils->shell_vars, var_str);

    } else if (mx_strstr(var_str, "=") != NULL) {

        char* value = mx_get_var_value(var_str);
        t_env_var* exported_var = mx_find_env_var(utils->exported_vars, name, NULL);
        t_env_var* env_var = mx_find_env_var(utils->env_vars, name, NULL);
        if (exported_var){
            setenv(name, value, 1);
            mx_overwrite_env_var(&exported_var, value);
        }
        if (env_var)
            mx_overwrite_env_var(&env_var, value);
        mx_overwrite_env_var(&shell_var, value);
        
    }

}

int mx_remove_env_var(t_cmd_utils** utils, char* name) {

    if (mx_strchr(name, '=') != NULL) {
        mx_printerr("env: cannot unsetenv '");
        mx_printerr(name);
        mx_printerr("': Invalid argument\n");
        return 1;
    }

    int env_index = 0;
    int export_index = 0;
    int sh_var_idx = 0;
    mx_find_env_var((*utils)->exported_vars, name, &export_index);
    mx_env_pop_index(&(*utils)->exported_vars, export_index);

    if (mx_find_env_var((*utils)->env_vars, name, &env_index) != NULL) {

        mx_env_pop_index(&(*utils)->env_vars, env_index);
    
    } else {

        mx_find_env_var((*utils)->shell_vars, name, &sh_var_idx);
        mx_env_pop_index(&(*utils)->shell_vars, sh_var_idx);
    
    }
    return 0;

}

int mx_env(t_cmd_utils* utils, char** args) {

    int curr_arg_idx = 1;
    t_env_flags* flags = malloc(sizeof(*flags));
    if (mx_env_parse_flags(&flags, args, &curr_arg_idx) != 0) {
        utils->builtin_exit_code = 1;
        return 1;
    }

    if (args[curr_arg_idx]) {

        mx_exec_env_utility(utils, args, curr_arg_idx, flags);

    } else {
        
        if (mx_handle_new_process(utils, args, flags, &curr_arg_idx) != 0) {
            mx_env_reset(&utils);
            utils->builtin_exit_code = 1;
            return 1;
        }
        mx_env_reset(&utils);
    
    }
    mx_strdel(&flags->p_param);
    mx_strdel(&flags->u_param);
    free(flags);

    return 0;

}
