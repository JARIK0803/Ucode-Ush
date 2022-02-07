#include "../inc/ush.h"

char** mx_read_line() {

    char* line = NULL;
    size_t length = 0;

    if (getline(&line, &length, stdin) == -1) {

        free(line);
        exit(1);

    }
    char** separated = mx_strsplit(line, ';');
    mx_strdel(&line);
    return separated;

}
