#include "../inc/ush.h"

static void insert_arr_in_elem_of_arr(char ***arr, char **new_elems, int insert_idx)
{
    int arr_len = 0;
    int new_elems_len = 0;

    for (; (*arr)[arr_len] != NULL; ++arr_len);
    for (; new_elems[new_elems_len] != NULL; ++new_elems_len);

    int res_arr_len = arr_len + new_elems_len + 1;

    char** res_arr = malloc((res_arr_len) * sizeof(**arr));
    for (int i = 0 ; i < res_arr_len; ++i) {
        res_arr[i] = NULL;
    }

    int idx = 0;
    for (; idx < insert_idx; ++idx) {

        res_arr[idx] = mx_strdup((*arr)[idx]);

    }
    for (int j = 0; j < new_elems_len; ++idx, ++j) {

        res_arr[idx] = mx_strdup(new_elems[j]);

    }
    for (; idx < arr_len + new_elems_len - 1; ++idx) {

        res_arr[idx] = mx_strdup((*arr)[idx - new_elems_len + 1]);

    }
    res_arr[idx] = NULL;

    mx_del_strarr(arr);

    *arr = malloc((res_arr_len) * sizeof(**arr));
    for (int i = 0 ; i < res_arr_len; ++i) {
        (*arr)[i] = NULL;
    }
    for (int i = 0; res_arr[i] != NULL; ++i) {

        (*arr)[i] = mx_strdup(res_arr[i]);

    }
    (*arr)[arr_len + new_elems_len] = NULL;
    mx_del_strarr(&res_arr);

}

void mx_command_substitution(char ***args, t_cmd_utils* utils)
{

    int dollar_pos;
    char *to_replace = NULL;
    char *cmd = NULL;

    int i = 0;
    for (; (*args)[i] != NULL; i++)
    {
        if ((*args)[i][0] == '\'' || mx_strlen((*args)[i]) <= 1)
        {
            continue;
        }

        char *tmp = (*args)[i];
        char **new_args = NULL;
        
        while ((dollar_pos = mx_get_char_index(tmp, '$')) != -1)
        {
            if (mx_get_substr_index(&tmp[dollar_pos + 1], "$(") != -1)
            {
                tmp += dollar_pos + 1;
                continue;
            }
            else if (tmp[dollar_pos + 1] == '(')
            {
                cmd = mx_strndup(&tmp[dollar_pos + 2], mx_get_char_index(&tmp[dollar_pos + 2], ')'));

                char** cmd_args = NULL;
                char *result = mx_strdup("");
                mx_parse_line(utils, cmd, &cmd_args);

                if (mx_strcmp(cmd_args[0], "") != 0) {
                    
                    mx_strdel(&result);
                    result = mx_cmd_exec(utils, cmd_args);

                }

                if ((*args)[i][0] == '\"')
                {
                    result = mx_strtrim(result);
                }
                else
                {
                    result = mx_del_extra_spaces(result);
                }
                
                to_replace = mx_strndup(&tmp[dollar_pos], mx_strlen(cmd) + 3);

                (*args)[i] = mx_replace_substr_free((*args)[i], to_replace, result);
                
                mx_del_strarr(&new_args);
                new_args = mx_strsplit(result, ' ');

                mx_strdel(&result);
                mx_strdel(&cmd);
                mx_strdel(&to_replace);
                tmp = (*args)[i];
            }
        }

        if (new_args != NULL && (*args)[i][0] != '\"')
        {
            insert_arr_in_elem_of_arr(args, new_args, i);
        }

        mx_del_strarr(&new_args);
    }
}
