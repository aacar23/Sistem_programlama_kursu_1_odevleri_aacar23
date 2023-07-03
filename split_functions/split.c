#include "split.h"

#include <string.h>
#include <stdlib.h>

static char **split_core(char *dest, const char *source_pointer, const char *delimeters)
{
	char **pointer_arr = calloc(strlen(dest) + 1, sizeof(char *));
	char *point = strtok(dest, delimeters);
	

	if (!pointer_arr)
		return NULL;
		
	
	int i;
	for (i = 0;point;point = strtok(NULL, delimeters))
		*(pointer_arr + i++) = point - dest + (char *)source_pointer;
		
	char **reallocation = realloc(pointer_arr, sizeof(char *) * (i + 1));
	
	if (!reallocation)
		return NULL;
		
	*(reallocation + i) = NULL;
	
	return pointer_arr = reallocation;
}

char **split(char *dest, const char *delimeters)
{
	return split_core(dest, dest, delimeters);
}

char **split2(const char *source, const char *delimeters)
{
	char *source_copy = malloc(strlen(source) + 1);
	strcpy(source_copy, source);
	char **result = split_core(source_copy, source, delimeters);
	free(source_copy);
	
	return result;
}


	
