#include <stdlib.h>
#include <stdio.h>

#include "create_merge_files.h"
#include "get_int_count.h"
#include "exit_sys.h"



static int cmp(const void *vp1, const void *vp2)
{
    int val1 = *(const int *)vp1;
    int val2 = *(const int *)vp2;
    return val1 > val2 ? 1 : val1 < val2 ? -1 : 0;
}

static int fill_file_with_sorted_ints(FILE *dest_file, FILE *source_file, size_t n_ints){

    int failed = 1;

    int *buffer = malloc(sizeof(int) * n_ints);
    if (!buffer)
        goto FAIL_LEVEL_1;

    size_t read_blocks = fread(buffer, sizeof(int), n_ints, source_file);
    if (ferror(source_file))
        goto FAIL_LEVEL_2;

    qsort(buffer, n_ints, sizeof(int), &cmp);

    fwrite(buffer, sizeof(int), read_blocks, dest_file);
    if (ferror(dest_file))
        goto FAIL_LEVEL_2;

    if (fseek(dest_file, 0, SEEK_SET))
        goto FAIL_LEVEL_2;

    failed = 0;

    FAIL_LEVEL_2:
    free(buffer);
    FAIL_LEVEL_1:
    if (failed)
        set_error_str_errno(__func__);
    return failed;
}

void destroy_merge_files(FILE **merge_files, size_t merge_file_count)
{
    for (size_t i = 0;i < merge_file_count;++i)
        fclose(*(merge_files + i));

    free(merge_files);
}

FILE **create_merge_files(const char *source_file_name, size_t merge_file_count)
{
    void(*set_error_str)(const char *additional_message) = &set_error_str_errno;
    size_t file_int_count;
    if (get_int_count(source_file_name, &file_int_count)){
        prepend_message_to_error_str(__func__);
        return NULL;
    }

    FILE *source_file = fopen(source_file_name, "rb");
    if (!source_file)
        goto FAIL_LEVEL_1;

    FILE **merge_files = malloc(sizeof(FILE *) * merge_file_count);
    if (!merge_files)
        goto FAIL_LEVEL_2;

    size_t created_merge_files = 0;
    for (;created_merge_files < merge_file_count;++created_merge_files)
        if (!(*(merge_files + created_merge_files) = tmpfile()))
            goto FAIL_LEVEL_3;

    for (size_t i = 0;i < merge_file_count;++i)
        if (fill_file_with_sorted_ints(*(merge_files + i), source_file, file_int_count / merge_file_count)){
            set_error_str = &prepend_message_to_error_str;
            goto FAIL_LEVEL_3;
        }

    fclose(source_file);
    return merge_files;

    FAIL_LEVEL_3:
    destroy_merge_files(merge_files, created_merge_files);
    FAIL_LEVEL_2:
    fclose(source_file);
    FAIL_LEVEL_1:
    (*set_error_str)(__func__);
    return NULL;
}


