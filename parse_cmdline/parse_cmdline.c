#include "parse_cmdline.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DEFAULT_ARRAY_SIZE (10)
#define DEFAULT_STR_SIZE (10)

struct dynamic_pointer_array{
    size_t size;
    size_t capacity;
    char **pointers;
};

struct dynamic_string{
    size_t size;
    size_t capacity;
    char *core_string;
};

static void swap_char(char *p1, char *p2)
{
    int temp = *p1;
    *p1++ = *p2;
    *p2++ = temp;
}

static struct dynamic_pointer_array *create_dynamic_pointer_array(void)
{
    struct dynamic_pointer_array *new = malloc(sizeof(struct dynamic_pointer_array));
    if (!new)
        return NULL;
    new -> pointers = calloc((new -> capacity = DEFAULT_ARRAY_SIZE) + 1, sizeof(char *));
    new -> size = 0;
    return new;
}

static struct dynamic_pointer_array *add_pointer(struct dynamic_pointer_array *arr, const char *pointer)
{
    if (arr -> size++ == arr -> capacity){
        char **temp = realloc(arr -> pointers, ((arr -> capacity *= 2) + 1) * sizeof(char *));
        if (!temp)
            return NULL;
        arr -> pointers = temp;
        memset(arr -> pointers + arr -> size - 1, 0, (arr -> capacity / 2 + 1) * sizeof(char *));
    }
    *(arr -> pointers + arr -> size - 1) = (char *)pointer;
    return arr;
}

static struct dynamic_string *create_dynamic_string(const char *default_string)
{
    size_t len = strlen(default_string);
    struct dynamic_string *new = malloc(sizeof(struct dynamic_string));
    if (!new)
        return NULL;
    new -> core_string = calloc((new -> capacity = 2 * len) + 1, sizeof(char));
    new -> size = len;
    strcpy(new -> core_string, default_string);
    return new;
}

static char *dynamic_string_input(struct dynamic_string *string, int ch)
{
    int c;
    *(string -> core_string + ++string -> size - 1) = '\n';
    while ((c = getchar()) != EOF){
        if (string -> size++ == string -> capacity){
            char *temp = realloc(string -> core_string, (string -> capacity * 2 + 1) * sizeof(char));
            if (!temp)
                return NULL;
            string -> core_string = temp;
            memset(string -> core_string + string -> size - 1, 0, (string -> capacity / 2 + 1) * sizeof(char));
        }
        *(string -> core_string + string -> size - 1) = c;
        if (c == ch)
            break;
    }
    return string -> core_string + string -> size - 1;
}

static void tidy_str(char *last)
{
    if (*(last + strlen(last) - 1) == '\'' || *(last + strlen(last) - 1) == '\"'){
        char *quote = strchr(last, '\'');
        char *double_quote = strchr(last, '\"');
        char *ptr = double_quote ? double_quote : quote ? quote : NULL;
        for (char *cur = ptr;cur != last;cur--)
            swap_char(cur - 1, cur);
    }
    else if(*last == '\'' || *last == '\"'){
        char *quote = strrchr(last, '\'');
        char *double_quote = strrchr(last, '\"');
        char *ptr = double_quote ? double_quote : quote ? quote : NULL;
        for (char *cur = ptr;cur != last + strlen(last) - 1;cur++)
            swap_char(cur + 1, cur);
    }
}

static char *tokenizer(struct dynamic_string *string, const char *delim, char *stop)
{
    size_t size = strlen(string -> core_string);
    for (size_t i = 0;i  < size;i++){
        if (strchr(delim, *(string -> core_string + i))){
            *(string -> core_string + i) = '\0';
            continue;
        }
        else if (strchr(stop, *(string -> core_string + i))){
            char *temp;
            if ((temp = strchr(string -> core_string + i + 1, *(string -> core_string + i)))){
                i = (size_t)(temp - string -> core_string);
                continue;
            }
            char *input;
            printf("%c counterpart missing please enter %c with optional input\n", *(string -> core_string + i), *(string -> core_string + i));
            if ((input = dynamic_string_input(string, *(string -> core_string + i)))) {
                i = (size_t)(input - string -> core_string);
                continue;
            }

            return NULL;

        }
    }
    return string -> core_string;
}


static size_t get_size_of_pointer_array(const char * const * pointer_array)
{
    size_t i;
    for (i = 0;*(pointer_array + i);i++)
        ;
    return i;
}

static char **initialize(struct dynamic_pointer_array *arr, struct dynamic_string *cmdline_dynamic_copy)
{
    if (!tokenizer(cmdline_dynamic_copy, " \n", "\"\'"))
        return NULL;
    for (size_t i = 0; i < cmdline_dynamic_copy -> size; i++){
        if (*(cmdline_dynamic_copy -> core_string + i)) {
            if (!add_pointer(arr, cmdline_dynamic_copy -> core_string + i))
                return NULL;
            tidy_str(cmdline_dynamic_copy -> core_string + i);
            for (;i < cmdline_dynamic_copy -> size; i++)
                if (!*(cmdline_dynamic_copy -> core_string + i + 1))
                    break;
        }
    }
    return arr -> pointers;
}



char **parse_cmdline(const char *cmdline)
{
    struct dynamic_pointer_array *arr = create_dynamic_pointer_array();
    if (!arr)
        return  NULL;
    struct dynamic_string *cmdline_dynamic_copy = create_dynamic_string(cmdline);
    if (!cmdline_dynamic_copy)
        return NULL;
    if (!initialize(arr, cmdline_dynamic_copy))
        return NULL;
    char **ret = calloc(arr -> size + 1, sizeof(char *));
    if (!ret)
        return NULL;
    memcpy(ret, arr -> pointers, arr -> size * sizeof(char *));
    char **temp = realloc(ret, (get_size_of_pointer_array((const char * const *)(ret)) + 1) * sizeof(char *));
    if (!temp)
        return NULL;
    ret = temp;
    return ret;
}


void print_arguments(const char * const *arguments)
{
    printf("arguments:");
    while (*arguments)
        printf("%s,", *arguments++);
    printf("\b ");
}
