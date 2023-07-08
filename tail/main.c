#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <ctype.h>

#define TO_INT_FAILURE (LONG_MIN)

#define PROCESS_FAILURE (1)
#define PROCESS_SUCCESS (0)

#define PRINT_FAILURE (1)
#define PRINT_SUCCESS (0)

#define C_FLAG (0)
#define N_FLAG (1)

static long to_int(const char *arg)
{
    char *end;
    int val = strtol(arg, &end, 10);
    if (end == arg || *end != '\0')
        return TO_INT_FAILURE;
    return val;
}

static int print_bytes(const char *file_name, int n)
{
    FILE *f = fopen(file_name, "rb");
    if (!f){
        fprintf(stderr, "file \"%s\" not found or couldn't be opened", file_name);
        return PRINT_FAILURE;
    }
    fseek(f, 0, SEEK_END);
    long end = ftell(f) - n;
    if (end < 0)
        end = 0;
    fseek(f, end, SEEK_SET);
    for (int c;(c = fgetc(f)) != EOF;){
        putchar(c);
    }
    if (ferror(f))
        return PRINT_FAILURE;
    fclose(f);
    return PRINT_SUCCESS;
}

static int print_lines(const char *file_name, int n)
{
    FILE *f = fopen(file_name, "rb");
    if (!f){
        fprintf(stderr, "file \"%s\" not found or couldn't be opened", file_name);
        return PRINT_FAILURE;
    }
    long start = ftell(f);
    fseek(f, -2, SEEK_END);
    int c;
    for (int i = 0;i < n;i++){
        for (;ftell(f) != start && (c = fgetc(f)) != EOF && c != '\n';fseek(f, -2, SEEK_CUR))
            ;
        if (ftell(f) == start)
            goto out;
        fseek(f, -2, SEEK_CUR);
    }
    out:
    if (ftell(f) != start)
        fseek(f, 2, SEEK_CUR);
    for (;(c = fgetc(f)) != EOF;putchar(c))
        ;
    if (ferror(f))
        return PRINT_FAILURE;
    return PRINT_SUCCESS;
}

static int process_file(const char *file_name, int print_flag, int n, int v_flag)
{
    if (v_flag == 1)
        printf("==>%s<==\n", file_name);
    if (print_flag == N_FLAG)
        return print_lines(file_name, n);
    else if (print_flag == C_FLAG)
        return print_bytes(file_name, n);
    return PROCESS_FAILURE;
}

int main(int argc, char **argv)
{
    int c_flag = 0, n_flag = 0, v_flag = 0;
    char *n_arg = NULL;
    int n = 10;
    opterr = 0;
    for (int option;(option = getopt(argc, argv, "c:n:v")) != EOF;)
        switch (option){
            case 'c':
            c_flag++;
            n_arg = optarg;
            break;
            case 'n':
            n_flag++;
            n_arg = optarg;
            break;
            case 'v':
            v_flag++;
            break;
            case '?':
            if (optopt == 'c' || optopt == 'n')
                fprintf(stderr, "no argument has been given for option -%c\n", optopt);
            else
                fprintf(stderr, "invalid option\n");
            return 1;
        }
    if (c_flag > 1 || n_flag > 1 || v_flag > 1)
        return 1;
    if (c_flag + n_flag + v_flag > 2 || c_flag + n_flag > 1)
        return 1;
    if (n_arg){
        long temp;
        if ((temp = to_int(n_arg)) == TO_INT_FAILURE){
            fprintf(stderr, "please enter an integer as an argument\n");
            return 1;
        }
        n = (int)temp;
    }
    for (int i = optind;i < argc;i++)
        if (process_file(*(argv + i), c_flag - n_flag == 1 ? C_FLAG : N_FLAG, n, v_flag) == PROCESS_FAILURE)
            return 1;
    return 0;
}
