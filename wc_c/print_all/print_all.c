#include "print_all.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

#define LINES_SUM (*sums)
#define WORDS_SUM (*(sums + 1))
#define BYTES_SUM (*(sums + 2))
#define MAX_LINE_LENGTH_SUM (*(sums + 3))

#define LINES_PRECISION (*precisions)
#define WORDS_PRECISION (*(precisions + 1))
#define BYTES_PRECISION (*(precisions + 2))
#define MAX_LINE_LENGTH_PRECISION (*(precisions + 3))

static size_t num_len(long num)
{
    size_t len = 1;
    while (num /= 10)
        len++;
    return len;
}

static struct counts *set_words_count(struct counts *count_set)
{
    
    FILE *file = count_set -> f;
    fseek(file, 0, SEEK_SET);

    for (int ch = fgetc(file);ch != EOF;ch = fgetc(file)){
        if (!isspace(ch))
            ++WORDS_VALUE(count_set);
        while (ch != EOF && !isspace(ch = fgetc(file)))
            ;
        while (ch != EOF && isspace(ch = fgetc(file)))
            ;
        if (ch != EOF)
            fseek(file, -1, SEEK_CUR);
    }

    if (ferror(file))
        return NULL;

    return count_set;
}

static struct counts *set_counts(struct counts *count_set)
{
    
    FILE *file = count_set -> f;
    fseek(file, 0, SEEK_SET);

    long temp_count = 0;
    for (int ch;(ch = fgetc(file)) != EOF;++BYTES_VALUE(count_set)){
        ++temp_count;
        if (ch == '\n' || ch == '\r'){
            if (temp_count > MAX_LINE_LENGTH_VALUE(count_set))
                MAX_LINE_LENGTH_VALUE(count_set) = temp_count - 1;
            temp_count = 0;
            ++LINES_VALUE(count_set);
        }
    }
    

    if (ferror(file))
        return NULL;

    return count_set;
}

static void set_sums(long *sums, const struct counts_list *count_set_list)
{
    for (size_t i = 0;i < 3;++i)
        for (size_t t = 0;t < count_set_list -> size;++t)
            *(sums + i) += NTH_VALUE(NTH_COUNT_SET(count_set_list, t), i);
    for (size_t t = 0;t < count_set_list -> size;++t)
            if (MAX_LINE_LENGTH_VALUE(NTH_COUNT_SET(count_set_list, t)) > *(sums + 3))
                MAX_LINE_LENGTH_SUM = MAX_LINE_LENGTH_VALUE(NTH_COUNT_SET(count_set_list, t));
}

static void set_precisions(size_t *precisions, const long *sums)
{
    for (size_t i = 0;i < 4;++i)
        *(precisions + i) = num_len(*(sums + i));
}


static int power(int base, int times)
{
    int res = 1;
    while (times--)
        res *= base;
    return res;
}

static void print_value(long value, size_t precision)
{
    size_t len = num_len(value);
    int multiply = power(10, (int)len);
    for (size_t i = 0;i < precision - len;++i)
        putchar(' ');
    do{
        printf("%ld", value % multiply / (multiply / 10));
    }while ((multiply /= 10) != 1);
    putchar(' ');
}

static size_t get_max(const size_t *precisions)
{
    size_t max = 0;
    if (LINES_PRECISION > max)
        max = LINES_PRECISION;
    if (WORDS_PRECISION > max)
        max = WORDS_PRECISION;
    if (BYTES_PRECISION > max)
        max = BYTES_PRECISION;
    if (MAX_LINE_LENGTH_PRECISION > max)
        max = MAX_LINE_LENGTH_PRECISION;
    return max;
}

static size_t get_max_all_one(const size_t *precisions, const int *flags)
{
    size_t max = 0;
    if (LINES_FLAG && LINES_PRECISION > max)
        max = LINES_PRECISION;
    if (WORDS_FLAG && WORDS_PRECISION > max)
        max = WORDS_PRECISION;
    if (BYTES_FLAG && BYTES_PRECISION > max)
        max = BYTES_PRECISION;
    if (MAX_LINE_LENGTH_FLAG && MAX_LINE_LENGTH_PRECISION > max)
        max = MAX_LINE_LENGTH_PRECISION;
    return max;
}

static void print_count_set(const struct counts *count_set, const int *flags, size_t max)
{
    if (LINES_FLAG)
        print_value(LINES_VALUE(count_set), max);
    if (WORDS_FLAG)
        print_value(WORDS_VALUE(count_set), max);
    if (BYTES_FLAG)
        print_value(BYTES_VALUE(count_set), max);
    if (MAX_LINE_LENGTH_FLAG)
        print_value(MAX_LINE_LENGTH_VALUE(count_set), max);
    
}

static void print_sums(const long *sums, const int *flags, size_t max)
{
    if (LINES_FLAG)
        print_value(LINES_SUM, max);
    if (WORDS_FLAG)
        print_value(WORDS_SUM, max);
    if (BYTES_FLAG)
        print_value(BYTES_SUM, max);
    if (MAX_LINE_LENGTH_FLAG)
        print_value(MAX_LINE_LENGTH_SUM, max);
    printf(" total\n");
}

int print_all(int argc, const char * const *argv, int index_of_optionless_arguments, const int *flags, long *sums, size_t *precisions)
{

    if (HELP_FLAG){
        if (system("cat help.txt"))
            return 1;
        return 0;
    }
    if (VERSION_FLAG){
        printf("v1.00\n");
        return 0;
    }

     if (index_of_optionless_arguments == argc){
        printf("No argument has been given\n");
        return 0;
    }
    
    struct counts_list *count_set_list = new_counts_list();

    for (int i = index_of_optionless_arguments;i < argc;++i){
        struct counts *current = new_counts(*(argv + i));
        if (!current){
            if (errno = EINVAL)
                printf("No such file or directory\n");
            return 1;
        }
        if (!set_counts(current))
            return 1;
        if (WORDS_FLAG)
            if (!set_words_count(current))
                return 1;
        if (!add_file(count_set_list, current))
            return 1;
        
    }

    set_sums(sums, count_set_list);

    

    set_precisions(precisions, sums);

    size_t max;
    if (argv + index_of_optionless_arguments == argv + argc){
        max = get_max_all_one(precisions, flags);
        goto out;
    }

    max = get_max(precisions);

    out:

    for (size_t i = 0;i < count_set_list -> size;++i){
        print_count_set(*(count_set_list -> list + i), flags, max);
        printf("%s\n", *(argv + i + index_of_optionless_arguments));
    }

    if (argv + index_of_optionless_arguments < argv + argc - 1)
        print_sums(sums, flags, max);

    return 0;
}
