#include "../inc/ush.h"

int mx_tilde_expansion(char **args)
{
    for (int i = 0; args[i] != NULL; i++)
    {
        if (args[i][0] == '~')
        {
            args[i] = mx_replace_substr_free(args[i], "~+", getenv(PWD_STR));
            args[i] = mx_replace_substr_free(args[i], "~-", getenv(OLDPWD_STR));

            int tilde_index = mx_get_char_index(args[i], '~');
            if (mx_strlen(args[i]) > 1 && !mx_isspace(args[i][tilde_index + 1]) && args[i][tilde_index + 1] != '/')
            {
                int slash_idx = mx_get_char_index(args[i], '/');
                char *username = NULL;

                if (slash_idx != -1)
                {
                    username = mx_strndup(&args[i][tilde_index + 1], slash_idx - 1);
                }
                else
                {
                    username = mx_strdup(&args[i][tilde_index + 1]);
                }

                if (mx_get_char_index(username, ' ') == -1)
                {
                    char *path = mx_replace_substr(getenv(HOME_STR), getenv("USER"), username);
                    char* result = NULL;
                    if (slash_idx != -1 && mx_strlen(args[i]) > slash_idx) {

                        result = mx_strjoin(path, &args[i][slash_idx]);
                        mx_strdel(&path);
                    
                    }

                    DIR* dir = opendir(result);

                    if (errno == ENOENT) {
                        mx_printerr("ush: no such user or named directory: ");
                        mx_printerr(result);
                        mx_printerr("\n");

                        return 1;
                    } 
                    else 
                    {    
                        mx_strdel(&args[i]);
                        args[i] = result;
                    }
                    closedir(dir);
                }
                else
                {
                    mx_strdel(&username);
                    return 0;
                }

                mx_strdel(&username);
            }
            else
            {
                args[i] = mx_replace_substr_free(args[i], "~", getenv(HOME_STR));
            }
        }
    }

    return 0;
}
