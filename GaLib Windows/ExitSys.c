#define _CRT_SECURE_NO_WARNINGS
#include "ExitSys.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define ERROR_STR_LENGTH (1000)

static CHAR szErrorStr[ERROR_STR_LENGTH];

VOID SetErrorStrLastError(LPCSTR lpcszMsg)
{
	DWORD dwLastErr = GetLastError();
	LPTSTR lpszErr;

	if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwLastErr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpszErr, 0, NULL)) {
		sprintf(szErrorStr, "%s:%s:%s", lpcszMsg, ":", (LPSTR)lpszErr);
		LocalFree(lpszErr);
	}
}

static VOID PrependTag(LPSTR lpszDest, LPCSTR lpcszHeadStr)
{
	memmove(lpszDest + 1, lpszDest, strlen(lpszDest) + 1);
	*lpszDest = ':';
	memmove(lpszDest + strlen(lpcszHeadStr) + 1, lpszDest, strlen(lpszDest) + 1);
	strcpy(lpszDest, lpcszHeadStr);
	memmove(lpszDest + strlen(lpcszHeadStr), lpszDest + strlen(lpcszHeadStr) + 1, strlen(lpszDest) - strlen(lpcszHeadStr));
}

VOID PrependTagToErrorStr(LPCSTR lpcszTag)
{
	PrependTag(szErrorStr, lpcszTag);
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
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpszErr, 0, NULL)) {
		fprintf(stderr, "%s: %s", lpszMsg, (LPSTR)lpszErr);
		LocalFree(lpszErr);
	}

	exit(EXIT_FAILURE);
}
