#include "../inc/ush.h"

void mx_pwd_add_flag(t_pwd_flags **flags, char flag)
{
    switch (flag)
    {

    case 'L':
        (*flags)->L = 1;
        (*flags)->P = 0;
        break;

    case 'P':
        (*flags)->P = 1;
        (*flags)->L = 0;
        break;

    default:
        break;
    }
}

static void print_pwd(char *pwd)
{
    mx_printstr(pwd);
    mx_printstr("\n");
}

int mx_pwd(t_cmd_utils* utils, char** args)
{
    t_pwd_flags *flags = malloc(sizeof(*flags));
    if (mx_pwd_parse_flags(&flags, args ? args : utils->args) != 0)
        return 1;

    if (flags->L)
    {
        char *cwd = NULL;
        (cwd = getenv(PWD_STR)) ? print_pwd(cwd) : (void) 0;
    }
    else if (flags->P)
    {
        char cwd[PATH_MAX];
        realpath(getenv(PWD_STR), cwd) ? print_pwd(cwd) : (void) 0; // if env clear PWD variable?
    }
    free(flags);

    return 0;
}
