#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>



#include "create_merge_files.h"
#include "get_files_int_count.h"
#include "exit_sys.h"


static void destroy_temp_files(temp_file *temp_files, ssize_t temp_file_count)
{
    for (ssize_t i = 0;i < temp_file_count;++i){
        close((temp_files + i) -> file);
        if (remove((temp_files + i) -> file_name)){
            fprintf(stderr, "%s: %s", __func__, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    free(temp_files);
}

static temp_file *create_temp_files(ssize_t temp_file_count)
{
    temp_file *temp_files = malloc(sizeof(temp_file) * temp_file_count);
    if (!temp_files)
        goto FAIL_LEVEL_1;

    
    ssize_t created_file_count = 0;
    for (;created_file_count < temp_file_count;++created_file_count){
        char temp_file_name[] = "./XXXXXX";
        (temp_files + created_file_count) -> file = mkstemp(temp_file_name);
        if ((temp_files + created_file_count) -> file == -1)
            goto FAIL_LEVEL_2;
        strcpy((temp_files + created_file_count) -> file_name, temp_file_name);
    }


    return temp_files;


    FAIL_LEVEL_2:
    destroy_temp_files(temp_files, created_file_count);
    FAIL_LEVEL_1:
    set_error_str_errno(__func__);
    return NULL;
}

static int cmp(const void *vp1, const void *vp2)
{
    const int *p1 = (const int *)vp1;
    const int *p2 = (const int *)vp2;

    return *p1 > *p2 ? 1 : *p1 < *p2 ? -1 : 0;
}

static int fill_file_with_sorted_int(file_descriptor dest_file, file_descriptor source_file, ssize_t int_count)
{
    int failure = 1;
    int *temp_buffer = malloc(sizeof(int) * int_count);
    if (!temp_buffer)
        goto FAIL_LEVEL_1;

    int read_bytes = read(source_file, temp_buffer, sizeof(int) * int_count);
    if (read_bytes == -1)
        goto FAIL_LEVEL_2;

    qsort(temp_buffer, int_count, sizeof(int), &cmp);

    if (write(dest_file, temp_buffer, read_bytes / sizeof(int) * sizeof(int)) == -1)
        goto FAIL_LEVEL_2;

    lseek(dest_file, 0, SEEK_SET);

    failure = 0;

    FAIL_LEVEL_2:
    free(temp_buffer);
    FAIL_LEVEL_1:
    if (failure)
        set_error_str_errno(__func__);
    return failure;
}


static int fill_files_with_sorted_ints(const temp_file *temp_files, const char *source_file_name, ssize_t temp_file_count, ssize_t int_count)
{
    file_descriptor source_file = open(source_file_name, O_RDONLY);
    if (source_file == -1){
        set_error_str_errno(__func__);
        return 1;
    }

    for (ssize_t i = 0;i < temp_file_count;++i)
        if (fill_file_with_sorted_int((temp_files + i) -> file, source_file, int_count)){
            prepend_message_to_error_str(__func__);
            close(source_file);
            return 1;
        }

    close(source_file);
    return 0;
}


temp_file *create_merge_files(const char *source_file_name, ssize_t merge_file_count)
{
    ssize_t files_int_count = get_files_int_count(source_file_name);

    if (files_int_count == -1)
        goto FAIL;

    if (files_int_count < merge_file_count){
        set_error_str_custom("create_merge_files: Merge file count cannot be higher than files int count");
        return NULL;
    }

    ssize_t int_count = (ssize_t)ceil((double)files_int_count / merge_file_count);
    
    temp_file *temp_files = create_temp_files(merge_file_count);
    if (!temp_files)
        goto FAIL;

    if (fill_files_with_sorted_ints(temp_files, source_file_name, merge_file_count, int_count))
        goto FAIL;

    return temp_files;

    FAIL:
    prepend_message_to_error_str(__func__);
    return NULL;
}

void destroy_merge_files(temp_file *temp_files, ssize_t temp_file_count)
{
    destroy_temp_files(temp_files, temp_file_count);
}



