#if !defined GET_FILES_INT_COUNT_H
#define GET_FILES_INT_COUNT_H

#include <sys/types.h>

typedef int file_descriptor;

ssize_t get_files_int_count(const char *file_name);

#endif