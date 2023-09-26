#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "exit_sys.h"

static char error_str[1000];

static void prepend_str_to_str(char *dest_str, const char *source_str)
{
    memmove(dest_str + strlen(source_str), dest_str, strlen(dest_str) + 1);
    strncpy(dest_str, source_str, strlen(source_str));
}

void prepend_message_to_error_str(const char *message)
{
    prepend_str_to_str(error_str, ": ");
    prepend_str_to_str(error_str, message);
}

void set_error_str_errno(const char *additional_message)
{
    sprintf(error_str, "%s: %s", additional_message, strerror(errno));
}

void set_error_str_custom(const char *error_message)
{
    strcpy(error_str, error_message);
}

void exit_sys(void)
{
    fprintf(stderr, "%s\n", error_str);
    exit(EXIT_FAILURE);
}