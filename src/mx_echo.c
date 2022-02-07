#include "../inc/ush.h"

void mx_echo_add_flag(t_echo_flags **flags, char flag)
{
    switch (flag)
    {

    case 'n':
        (*flags)->n = 1;
        break;

    case 'e':
        (*flags)->e = 1;
        (*flags)->E = 0;
        break;

    case 'E':
        (*flags)->e = 0;
        (*flags)->E = 1;
        break;

    default:
        break;
    }
}

static void print_oct(char *str, int *letter_count)
{
    int oct_len = 0;
    while (mx_isdigit(str[oct_len]) && (oct_len <= 3) && (str[oct_len] != '\0')) // 3 is a max len of oct num in our shell
    {
        oct_len++;
        (*letter_count)++;
    }

    if (!oct_len)
    {
        return;
    }

    char *oct = mx_strndup(str, oct_len);
    int dec = mx_octal_to_decimal(mx_atoi(oct));
    mx_printchar(dec);

    mx_strdel(&oct);
}

static void print_hex(char *str, int *letter_count)
{
    int hex_len = 0;
    while (mx_isdigit(str[hex_len]) && (hex_len <= 2) && (str[hex_len] != '\0')) // 2 is a max len of hex num in our shell
    {
        hex_len++;
        (*letter_count)++;
    }

    if (!hex_len)
    {
        return;
    }

    char *hex = mx_strndup(str, hex_len);
    int dec = mx_hex_to_nbr(hex);
    mx_printchar(dec);

    mx_strdel(&hex);
}

// bec - backslash escape character
static void print_bec(char *str, int *letter_count, bool quotes)
{
    int bec_pos = quotes ? 0 : 1;

    //for out backslashes like in zsh
    if (str[bec_pos + 1] == '\\' && str[bec_pos + 2] != '\\')
    {
        return;
    }
    else if (str[bec_pos + 1] == '\\' && str[bec_pos + 2] == '\\')
    {
        mx_printchar('\\');
        (*letter_count) += bec_pos + 2;
        return;
    }
    //

    switch (str[bec_pos + 1])
    {

    case 'a':
        mx_printchar('\a');
        (*letter_count) += bec_pos + 1;
        break;

    case 'b':
        mx_printchar('\b');
        (*letter_count) += bec_pos + 1;
        break;

    case 'c':
        (*letter_count) += mx_strlen(str);
        break;

    case 'e':
        mx_printchar(27);
        (*letter_count) += bec_pos + 1;
        break;

    case 'f':
        mx_printchar('\f');
        (*letter_count) += bec_pos + 1;
        break;

    case 'n':
        mx_printchar('\n');
        (*letter_count) += bec_pos + 1;
        break;

    case 't':
        mx_printchar('\t');
        (*letter_count) += bec_pos + 1;
        break;

    case 'v':
        mx_printchar('\v');
        (*letter_count) += bec_pos + 1;
        break;

    case '0':
        if (mx_isdigit(str[bec_pos + 2]))
        {
            print_oct(str + bec_pos + 2, letter_count);
            (*letter_count) += bec_pos + 1;
            break;
        }
        else
        {
            mx_printchar('\\');
            (*letter_count) += bec_pos;
            break;
        }

    case 'x':
        if (mx_isalpha(str[bec_pos + 2]) || mx_isdigit(str[bec_pos + 2]))
        {
            print_hex(str + 3, letter_count);
            (*letter_count) += bec_pos + 1;
            break;
        }
        else
        {
            mx_printchar('\\');
            (*letter_count) += bec_pos;
            break;
        }

    default:
        mx_printchar('\\');
        (*letter_count) += bec_pos;
        break;
    }
}

static void out_echo_e(char *str)
{
    for (int i = 0; i < mx_strlen(str); i++)
    {
        if (str[0] == '"' || str[0] == '\'')
        {
            if (i == 0 || i == mx_strlen(str) - 1)
            {
                continue;
            }
            else if (str[i] == '\\')
            {
                print_bec(str + i, &i, true);
            }
            else
            {
                mx_printchar(str[i]);
            }
        }
        else
        {
            if (str[i] == '\\' && str[i + 1] != '\\')
            {
                continue;
            }
            else if (str[i] == '\\' && str[i + 1] == '\\')
            {
                print_bec(str + i, &i, false);
            }
            else
            {
                mx_printchar(str[i]);
            }
        }   
    }
}

static void out_echo_E(char *str)
{
    if (str[0] == '"' || str[0] == '\'')
    {
        for (int i = 1; i < mx_strlen(str) - 1; i++)
            mx_printchar(str[i]);
    }
    else
    {
        mx_printstr(str);
    }
}

int mx_echo(t_cmd_utils *utils, char** args)
{
    int flag_count = 0;
    t_echo_flags *flags = malloc(sizeof(*flags));
    mx_echo_parse_flags(&flags, args ? args : utils->args, &flag_count);

    for (int i = 1 + flag_count; args[i]; i++)
    {
        flags->e ? out_echo_e(args[i]) : out_echo_E(args[i]);
        args[i + 1] ? mx_printstr(" ") : (void)0;
    }

    flags->n ? (void)0 : mx_printstr("\n");
    free(flags);
    return 0;
}
