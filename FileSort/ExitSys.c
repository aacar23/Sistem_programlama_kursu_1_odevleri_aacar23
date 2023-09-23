#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


#include "ExitSys.h"

CHAR szErrorStr[ERROR_STR_SIZE];

VOID PrependMsgToErrorStr(LPCSTR lpszMsg)
{
	memmove(szErrorStr + strlen(lpszMsg) + 2, szErrorStr, strlen(szErrorStr) + 1);
	strcpy(szErrorStr, lpszMsg);
	memcpy(szErrorStr + strlen(lpszMsg), ": ", 2);
}

VOID SetErrorStrLastError(LPCSTR lpszMsg)
{
	DWORD dwLastErr = GetLastError();
	LPTSTR lpszErr;

	if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwLastErr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpszErr, 0, NULL)) {
		sprintf(szErrorStr, "%s: %s", lpszMsg, lpszErr);
		LocalFree(lpszErr);
	}
}

VOID SetErrorStrErrno(LPCSTR lpszMsg)
{
	sprintf(szErrorStr, "%s: %s", lpszMsg, strerror(errno));
}

VOID SetErrorStrSuccess(LPCSTR lpszMsg)
{
	sprintf(szErrorStr, "%s: %s", lpszMsg, "success");
}

VOID SetErrorStrCustom(LPCSTR lpszMsg, LPCSTR lpszError)
{
	sprintf(szErrorStr, "%s: %s", lpszMsg, lpszError);
}

VOID ExitSys(void)
{
	fprintf(stderr, "%s\n", szErrorStr);
	exit(EXIT_FAILURE);
}