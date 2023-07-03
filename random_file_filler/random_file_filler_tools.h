#if !defined RANDOM_FILE_FILLER_TOOLS_H
#define RANDOM_FILE_FILLER_TOOLS_H

#define SORT_VERIFICATION_FAILURE (-1)

#define randomize() (srand((unsigned int)(time(NULL))))

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

FILE *quick_data_sort(FILE *f, size_t size);
FILE *print_file(FILE *f, size_t size);
FILE *fill_file_with_random_n_size_of_int_byte_numbers(FILE *f, int n, long modulus);
FILE *swap_data(FILE *f, int offset1, int offset2);
FILE *main_sort(FILE *f, size_t size);
int file_is_sorted(FILE *f, size_t size);

#endif