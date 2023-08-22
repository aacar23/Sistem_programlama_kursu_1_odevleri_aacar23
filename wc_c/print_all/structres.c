#include "structres.h"

#include <stdlib.h>
#include <string.h>

#define DEFAULT_CAPACITY (10)

struct counts *new_counts(const char *file_name)
{
    struct counts *new = (struct counts *)malloc(sizeof(struct counts));
    if (!new)
        return NULL;
    memset(new, 0, sizeof(struct counts));
    FILE *temp = fopen(file_name, "rb");
    if (!temp)
        return NULL;
    new -> f = temp;
    return new;
}

void free_counts(struct counts *counts_obj)
{
    fclose(counts_obj -> f);
    free(counts_obj);
}

struct counts_list *new_counts_list(void)
{
    struct counts_list *new = (struct counts_list *)malloc(sizeof(struct counts_list));
    if (!new)
        return NULL;
    struct counts **temp = (struct counts **)malloc(sizeof(struct counts *) * (new -> capacity = DEFAULT_CAPACITY));
    if (!temp)
        return NULL;
    new -> list = temp;
    new -> size = 0;
    return new;
}

struct counts_list *add_file(struct counts_list *count_set_list, const struct counts *count_set)
{
    if (count_set_list -> size == count_set_list -> capacity){
        struct counts **temp = (struct counts **)realloc(count_set_list -> list, sizeof(struct counts *) * (count_set_list -> capacity *= 2));
        if (!temp)
            return NULL;
        count_set_list -> list = temp;
    }
    *(count_set_list -> list + ++count_set_list -> size - 1) = (struct counts *)count_set;
    return count_set_list;
}

void free_counts_list(struct counts_list *counts_list_obj)
{
    for (size_t i = 0;i < counts_list_obj -> size;++i)
        free_counts(*(counts_list_obj -> list + i));
    free(counts_list_obj);
}