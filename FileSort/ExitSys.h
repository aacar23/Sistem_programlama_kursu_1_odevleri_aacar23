#if !defined EXIT_SYS_H
#define EXIT_SYS_H

#include <Windows.h>

#define ERROR_STR_SIZE (1000)

extern CHAR szErrorStr[ERROR_STR_SIZE];

VOID PrependMsgToErrorStr(LPCSTR lpszMsg);
VOID SetErrorStrLastError(LPCSTR lpszMsg);
VOID SetErrorStrErrno(LPCSTR lpszMsg);
VOID SetErrorStrSuccess(LPCSTR lpszMsg);
VOID SetErrorStrCustom(LPCSTR lpszMsg, LPCSTR lpszError);
VOID ExitSys(void);

#endif