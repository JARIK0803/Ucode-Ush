#include "../inc/ush.h"

static void del_dir_from_path(char **dir_names, int index)
{
    mx_strdel(&dir_names[index]);
    dir_names[index] = mx_strdup("");
}

static void handle_double_dot(char **dir_names, int index)
{
    del_dir_from_path(dir_names, index);

    for (int i = index; i >= 0; i--)
    {
        if (mx_strcmp(dir_names[i], "") != 0)
        {
            del_dir_from_path(dir_names, i);
            break;
        }
    }
}

static void handle_dir_names(char ***dir_names)
{
    for (int i = 0; (*dir_names)[i] != NULL; i++)
    {
        if (!mx_strcmp((*dir_names)[i], "."))
        {
            del_dir_from_path((*dir_names), i);
        }
        else if (!mx_strcmp((*dir_names)[i], ".."))
        {
            handle_double_dot((*dir_names), i);
        }
    }
}

static char *build_path(char **dir_names)
{
    char *path = NULL;

    for (int i = 0; dir_names[i] != NULL; i++)
    {
        if (!mx_strcmp(dir_names[i], ""))
        {
            continue;
        }

        char *tmp = mx_strjoin(path, "/");
        path ? mx_strdel(&path) : (void) 0;

        path = mx_strjoin(tmp, dir_names[i]);
        mx_strdel(&tmp);    
    }

    if (!path)
        path = mx_strdup("/");
    
    return path;
}

char *mx_normalize_path(char *pwd, char *point)
{
    char *path;
    char **dir_names;
    char *normalized_path;

    if (point[0] == '/')
    {
        path = mx_strdup(point);
    }
    else
    {
        char *tmp = mx_strjoin(pwd, "/");
        path = mx_strjoin(tmp, point);
        mx_strdel(&tmp);
    }

    dir_names = mx_strsplit(path, '/');
    handle_dir_names(&dir_names);
    normalized_path = build_path(dir_names);

    mx_strdel(&path);
    mx_del_strarr(&dir_names);
    
    return normalized_path;
}
