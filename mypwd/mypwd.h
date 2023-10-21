#if !defined MYPWD_H
#define MYPWD_H

#include <pwd.h>


struct passwd *csd_getpwnam(const char *name);
struct passwd *csd_getpwuid(uid_t uid_val);
void csd_setpwent(void);
struct passwd *csd_getpwent(void);
void csd_endpwent(void);


#endif