#if !defined MYGRP_H
#define MYGRP_H

#include <grp.h>

struct group *csd_getgrnam(const char *name);
struct group *csd_getgrgid(gid_t gid_val);
void csd_setgrent(void);
struct group *csd_getgrent(void);
void csd_endgrpent(void);

#endif