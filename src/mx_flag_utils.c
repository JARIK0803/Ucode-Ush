#include "../inc/ush.h"

static bool is_flag_found(const char* flags, char flag) {

    for (int i = 0; i < mx_strlen(flags); ++i) {

        if (flag == flags[i])
            return true;

    }
    return false;

}

static bool is_only_flags(const char* flags, char* flags_str) {

    bool is_flag;
    for (int i = 1; i < mx_strlen(flags_str); i++) {
        is_flag = false;

        for (int j = 0; j < mx_strlen(flags); j++) {

            if (flags_str[i] == flags[j]) {
                is_flag = true;
                break;
            }
        }

        if (!is_flag)
        {
            return false;
        }
        
    }
    return true;
}

void mx_cd_parse_flags(t_cd_flags** flags, char** args, int* arg_idx) {

    (*flags)->P = (*flags)->prev = (*flags)->s = 0;

    if (args == NULL) return;
    const char* const_flags = "sP";
    for (int i = 1; args[i] != NULL; ++i) {

        char* arg = args[i];
        if (arg[0] == '-' && mx_strlen(arg) == 1 && !args[i + 1]) {
            mx_cd_add_flag(flags, arg[0]);
            ++(*arg_idx);
            continue;
        } 
        if ((arg[0] == '-') && !mx_isspace(arg[1])) {

            ++(*arg_idx);
            for (int j = 1; arg[j] != '\0'; j++) {

                if (is_flag_found(const_flags, arg[j])) {
                    mx_cd_add_flag(flags, arg[j]);
                } else {
                    --(*arg_idx);
                }
            
            }

        } else break;
    }

}

int mx_wch_parse_flags(t_wch_flags** flags, char** args, int* arg_idx) {

    (*flags)->s = (*flags)->a = 0;

    if (args == NULL) return 0;

    const char* const_flags = "sa";
    for (int i = 1; args[i] != NULL; ++i) {

        char* arg = args[i];
        if ((arg[0] == '-') && !mx_isspace(arg[1])) {

            ++(*arg_idx);
            for (int j = 1; arg[j] != '\0'; j++) {

                if (is_flag_found(const_flags, arg[j])) {
                    mx_wch_add_flag(flags, arg[j]);
                } else {
                    mx_print_flag_err("which", arg[j]);
                    return 1;
                }
            
            }

        } else break;
    }
    return 0;

}

int mx_env_parse_flags(t_env_flags** flags, char** args, int* arg_idx) {

    (*flags)->i = (*flags)->P = (*flags)->u = 0;
    (*flags)->u_param = (*flags)->p_param = NULL;

    if (args == NULL) return 0;
    const char* const_flags = "iPu";

    for (int i = 1; args[i] != NULL; ++i) {

        char* arg = args[i];
        if (mx_strstr(arg, "=") == NULL) {
            if ((arg[0] == '-') && !mx_isspace(arg[1])) {

                ++(*arg_idx);
                int flag_count = mx_strlen(arg);
                for (int j = 1; j < flag_count; j++) {

                    if (is_flag_found(const_flags, arg[j])) {

                        mx_env_add_flag(flags, arg[j]);
                        if (arg[j] == 'u') {

                            if (args[i + 1] != NULL && j == flag_count - 1) {
                                (*flags)->u_param = mx_strdup(args[++i]);
                                ++(*arg_idx);
                            } else if (j == flag_count - 1) {
                                mx_print_env_arg_err(arg[j]);
                                return 1;
                            }

                        } else if (arg[j] == 'P') {

                            if (args[i + 1] != NULL && j == flag_count - 1) {
                                (*flags)->p_param = mx_strdup(args[++i]);
                                ++(*arg_idx);
                            } else if (j == flag_count - 1) {
                                mx_print_env_arg_err(arg[j]);
                                return 1;
                            }

                        }
                    
                    } else if (j == 1) {
                        mx_print_env_option_err(arg[j], "env");
                        --(*arg_idx);
                        return 1;
                    }
                }
            } else break;
        } else break;
    }
    return 0;
}

void mx_echo_parse_flags(t_echo_flags** flags, char** args, int *flag_count) {

    (*flags)->n = (*flags)->E = 0;
    (*flags)->e = 1;
    // (*flags)->E = 1; //default

    if (args == NULL) return;

    const char* const_flags = "neE";
    for (int i = 1; args[i] != NULL; ++i) {

        char* arg = args[i];
        if ((arg[0] == '-') && !mx_isspace(arg[1])) {

            if (!is_only_flags(const_flags, arg))
                return;
            
            for (int j = 1; arg[j] != '\0'; j++)
                mx_echo_add_flag(flags, arg[j]);

        } else return;

        (*flag_count)++;
    }

}

int mx_pwd_parse_flags(t_pwd_flags** flags, char** args) {

    (*flags)->L = 1;
    (*flags)->P = 0;

    if (args == NULL) return 0;

    const char* const_flags = "LP";
    for (int i = 1; args[i] != NULL; ++i) {

        char* arg = args[i];

        if (arg[0] != '-')
        {
            mx_print_too_many_args_err("pwd");
            return 1;
        }
        
        if ((arg[0] == '-') && !mx_isspace(arg[1])) {

            for (int j = 1; arg[j] != '\0'; j++) {

                if (is_flag_found(const_flags, arg[j])) {
                    mx_pwd_add_flag(flags, arg[j]);
                } else {
                    mx_print_flag_err("pwd", arg[j]);
                    return 1;
                }
            
            }

        } else break;
    }
    return 0;

}

int mx_parse_for_no_flags(char** args, const char* cmd) {

    if (args == NULL) return 0;

    for (int i = 1; args[i] != NULL; ++i) {

        char* arg = args[i];
        if ((arg[0] == '-') && !mx_isspace(arg[1])) {

            mx_print_flag_err(cmd, arg[1]);
            return 1;

        } else break;
    }
    return 0;

}
