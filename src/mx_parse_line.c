#include "../inc/ush.h"

static int check_odd_quotes(char *str)
{
    int single_quotes = 0;  //'
    int double_quotes = 0;  //"
    int parentheses = 0;    //()

    for (int i = 0; str[i] != '\0'; i++)
    {
        if (!single_quotes && !double_quotes)
        {
            if (str[i] == '\'' && str[i - 1] != '\\')
            {
                single_quotes++;
            }
            else if (str[i] == '\"' && str[i - 1] != '\\')
            {
                double_quotes++;
            }
            else if (str[i] == '(')
            {
                parentheses++;
            }
            else if (str[i] == ')')
            {
                parentheses--;
                if (parentheses < 0)
                {
                    mx_printerr("ush: parse error near `)\'\n");
                    return 1;
                }  
            }
        }
        else if (single_quotes && !double_quotes)
        {
            if (str[i] == '\'' && str[i - 1] != '\\')
            {
                single_quotes--;
            }
            continue;
        }
        else if (!single_quotes && double_quotes)
        {
            if (str[i] == '\"' && str[i - 1] != '\\')
            {
                double_quotes--;
            }
            continue;
        }
    }
    
    if (single_quotes || double_quotes || parentheses)
    {
        mx_print_odd_quotes_err();
        return 1;
    }

    return 0;
}

char *mx_replace_substr_free(char *str, char *sub, char *replace)
{
    char *tmp = str;
    char *result = mx_replace_substr(tmp, sub, replace);
    mx_strdel(&tmp);

    return result;
}

static void handle_backslashes(char **args)
{

    if (!mx_strcmp(args[0], "echo") || !mx_strcmp(args[0], "\"echo\"") || !mx_strcmp(args[0], "'echo'"))
    {
        return;
    }

    for (int i = 1; args[i] != NULL; i++)
    {
        if (args[i][0] != '\'' && args[i][0] != '"')
        {
            args[i] = mx_replace_substr_free(args[i], "\\ ", " ");
            args[i] = mx_replace_substr_free(args[i], "\\`", "`");
            args[i] = mx_replace_substr_free(args[i], "\\'", "'");
            args[i] = mx_replace_substr_free(args[i], "\\\"", "\"");
            args[i] = mx_replace_substr_free(args[i], "\\", "");
        }
    }
}

static void del_extra_quotes(char **args)
{
    for (size_t i = 0; args[i] != NULL; i++)
    {
        if (args[i][0] == '\'' || args[i][0] == '"')
        {
            char *tmp = mx_strndup(args[i] + 1, mx_strlen(args[i]) - 2); //dup without quotes
            mx_strdel(&args[i]);
            args[i] = tmp;
        }

        if (i == 0 && !mx_strcmp(args[0], "echo"))
        {
            return;
        }
    }
}

int get_close_extension_brackets_idx(char *str, const char opening, const char closing)
{
    int open_brackets = 0;

    for (int i = 0; str[i] != '\0'; i++)
    {
        if (str[i] == '$' && str[i + 1] == opening)
        {
            open_brackets++;
        }
        if (str[i] == closing)
        {
            open_brackets--;
        }
        if (open_brackets == 0)
        {
            return i;
        }
    }

    return -1;
}

static int get_close_quote_idx(char *str, const char quote)
{
    bool is_quote_open = false;
    bool is_open = false;

    for (int i = 0; str[i] != '\0'; i++)
    {
        if (!is_quote_open && str[i] == quote && str[i - 1] != '\\')
        {
            is_quote_open = true;
            is_open = true;
        }
        else if (is_quote_open && str[i] == quote && str[i - 1] != '\\')
        {
            is_quote_open = false;
        }
        if (is_open && !is_quote_open)
        {
            return i;
        }
    }

    return -1;
}

static void move_to_next_elem(char *str, int *index, bool *is_finish)
{   
    while (str[*index] != '\0' && (!mx_isspace(str[*index]) || (str[(*index) - 1] == '\\' && str[(*index)] == ' ')))
    {
        if (str[*index] == '\'')
        {

            *index += get_close_quote_idx(&str[(*index)], '\'') + 1;
            continue;
        }
        else if (str[*index] == '\"' && str[(*index) - 1] != '\\')
        {
            *index += get_close_quote_idx(&str[(*index)], '\"') + 1;
            continue;
        }
        else if (str[*index] == '$' && str[(*index) + 1] == '{')
        {
            *index += get_close_extension_brackets_idx(&str[*index], '{', '}');
            continue;
        }
        else if (str[*index] == '$' && str[(*index) + 1] == '(')
        {
            *index += get_close_extension_brackets_idx(&str[*index], '(', ')');
            continue;
        }
        else
        {
            (*index)++;
        }        
    }

    if (str[(*index)] == '\0')
    {
        *is_finish = true;
    }
    else
    {
        *is_finish = false;
    }
}

static bool is_variable(char *str)
{
    if ((mx_isalpha(str[0]) || str[0] == '=') && mx_get_char_index(str, '=') != -1)
    {
        return true;
    }

    return false;
}

static bool is_variable_valid(char *str)
{
    if (str[0] == '=')
    {
        mx_printerr(USH_STR);
        mx_printerr(&str[1]);
        mx_printerr(" not found\n");
        return false;
    }
    return true;
}

int mx_parse_line(t_cmd_utils *utils, char *line, char ***dst_args)
{
    char *mod_line = mx_strtrim(line);
    if (check_odd_quotes(mod_line) != 0)
        return 1;

    (*dst_args) = NULL;

    char *tmp = mod_line;
    bool is_finish = false;
    int i = 0;

    while (!is_finish)
    {
        int index = 0;

        (*dst_args) = mx_realloc((*dst_args), (i + 2) * sizeof(char *));

        move_to_next_elem(tmp, &index, &is_finish);

        if (is_finish)
        {
            (*dst_args)[i++] = mx_strndup(mod_line, mx_strlen(mod_line));
        }
        else
        {
            tmp += index;
            (*dst_args)[i++] = mx_strndup(mod_line, mx_strlen(mod_line) - mx_strlen(tmp));
        }

        if (i - 1 == 0 && is_variable((*dst_args)[i - 1]))
        {
            if (!is_variable_valid((*dst_args)[i - 1]))
                return 1;

            char *new_var = mx_strdup((*dst_args)[i - 1]);
            
            mx_add_shell_var(utils, new_var);

            mx_strdel(&(*dst_args)[i - 1]);

            i--;
        }
        

        while (mx_isspace(tmp[0]))
        {
            tmp++;
        }

        mod_line = tmp;
    }

    (*dst_args)[i] = NULL;

    if ((*dst_args)[0] != NULL) {

        handle_backslashes((*dst_args));
        if (mx_param_expansion(utils, (*dst_args)) != 0)
            return 1; 

        if (mx_tilde_expansion((*dst_args)) != 0)
            return 1;

        mx_command_substitution(dst_args, utils);
        del_extra_quotes((*dst_args));

    }

    return 0;
}
