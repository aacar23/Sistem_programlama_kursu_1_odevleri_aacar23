#include <stdlib.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>

#include "merge_sort_file.h"
#include "create_merge_files.h"
#include "exit_sys.h"


typedef struct{
    file_descriptor file;
    int cur_val;
    int eof_flag;
}merger;

#define END_OF_FILE (-1)

static merger *create_mergers(const temp_file *merge_files, ssize_t merge_file_count)
{
    merger *mergers = malloc(sizeof(merger) * merge_file_count);
    if (!mergers)
        goto FAIL_LEVEL_1;

    for (ssize_t i = 0;i < merge_file_count;++i){
        (mergers + i) -> file = (merge_files + i) -> file;
        int val;
        int read_bytes = read((mergers + i) -> file, &val, sizeof(int));
        if (read_bytes == -1)
            goto FAIL_LEVEL_2;
        (mergers + i) -> cur_val = val;
        (mergers + i) -> eof_flag = read_bytes < (ssize_t)sizeof(int);
    }


    return mergers;

    FAIL_LEVEL_2:
    free(mergers);
    FAIL_LEVEL_1:
    set_error_str_errno(__func__);
    return NULL;
}

static int iterate_merge(file_descriptor dest_file, merger *mergers, ssize_t merger_count)
{
    int min_val = INT_MAX;
    merger *min_merger;
    ssize_t eof_count = 0;

    for (ssize_t i = 0;i < merger_count;++i){
        if ((mergers + i) -> eof_flag)
           ++eof_count;
        else if((mergers + i) -> cur_val < min_val){
            min_val = (mergers + i) -> cur_val;
            min_merger = mergers + i;
        }
    }

    if (eof_count == merger_count)
        return END_OF_FILE;

    if (write(dest_file, &min_val, sizeof(int)) == -1)
        goto FAIL;

    int read_bytes = read((min_merger) -> file, &((min_merger) -> cur_val), sizeof(int));
    if (read_bytes == -1)
        goto FAIL;
    
    if (read_bytes < (ssize_t)sizeof(int))
        (min_merger) -> eof_flag = 1;

    return 0;

    FAIL:
    set_error_str_errno(__func__);
    return 1;
}

static int merge_runner(const char *dest_file_name, merger *mergers, ssize_t merger_count)
{
    int failure = 0;

    file_descriptor dest_file = open(dest_file_name, O_WRONLY);
    if (dest_file == -1){
        set_error_str_errno(__func__);
        return 1;
    }

    for (int iterate_res;(iterate_res = iterate_merge(dest_file, mergers, merger_count)) != END_OF_FILE;){
        if (iterate_res == 1){
            failure = 1;
            break;
        }
    }

    close(dest_file);
    prepend_message_to_error_str(__func__);
    return failure;
}

int merge_sort_file(const char *dest_file_name, ssize_t merge_file_count)
{
    int failure = 1;
    temp_file *temp_files = create_merge_files(dest_file_name, merge_file_count);
    if (!temp_files)
        goto FAIL_LEVEL_1;

    merger *mergers = create_mergers(temp_files, merge_file_count);
    if (!mergers)
        goto FAIL_LEVEL_2;

    if (merge_runner(dest_file_name, mergers, merge_file_count))
        goto FAIL_LEVEL_3;

    failure = 0;

    FAIL_LEVEL_3:
    free(mergers);
    FAIL_LEVEL_2:
    destroy_merge_files(temp_files, merge_file_count);
    FAIL_LEVEL_1:
    if (failure)
        prepend_message_to_error_str(__func__);
    return failure;
}


