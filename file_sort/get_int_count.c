#include "get_int_count.h"
#include "exit_sys.h"

int get_int_count(const char *file_name, size_t *count)
{
    int failed = 1;
    FILE *file = fopen(file_name, "rb");
    if (!file)
        goto FAIL_LEVEL_1;

    for (*count = 0;fread(&(int){0}, sizeof(int), 1, file);++*count)
        if (ferror(file))
            goto FAIL_LEVEL_2;

    failed = 0;

    FAIL_LEVEL_2:
    fclose(file);
    FAIL_LEVEL_1:
    if (failed)
        set_error_str_errno(__func__);
    return failed;
}