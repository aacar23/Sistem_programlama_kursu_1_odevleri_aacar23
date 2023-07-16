#include "print_all/print_all.h"

#include <stdio.h>
#include <getopt.h>

#define HELP (1)
#define VERSION (2)

#define NUMBER_OF_OPTIONS (6)


static int all_zeros(const int *flags)
{
    for (int i = 0;i < NUMBER_OF_OPTIONS;i++)
        if (*(flags + i))
            return 0;
    return 1;
}


int main(int argc, char **argv)
{
    opterr = 0;
    struct option options[] = { {"lines", no_argument, NULL, 'l'},
                                {"words", no_argument, NULL, 'w'},
                                {"bytes", no_argument, NULL, 'c'},
                                {"max-line-length", no_argument, NULL, 'L'},
                                {"help", no_argument, NULL, HELP},
                                {"version", no_argument, NULL, VERSION},
                                {0, 0, 0, 0}
                              };
    
    int flags[NUMBER_OF_OPTIONS] = {0};
    long sums[4] = {0};
    size_t precisions[4] = {0};

    for (int i = getopt_long(argc, argv, "lwcL", options, NULL);i != -1;i = getopt_long(argc, argv, "lwcL", options, NULL)){
        switch(i){
            case 'l':
            ++LINES_FLAG;
            break;
            case 'w':
            ++WORDS_FLAG;
            break;
            case 'c':
            ++BYTES_FLAG;
            break;
            case 'L':
            ++MAX_LINE_LENGTH_FLAG;
            break;
            case HELP:
            ++HELP_FLAG;
            break;
            case VERSION:
            ++VERSION_FLAG;
            break;
            case '?':
            fprintf(stderr, "Invalid option\n");
            return 1;
        }
    }

    if (all_zeros(flags))
        LINES_FLAG = WORDS_FLAG = BYTES_FLAG = 1;

    return print_all(argc, (const char * const *)argv, optind, flags, sums, precisions);
}