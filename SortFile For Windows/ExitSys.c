#define _CRT_SECURE_NO_WARNINGS
#include "ExitSys.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define ERROR_STR_LENGTH (1000)

CHAR szErrorStr[ERROR_STR_LENGTH];

VOID SetErrorStrLastError(LPCSTR lpcszMsg)
{
	DWORD dwLastErr = GetLastError();
	LPTSTR lpszErr;

	if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwLastErr,
		MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), (LPTSTR)&lpszErr, 0, NULL)) {
		sprintf(szErrorStr, "%s:%s%s", lpcszMsg, ":", lpszErr);
		LocalFree(lpszErr);
	}
}

VOID SetErrorStrCustom(LPCSTR lpszMsg)
{
	strcpy(szErrorStr, lpszMsg);
}

VOID ExitSysWithErrorStr(LPCSTR lpszMsg)
{
	fprintf(stderr, "%s: %s", lpszMsg, szErrorStr);

	exit(EXIT_FAILURE);
}

VOID ExitSys(LPCSTR lpszMsg)
{
	DWORD dwLastErr = GetLastError();
	LPTSTR lpszErr;

	if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwLastErr,
		MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), (LPTSTR)&lpszErr, 0, NULL)) {
		fprintf(stderr, "%s: %s", lpszMsg, lpszErr);
		LocalFree(lpszErr);
	}

	exit(EXIT_FAILURE);
}