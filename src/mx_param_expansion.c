#include "../inc/ush.h"

static int get_param_len(char *str, int dollar_pos)
{
    int space_idx = mx_get_char_index(&str[dollar_pos + 1], ' ');
    int quote_idx = mx_get_char_index(&str[dollar_pos + 1], '\"');

    if (space_idx == -1 && quote_idx == -1)
    {
        return -1;
    }

    if (space_idx != -1 && quote_idx == -1)
    {
        return space_idx;
    }

    if (space_idx == -1 && quote_idx != -1)
    {
        return quote_idx;
    }

    if (space_idx != -1 && quote_idx != -1)
    {
        return space_idx < quote_idx ? space_idx : quote_idx;
    }
    
    return -1;
}

static bool is_bad_substitution(char *param)
{
    int space_idx = mx_get_char_index(param, ' ');
    int backslash_idx = mx_get_char_index(param, '\\');

    if (space_idx != -1 || backslash_idx != -1)
    {
        return true;
    }
    
    return false;
}

int mx_param_expansion(t_cmd_utils* utils, char **args)
{
    int dollar_pos;
    int to_replace_len;
    char *to_replace = NULL;
    char *param = NULL;
    t_env_var *env_var = NULL;

    for (int i = 0; args[i] != NULL; i++)
    {

        if (args[i][0] == '\'' || mx_strlen(args[i]) <= 1)
        {
            continue;
        }
        
        char *dup_arg = mx_strdup(args[i]);
        char *tmp = dup_arg;

        while ((dollar_pos = mx_get_char_index(tmp, '$')) != -1)
        {
            if (dollar_pos != -1 && !mx_isspace(tmp[dollar_pos + 1]) && tmp[dollar_pos + 1] != '(' && tmp[dollar_pos + 1] != '\\')
            {
                if (tmp[dollar_pos + 1] == '?') 
                {
                    param = mx_strndup(&tmp[dollar_pos], 2);
                    to_replace_len = 2;
                }
                else if (tmp[dollar_pos + 1] == '{')
                {
                    param = mx_strndup(&tmp[dollar_pos + 2], get_close_extension_brackets_idx(&tmp[dollar_pos], '{', '}') - 2);
                    to_replace_len = mx_strlen(param) + 3;
                }
                else
                {
                    int param_len = get_param_len(tmp, dollar_pos);
                    param = (param_len != -1) ? mx_strndup(&tmp[dollar_pos + 1], param_len) : mx_strdup(&tmp[dollar_pos + 1]);
                    to_replace_len = mx_strlen(param) + 1;   
                }

                if (is_bad_substitution(param))
                {
                    mx_printerr("ush: bad substitution\n");
                    return -1;
                }

                to_replace = mx_strndup(&tmp[dollar_pos], to_replace_len);
                if (tmp[dollar_pos + 1] != '?') 
                {
                    env_var = mx_find_env_var(utils->exported_vars, param, NULL);
                    if (env_var != NULL)
                    {
                        args[i] = mx_replace_substr_free(args[i], to_replace, env_var->value);
                    }
                    else
                    {
                        env_var = mx_find_env_var(utils->shell_vars, param, NULL);
                        if (env_var != NULL)
                        {
                            args[i] = mx_replace_substr_free(args[i], to_replace, env_var->value);
                        } 
                        else 
                        {    
                            args[i] = mx_replace_substr_free(args[i], to_replace, "");
                        }
                    }
                }
                else
                {
                    char* exit_code_str = mx_itoa(mx_get_last_exit_code(utils));
                    args[i] = mx_replace_substr_free(args[i], to_replace, exit_code_str);
                    mx_strdel(&exit_code_str);
                }

                mx_strdel(&param);
                mx_strdel(&to_replace);
            }

            tmp += dollar_pos + 1; // if str includes single $ 
        }
        mx_strdel(&dup_arg);
    }
    
    return 0;
}
