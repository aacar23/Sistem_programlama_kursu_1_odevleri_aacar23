#include "random_file_filler_tools.h"
#include <limits.h>

#define PIVOT_FAILIURE (-22)

FILE *swap_data(FILE *f, int offset1, int offset2)
{
    int data1;
    int data2;
    
    if (fseek(f, sizeof(int) * offset1, SEEK_SET))
        return NULL;
    fread(&data1, sizeof(int), 1, f);
    if (fseek(f, sizeof(int) * offset2, SEEK_SET))
        return NULL;
    fread(&data2, sizeof(int), 1, f);
    if(fseek(f, sizeof(int) * offset1, SEEK_SET))
        return NULL;
    fwrite(&data2, sizeof(int), 1, f);
    if(fseek(f, sizeof(int) * offset2, SEEK_SET))
        return NULL;
    fwrite(&data1, sizeof(int), 1, f);
    
    
    if (ferror(f))
        return NULL;
    return f;
}

static int pivot_init_and_partial_sort(FILE *f, int start, int end)
{
    int end_value;
    if (fseek(f, sizeof(int) * end, SEEK_SET))
        return PIVOT_FAILIURE;
    if (feof(f))
        return PIVOT_FAILIURE;
    fread(&end_value, sizeof(int), 1, f);
    int pivot = start;
    while(start < end){
        int start_value;
        if (fseek(f, sizeof(int) * start, SEEK_SET))
            return PIVOT_FAILIURE;
        fread(&start_value, sizeof(int), 1, f);
        if (start_value < end_value){
            if (!swap_data(f, start, pivot++))
                return PIVOT_FAILIURE;
        }
        start++;
    }
    
    if (!swap_data(f, pivot, end))
        return PIVOT_FAILIURE;

    if (ferror(f))
        return PIVOT_FAILIURE;

    return pivot;
}

static FILE *quick_data_sort_core(FILE *f, int start, int end)
{
    if (start < end){
        int pivot = pivot_init_and_partial_sort(f, start, end);
        if (pivot == PIVOT_FAILIURE)
            return NULL;
        if (!quick_data_sort_core(f, start, pivot - 1))
            return NULL;
        if (!quick_data_sort_core(f, pivot + 1, end))
            return NULL;
        if (ferror(f))
            return NULL;
        return f;
    }
    return f;
}

FILE *quick_data_sort(FILE *f, size_t size){
    return quick_data_sort_core(f, 0, size - 1);
}

FILE *main_sort(FILE *f, size_t size)
{
    size /= 10;
    FILE *temp_files[10];

    for (size_t i = 0;i < 10;i++){
        temp_files[i] = tmpfile();

        for (size_t t = 0;t < size;t++){
            if (fseek(f, sizeof(int) * (t + i * size), SEEK_SET))
                return NULL;

            int number;
            fread(&number, sizeof(int), 1, f);
            fwrite(&number, sizeof(int), 1, temp_files[i]);
        }

        quick_data_sort(temp_files[i], size);
    }

    int idx[10] = {0};
    for (size_t t = 0;t < size * 10;t++){
        long cmp[10];
        
        for (size_t n = 0;n < 10;n++){
            if ((size_t)idx[n] >= size){
                cmp[n] = LONG_MAX;
                continue;
            }

            if (fseek(temp_files[n], sizeof(int) * idx[n], SEEK_SET))
                return NULL;

            int num;
            fread(&num, sizeof(int), 1, temp_files[n]);
            cmp[n] = num;
        }

        int min = 0;

        for (size_t n = 1;n < 10;n++)
            if (cmp[min] > cmp[n])
                min = n;

        if (fseek(f, sizeof(int) * t, SEEK_SET))
            return NULL;
        if (fseek(temp_files[min], sizeof(int) * idx[min], SEEK_SET))
            return NULL;

        int num;
        fread(&num, sizeof(int), 1, temp_files[min]);
        fwrite(&num, sizeof(int), 1, f);
        idx[min]++;
    }

    if (ferror(f))
        return NULL;

    return f;


}

FILE *print_file(FILE *f, size_t size)
{
    rewind(f);

    for (int a, i = 1;size--;i++){
        fread(&a, sizeof(int), 1, f);
        printf("%d\n", a);

        if (fseek(f, sizeof(int) * i, SEEK_SET))
            return NULL;
    }

    if (ferror(f))
        return NULL;

    return f;
}

FILE *fill_file_with_random_n_size_of_int_byte_numbers(FILE *f, int n, long modulus)
{
    for (int a, i = 0;i < n;){
        if (fseek(f, sizeof(int) * i++, SEEK_SET))
            return NULL;

        a = (int)(rand() % modulus);
        fwrite(&a, sizeof(int), 1, f);
    }

    if (ferror(f))
        return NULL;

    return f;
}

int file_is_sorted(FILE *f, size_t size)
{
    if (fseek(f, sizeof(int), SEEK_SET))
        return SORT_VERIFICATION_FAILURE;

    int prev = 0;
    
    for (size_t i = 0;i < size;i++){
        int next;
        if (fseek(f, sizeof(int), SEEK_CUR))
            return SORT_VERIFICATION_FAILURE;

        fread(&next, sizeof(int), 1, f);

        if (prev > next)
            return 0;

        prev = next;
    }

    if (ferror(f))
        return SORT_VERIFICATION_FAILURE;

    return 1;
}

