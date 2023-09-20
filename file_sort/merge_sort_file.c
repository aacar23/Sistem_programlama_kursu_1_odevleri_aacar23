#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "merge_sort_file.h"
#include "create_merge_files.h"
#include "get_int_count.h"
#include "exit_sys.h"


typedef struct{
    FILE *merger_file;
    int curval;
    int eof_flag;
}merger;

#define SET_MERGER_FILE(merger_ptr, file_desc)  (((merger_ptr) -> merger_file) = (file_desc))
#define SET_MERGER_CURVAL(merger_ptr, val)      (((merger_ptr) -> curval) = (val))
#define SET_MERGER_EOF_FLAG(merger_ptr, bool)   (((merger_ptr) -> eof_flag) = (bool))

#define GET_MERGER_FILE(merger_ptr)     (((const merger *)(merger_ptr)) -> merger_file)
#define GET_MERGER_CURVAL(merger_ptr)   (((const merger *)(merger_ptr)) -> curval)
#define GET_MERGER_EOF_FLAG(merger_ptr) (((const merger *)(merger_ptr)) -> eof_flag)

#define FORWARD_MERGE_FAILURE   (1)
#define FORWARD_MERGE_SUCCESS   (0)

static int initialize_merger(merger *merger_ptr, FILE *source_file)
{
    SET_MERGER_FILE(merger_ptr, source_file);
    
    int val;
    fread(&val, sizeof(int), 1, source_file);
    if (ferror(source_file)){
        set_error_str_errno(__func__);
        return 1;
    }
    SET_MERGER_CURVAL(merger_ptr, val);

    SET_MERGER_EOF_FLAG(merger_ptr, 0);

    return  0;
}

static merger *create_merger_array(FILE * const *merge_files, size_t merge_file_count)
{
    merger *mergers = malloc(sizeof(merger) * merge_file_count);
    if (!mergers){
        set_error_str_errno(__func__);
        return NULL;
    }

    for (size_t i = 0;i < merge_file_count;++i)
        if (initialize_merger(mergers + i, *(merge_files + i))){
            prepend_message_to_error_str(__func__);
            return NULL;
        }

    return mergers;   
}

static int forward_merge(FILE *dest_file, merger *mergers, size_t merger_count)
{

    int min_val = INT_MAX;
    merger *min_merger;

    size_t eof_count = 0;

    for (size_t i = 0;i < merger_count;++i){
        if (GET_MERGER_EOF_FLAG(mergers + i)){
            ++eof_count;
            continue;
        }
        if (GET_MERGER_CURVAL(mergers + i) < min_val){
            min_val = GET_MERGER_CURVAL(mergers + i);
            min_merger = mergers + i;
        }
    }

    if (eof_count == merger_count)
        return EOF;

    fwrite(&min_val, sizeof(int), 1, dest_file);
    
    if (ferror(dest_file))
        goto FAIL;

    fread(&min_merger -> curval, sizeof(int), 1, GET_MERGER_FILE(min_merger));
    if (ferror(GET_MERGER_FILE(min_merger)))
        goto FAIL;

    if (feof(GET_MERGER_FILE(min_merger)))
        SET_MERGER_EOF_FLAG(min_merger, 1);

    return FORWARD_MERGE_SUCCESS;

    FAIL:
    set_error_str_errno(__func__);
    return FORWARD_MERGE_FAILURE;
}

static int merge_the_files(const char *dest_file_name, FILE * const *merge_files, size_t merge_file_count)
{
    void (*set_error_str)(const char *additional_message) = &prepend_message_to_error_str;
    FILE *dest_file = fopen(dest_file_name, "rb+");
    if (!dest_file){
        set_error_str = &set_error_str_errno;
        goto FAIL_LEVEL_1;
    }

    merger *mergers = create_merger_array(merge_files, merge_file_count);
    if (!mergers)
        goto FAIL_LEVEL_2;

    for (int res;(res = forward_merge(dest_file, mergers, merge_file_count)) != EOF;)
        if (res == FORWARD_MERGE_FAILURE)
            goto FAIL_LEVEL_3;

    set_error_str = &set_error_str_success;

    FAIL_LEVEL_3:
    free(mergers);
    FAIL_LEVEL_2:
    fclose(dest_file);
    FAIL_LEVEL_1:
    (*set_error_str)(__func__);
    return set_error_str != set_error_str_success;
}


int merge_sort_file(const char *dest_file_name, size_t merge_file_count)
{
    int failed = 1;

    size_t dest_int_count;

    if (get_int_count(dest_file_name, &dest_int_count))
        goto FAIL_LEVEL_1;

    if (dest_int_count < merge_file_count){
        set_error_str_custom(__func__, "Cannot merge with file count more than destination files integer count\n");
        return 1;
    }

    FILE **merge_files = create_merge_files(dest_file_name, merge_file_count);
    if (!merge_files)
        goto FAIL_LEVEL_1;

    int res = merge_the_files(dest_file_name, (FILE * const *)merge_files, merge_file_count);

    if (res)
        goto FAIL_LEVEL_2;

    failed = 0;

    FAIL_LEVEL_2:
    destroy_merge_files(merge_files, merge_file_count);
    FAIL_LEVEL_1:
    if (failed)
        prepend_message_to_error_str(__func__);
    return failed;
}