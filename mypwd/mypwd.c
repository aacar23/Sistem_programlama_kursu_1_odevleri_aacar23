#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <inttypes.h>
#include <pwd.h>
#include <errno.h>

#include "mypwd.h"

#if !defined IMP_UTILITES_H
#define IMP_UTILITES_H

#define define_inttypetostr(type)  char *type##tostr(type val)                                                              \
                                   {                                                                                        \
                                        size_t val_size = sizeof(type);                                                  \
                                        static char conv_res[sizeof(type) / 4 + 1] = {0};                      \
                                        memset(conv_res, 0, strlen(conv_res));                                              \
                                        static char hexchars[16] = "0123456789ABCDEF";                                      \
                                                                                                                            \
                                                                                                                            \
                                                                                                                            \
                                        for (size_t shift_val = val_size, conv_res_idx = 0;shift_val -= 4;++conv_res_idx)   \
                                            *(conv_res + conv_res_idx) = *(hexchars + (val & (15 << shift_val)));           \
                                                                                                                            \
                                        return conv_res;                                                                    \
                                   }

#endif

static char cur_passwd_line[PATH_MAX + 1] = {0};
static struct passwd cur_usr;
static FILE *passwd_file;

struct pseudo_passwd{
    const char *pspw_name;
    const char *pspw_passwd;
    const char *pspw_uid;
    const char *pspw_gid;
    const char *pspw_gecos;
    const char *pspw_dir;
    const char *pspw_shell;
};

static struct pseudo_passwd token_res;

#define NAME_IDX    (0)
#define UID_IDX     (1)

static struct pseudo_passwd *initialize_token_res(fpos_t prev_pos)
{
    if (fsetpos(passwd_file, &prev_pos) || !fgets(cur_passwd_line, PATH_MAX, passwd_file))
        return NULL;

    *strchr(cur_passwd_line, '\n') = '\0';

    int idx = 0;
    for (const char *cur_tok_res = strtok(cur_passwd_line, ":");cur_tok_res;cur_tok_res = strtok(NULL, ":"), ++idx)
        *((const char **)&token_res + idx) = cur_tok_res;

    return &token_res;
}

static struct passwd *initialize_cur_usr(fpos_t prev_pos)
{
    if (!initialize_token_res(prev_pos))
        return NULL;

    cur_usr.pw_name = (char *)token_res.pspw_name;
    cur_usr.pw_passwd = (char *)token_res.pspw_passwd;
    cur_usr.pw_uid = (uid_t)strtoumax(token_res.pspw_uid, NULL, 10);
    cur_usr.pw_gid = (gid_t)strtoumax(token_res.pspw_gid, NULL, 10);
    cur_usr.pw_gecos = (char *)token_res.pspw_gecos;
    cur_usr.pw_dir = (char *)token_res.pspw_dir;
    cur_usr.pw_shell = (char *)token_res.pspw_shell;

    return &cur_usr;
}

static struct passwd *iterate_etc_file_lines(void)
{
    fpos_t prev_pos;

    if (fgetpos(passwd_file, &prev_pos) || !fgets(cur_passwd_line, PATH_MAX, passwd_file))
        return NULL;

    return initialize_cur_usr(prev_pos);
}

struct passwd *csd_getpwnam(const char *name)
{
    if (!passwd_file)
        csd_setpwent();

    int comparison_result = 1;

    struct passwd *cur_passwd;

    for (;(cur_passwd = csd_getpwent()) && (comparison_result = strcmp(cur_passwd -> pw_name, name));)
        ;

    csd_endpwent();

    return !comparison_result ? cur_passwd : NULL;

    
}

struct passwd *csd_getpwuid(uid_t uid_val)
{
    if (!passwd_file)
        csd_setpwent();

    struct passwd *cur_passwd;

    for (;(cur_passwd = csd_getpwent()) && (cur_passwd -> pw_uid != uid_val);)
        ;

    csd_endpwent();

    return cur_passwd -> pw_uid == uid_val ? cur_passwd : NULL;

}

void csd_setpwent(void)
{
    if (!(passwd_file = fopen("/etc/passwd", "r")))
        exit(EXIT_FAILURE);
}


struct passwd *csd_getpwent(void)
{
    return iterate_etc_file_lines();
}

void csd_endpwent(void)
{
    fclose(passwd_file);
    passwd_file = NULL;
}