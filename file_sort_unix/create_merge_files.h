#if !defined CREATE_MERGE_FILES_H
#define CREATE_MERGE_FILES_H

#include <sys/types.h>

typedef int file_descriptor;

typedef struct{
    file_descriptor file;
    char file_name[15];
}temp_file;

temp_file *create_merge_files(const char *source_file_name, ssize_t merge_file_count);
void destroy_merge_files(temp_file *temp_files, ssize_t temp_file_count);

#endif

