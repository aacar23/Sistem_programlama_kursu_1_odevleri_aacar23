#if !defined EXIT_SYS_H
#define EXIT_SYS_H

#include <Windows.h>

VOID SetErrorStrLastError(LPCSTR lpcszMsg);
VOID SetErrorStrCustom(LPCSTR lpszMsg);
VOID ExitSysWithErrorStr(LPCSTR lpszMsg);
VOID ExitSys(LPCSTR lpszMsg);

#endif