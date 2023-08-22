#if !defined STRUCTRES_H
#define STRUCTRES_H

#include <stdio.h>

#define NTH_VALUE(obj, n) (*((obj) -> values + n))
#define LINES_VALUE(obj) (*((obj) -> values))
#define WORDS_VALUE(obj) (*((obj) -> values + 1))
#define BYTES_VALUE(obj) (*((obj) -> values + 2))
#define MAX_LINE_LENGTH_VALUE(obj) (*((obj) -> values + 3))

#define NTH_COUNT_SET(obj_list, index) (*((obj_list) -> list + index))
#define COUNTS_LIST_SIZE(obj_list) (*(obj_list) -> size))
#define COUNTS_LIST_CAPACITY(obj_list) (*(obj_list) -> capacity)

struct counts{
    FILE *f;
    long values[4];
};

struct counts_list{
    struct counts **list;
    size_t size;
    size_t capacity;
};

struct counts *new_counts(const char *file_name);
void free_counts(struct counts *counts_obj);

struct counts_list *new_counts_list(void);
struct counts_list *add_file(struct counts_list *count_set_list, const struct counts *count_set);
void free_counts_list(struct counts_list *counts_list_obj);

#endif