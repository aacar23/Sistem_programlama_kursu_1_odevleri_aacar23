#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "merge_sort_file.h"
#include "exit_sys.h"

int main(int argc, char **argv)
{
    ssize_t merge_file_count = 10;
    int base = 0;

    opterr = 0;
    optarg = NULL;

    struct option options[4] = {
        {.name = "dec-merge-file-count", .has_arg = required_argument, .flag = &base, .val = 10},
        {.name = "oct-merge-file-count", .has_arg = required_argument, .flag = &base, .val = 8},
        {.name = "hex-merge-file-count", .has_arg = required_argument, .flag = &base, .val = 16}
    };

    for (int c;(c = getopt_long(argc, argv, "", options, NULL)) != -1;){
        if (base)
            break;

        if (c == '?'){
            fprintf(stderr, "Invalid argument or no argument has been given for an option\n");
            return 1;
        }
    }


    if (optarg){
        char *end;
        merge_file_count = strtoll(optarg, &end, base);
        if (end == optarg || *end != '\0'){
            fprintf(stderr, "Please enter the number in base-%d", base);
            return 1;
        }
    }

    for (int i = optind;i < argc;++i)
        if (merge_sort_file(*(argv + i), merge_file_count))
            exit_sys();

    return 0;
}