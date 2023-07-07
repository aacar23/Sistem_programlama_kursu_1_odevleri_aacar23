#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>

#define TO_INT_FAILURE (LONG_MIN)

#define C_FLAG (0)
#define N_FLAG (1)

#define PROCESS_FAILURE (1)
#define PROCESS_SUCCESS (0)

static long to_int(const char *n_arg)
{
    char *end;
    int val = strtol(n_arg, &end, 10);
    if (end == n_arg || *end != '\0')
        return TO_INT_FAILURE;
    return val;
}

static void print_bytes(FILE *f, int n)
{
    for (int bytes = 0;!feof(f) && bytes < n;bytes++)
        putchar(fgetc(f));
}

static void print_lines(FILE *f, int n)
{
    for (int lines = 0;lines < n;lines++)
        for (int c;(c = fgetc(f)) != EOF && putchar(c) != '\n';)
            ;
}

static int process_file(const char *file_name, int print_flag, int n, int v_flag)
{
    FILE *f = fopen(file_name, "rb");
    if (!f){
        fprintf(stderr, "failed to open or find the file \"%s\"\n", file_name);
        return PROCESS_FAILURE;
    }
    if (v_flag)
        printf("==>%s<==\n", file_name);
    if (print_flag == N_FLAG)
        print_lines(f, n);
    else if (print_flag == C_FLAG)
        print_bytes(f, n);
    else
        return PROCESS_FAILURE;
    if (ferror(f))
        return PROCESS_FAILURE;
    fclose(f);
    return PROCESS_SUCCESS;
}

int main(int argc, char **argv)
{
    opterr = 0;
    int c_flag = 0, n_flag = 0, v_flag = 0;
    const char *n_arg = NULL;
    int n = 10;
    for (int option;(option = getopt(argc, argv, "c:n:v")) != -1;)
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
                fprintf(stderr, "no argument has been entered for option -%c\n", optopt);
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
        if ((temp = to_int(n_arg)) == TO_INT_FAILURE)
            return 1;
        n = (int)temp;
    }
    for (int index = optind;index < argc;index++)
        if (process_file(*(argv + index), c_flag - n_flag == 1 ? C_FLAG : N_FLAG, n, v_flag))
            return 1;
    return 0;
}
