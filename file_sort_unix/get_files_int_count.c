#include <fcntl.h>
#include <unistd.h>

#include "get_files_int_count.h"
#include "exit_sys.h"

ssize_t get_files_int_count(const char *file_name)
{
    file_descriptor file = open(file_name, O_RDONLY);
    if (file == -1)
        goto FAIL;

    ssize_t count = 0;

    for (ssize_t res;(res = read(file, &(int){0}, sizeof(int))) == sizeof(int);++count)
        if (res == -1)
            goto FAIL;

    return count;

    FAIL:
    set_error_str_errno(__func__);
    return -1;
}