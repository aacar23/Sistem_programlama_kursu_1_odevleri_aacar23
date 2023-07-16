#if !defined RUNNER_FUNCTIONS_H
#define RUNNER_FUNCTIONS_H

#define LINES_FLAG (*flags)
#define WORDS_FLAG (*(flags + 1))
#define BYTES_FLAG  (*(flags + 2))
#define MAX_LINE_LENGTH_FLAG (*(flags + 3))
#define HELP_FLAG (*(flags + 4))
#define VERSION_FLAG (*(flags + 5))

#include "structres.h"

int print_all(int argc, const char * const *argv, int index_of_optionless_arguments, const int *flags, long *sums, size_t *precisions);

#endif