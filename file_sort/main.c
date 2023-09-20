#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>

#include "merge_sort_file.h"
#include "exit_sys.h"

static int isodigit(int ch)
{
    return ch >= '0' && ch <= '8';
}

static int is_int(const char *str, int (*char_test_func)(int ch))
{
    for (;*str;++str)
        if (!(*char_test_func)(*str))
            return 0;

    return 1;
}

int main(int argc, char **argv)
{
    struct option options[4] = {
        {.name = "dec-merge-file-count", .has_arg = required_argument, .flag = NULL, .val = 'd'},
        {.name = "oct-merge-file-count", .has_arg = required_argument, .flag = NULL, .val = 'o'},
        {.name = "hex-merge-file-count", .has_arg = required_argument, .flag = NULL, .val = 'x'},
    };

    opterr = 0;
    optarg = NULL;

    int (*char_test_func)(int ch) =&isdigit;

    for (int c;(c = getopt_long(argc, argv, "d:o:x:", options, NULL)) != -1;)
        switch (c){
            case 'd':
            goto OUT;
            case 'o':
            char_test_func = &isodigit;
            goto OUT;
            case 'x':
            char_test_func = &isxdigit;
            goto OUT;
            case '?':
            if (strchr("d:o:x:", optopt))
                fprintf(stderr, "No argument has been entered for option %c", optopt);
            else
                fprintf(stderr, "Invalid option %c", optopt);
            return 1;
        }

    int merge_file_count = 2;

    OUT:

    if (optarg){
        if (is_int(optarg, char_test_func))
            merge_file_count = atoi(optarg);
        else{
            fprintf(stderr, "Invalid argument %s\n", optarg);
            return 1;
        }
    }


    

    for (int i = optind;i < argc;++i)
        if (merge_sort_file(*(argv + i), merge_file_count))
            exit_sys();

    return 0;
}