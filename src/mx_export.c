#include "../inc/ush.h"

bool export_cmp(char* a, char* b) {

    return mx_strcmp(a, b) > 0;

}

char* mx_get_var_name(const char* var) {

    char* var_name = mx_strndup(var, mx_get_char_index(var, '='));
    char* result = mx_replace_substr(var_name, "\"", "");
    mx_strdel(&var_name);
    return result;

}

char* mx_get_var_value(const char* var) {

    if (mx_strstr(var, "=") != NULL) {

        char* var_value = mx_strchr(var, '=') + 1;
        char* result = mx_replace_substr(var_value, "\"", "");
        return result;
        
    }
    return mx_strdup("");

}

char* mx_get_var_str(t_env_var* env_var) {

    char* result = mx_strnew(mx_strlen(env_var->name) + mx_strlen(env_var->value) + 1);
    mx_strcat(result, env_var->name);
    mx_strcat(result, "=");
    mx_strcat(result, env_var->value);
    return result;

}

int mx_unset(t_cmd_utils* utils, char** args) {

    if (mx_parse_for_no_flags(args, "unset") != 0)
        return 1;

    for (int idx = 1; args[idx] != NULL; ++idx) {

        if (mx_remove_env_var(&utils, args[idx]) != 0) {
            exit(EXIT_FAILURE);
        }
        char* var_name = mx_get_var_name(args[idx - 1]);
        unsetenv(var_name);
        mx_strdel(&var_name);

    }
    return 0;

}

void mx_export_vars(t_cmd_utils* utils, char** args) {

    for (int i = 1; args[i] != NULL; ++i) {

        char* arg = args[i];
        bool is_to_overwrite = mx_strstr(arg, "=") != NULL;
        char* var_name = mx_get_var_name(arg);
        char* var_value = mx_get_var_value(arg);
        if (mx_strlen(var_name) == 0) {
            mx_strdel(&var_name);
            mx_printerr(USH_STR);
            mx_printerr(var_value);
            mx_printerr(" not found\n");
            continue;
        }
        t_env_var* exported_var = mx_find_env_var(utils->exported_vars, var_name, NULL);
        t_env_var* sh_var = mx_find_env_var(utils->shell_vars, var_name, NULL);
        if (exported_var == NULL) {

            if (sh_var && !is_to_overwrite) {

                if (var_value)
                        setenv(var_name, sh_var->value, 1);

                mx_env_dup_push_back(&utils->exported_vars, sh_var);
                mx_env_dup_push_back(&utils->env_vars, sh_var);

            } else {

                if (sh_var && is_to_overwrite) {

                    if (var_value)
                        setenv(var_name, sh_var->value, 1);
                    mx_overwrite_env_var(&sh_var, var_value);

                }
                if (var_value)
                        setenv(var_name, var_value, 1);
                mx_env_push_back(&utils->exported_vars, arg);
                mx_env_push_back(&utils->env_vars, arg);
            
            }

        } else if (is_to_overwrite) {

            mx_overwrite_env_var(&exported_var, var_value);
            if (sh_var) {

                mx_overwrite_env_var(&sh_var, var_value);

            } else {

                t_env_var* env_var = mx_find_env_var(utils->env_vars, var_name, NULL);
                mx_overwrite_env_var(&env_var, var_value);

            }

            if (var_value)
                setenv(var_name, var_value, 1);
        
        }
        mx_strdel(&var_value);
        mx_strdel(&var_name);
        
    }

}

int mx_export(t_cmd_utils* utils, char** args) {

    if (mx_parse_for_no_flags(args, "export") != 0)
        return 1;

    if (args[1] != NULL) {
        
        mx_export_vars(utils, args);

    } else {

        mx_print_env_list(utils->exported_vars, false);

    }
    return 0;

}

void mx_export_reset(t_cmd_utils** utils) {

    mx_env_clear_list(&(*utils)->exported_vars);
    for (int i = 0; environ[i] != NULL; ++i) {

        // remove the LS_COLORS check later
        if (environ[i][0] == '_' || mx_strstr(environ[i], "LS_COLORS") != NULL)
            continue;

        mx_env_push_back(&(*utils)->exported_vars, environ[i]);

    }
    mx_env_sort_list(&(*utils)->exported_vars, export_cmp);

}
