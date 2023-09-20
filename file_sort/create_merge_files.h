#if !defined CREATE_MERGE_FILES_H
#define CREATE_MERGE_FILES_H

#include <stdio.h>

FILE **create_merge_files(const char *source_file_name, size_t merge_file_count);
void destroy_merge_files(FILE **merge_files, size_t merge_file_count);

#endif