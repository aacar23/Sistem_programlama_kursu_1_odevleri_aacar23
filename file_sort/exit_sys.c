#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "exit_sys.h"

#define ERROR_STR_SIZE (1000)

static char error_str[ERROR_STR_SIZE];

static void prepend_str_to_str(char *dest, const char *source)
{
    size_t source_size = strlen(source);
    memmove(dest + source_size + 2, dest, strlen(dest) + 1);
    strcpy(dest, source);
    *(dest + source_size) = ':';
    *(dest + source_size + 1) = ' ';
}

void set_error_str_errno(const char *additional_message)
{
    set_error_str_custom(additional_message, strerror(errno));
}

void prepend_message_to_error_str(const char *message)
{
    prepend_str_to_str(error_str, message);
}

void set_error_str_success(const char *additional_message)
{
    set_error_str_custom(additional_message, strerror(errno));
}

void set_error_str_custom(const char *additional_message, const char *error_message)
{
    sprintf(error_str, "%s: %s", additional_message, error_message);
}

void exit_sys(void)
{
    printf("%s\n", error_str);
    exit(EXIT_FAILURE);
}