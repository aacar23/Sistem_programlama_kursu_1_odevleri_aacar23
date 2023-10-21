#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <inttypes.h>
#include <grp.h>

#include "mygrp.h"


static char cur_group_line[PATH_MAX + 1] = {0};
static struct group cur_group;

FILE *grp_file;

static char **initialize_member_list(char **dest_member_list, char *members)
{
    if (!members)
        return dest_member_list;
    


    int idx = 0;
    for (const char *cur_str = strtok(members, ",");cur_str;cur_str = strtok(NULL, ","))
        *(dest_member_list + idx++) = (char *)cur_str;


    return dest_member_list;
}

static struct group *initialize_cur_grp(void)
{

    static char *member_list[1000] = {0};

    for (size_t i = 0;*(member_list + i);++i)
        *(member_list + i) = NULL;

    cur_group.gr_name = strtok(cur_group_line, ":");
    cur_group.gr_passwd = strtok(NULL, ":");
    cur_group.gr_gid = strtoumax(strtok(NULL, ":"), NULL, 10);
    cur_group.gr_mem = initialize_member_list(member_list, strtok(NULL, ":"));

    return &cur_group;
}

static struct group *iterate_etc_group_file_lines(void)
{
    if (!fgets(cur_group_line, PATH_MAX, grp_file))
        return NULL;

    *strchr(cur_group_line, '\n') = '\0';

    return initialize_cur_grp();
}

struct group *csd_getgrnam(const char *name)
{

    if (!grp_file)
        csd_setgrent();

    struct group *result;

    int cmp_res = 1;

    for (;(result = iterate_etc_group_file_lines()) && (cmp_res = strcmp(result -> gr_name, name));)
        ;

    return !cmp_res ? result : NULL;
}

struct group *csd_getgrgid(gid_t gid_val)
{
    if (!grp_file)
        csd_setgrent();

    struct group *result;

    int cmp_res = 0;

    for (;(result = iterate_etc_group_file_lines()) && !(cmp_res = result -> gr_gid == gid_val);)
        ;

    return cmp_res ? result : NULL;
}

void csd_setgrent(void)
{
    if (!(grp_file = fopen("/etc/group", "r")))
        exit(EXIT_FAILURE);
}

struct group *csd_getgrent(void)
{
    return iterate_etc_group_file_lines();
}


void csd_endgrpent(void)
{
    fclose(grp_file);
    grp_file = NULL;
}