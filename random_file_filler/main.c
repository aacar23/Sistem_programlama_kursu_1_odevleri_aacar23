#include "random_file_filler_tools.h"

#include <limits.h>

#define FILE_SIZE (1000000)
#define MOD (2147483648L)

int main(void)
{
    randomize();

    FILE *f = fopen("random.dat", "wb+");
    if (!fill_file_with_random_n_size_of_int_byte_numbers(f, FILE_SIZE, MOD))
        exit(EXIT_FAILURE);
    printf("files numbers:\n");
    if (!print_file(f, FILE_SIZE))
        exit(EXIT_FAILURE);
    if (!main_sort(f, FILE_SIZE))
        exit(EXIT_FAILURE);
    printf("\n\n\n");
    int ver;
    if ((ver = file_is_sorted(f, FILE_SIZE)))
        printf("sorted successfully\n");
    else if((ver = SORT_VERIFICATION_FAILURE))
        printf("cannot verifiy\n");
    else
        printf("try to code the program again dummy\n");
    printf("\n\n\n");
    printf("files numbers sorted:\n");
    if (!print_file(f, FILE_SIZE))
        exit(EXIT_FAILURE);
}
