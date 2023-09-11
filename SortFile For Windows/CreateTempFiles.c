#define _CRT_SECURE_NO_WARNINGS

#include "CreateTempFiles.h"
#include "ExitSys.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define CREATE_CMP_FUNC(type) static type Cmp##type(const void *vp1, const void *vp2){ \
									const type *p1 = (const type *)vp1; \
									const type* p2 = (const type*)vp2; \
									return *p1 > *p2 ? 1 : *p1 < *p2 ? -1 : 0; \
							  }

static HANDLE CreateTempFile(void)
{
	HANDLE hTemp = CreateFile(tmpnam(NULL), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_DELETE_ON_CLOSE | FILE_ATTRIBUTE_TEMPORARY, NULL);
	if (hTemp == INVALID_HANDLE_VALUE) {
		SetErrorStrLastError(__func__);
		return NULL;
	}

	return hTemp;
}

static VOID HandleCreateTempFileArrayError(LPHANDLE lphTempFileArray, DWORD dwCreatedFiles)
{
	for (DWORD i = dwCreatedFiles - 1; i > 0; --i)
		CloseHandle(*(lphTempFileArray + i));

	free(lphTempFileArray);
}


LPHANDLE CreateTempFileArray(DWORD dwTempFileCount) {
	LPHANDLE lphTempFileArray = malloc(sizeof(HANDLE) * dwTempFileCount);
	if (!lphTempFileArray) {
		SetErrorStrCustom("CreateTempFileArray:Cannot allocate temporary file array\n");
		return NULL;
	}

	for (DWORD i = 0; i < dwTempFileCount; ++i)
		if (!(*(lphTempFileArray + i) = CreateTempFile())) {
			SetErrorStrCustom("CreateTempFileArray:Failed to create temp files\n");
			HandleCreateTempFileArrayError(lphTempFileArray, i + 1);
			return NULL;
		}

	return lphTempFileArray;
}

CREATE_CMP_FUNC(DWORD)


BOOL FillSortedTempFileArray(LPHANDLE lphTemps, HANDLE hFile, DWORD dwTempFileCount)
{
	LARGE_INTEGER size;
	if (!GetFileSizeEx(hFile, &size))
		goto WINFAIL;

	DWORD dwDwordCount = (DWORD)ceil((double)size.QuadPart / dwTempFileCount / sizeof(DWORD));

	DWORD* lpdwBuf = malloc(dwDwordCount * sizeof(DWORD));
	if (!lpdwBuf) {
		SetErrorStrCustom("FillTempFileArray:Cannot allocate temporary buffer\n");
		return FALSE;
	}

	for (DWORD i = 0, dwReadBytes; i < dwTempFileCount; ++i) {
		if (!ReadFile(hFile, lpdwBuf, dwDwordCount * sizeof(DWORD), &dwReadBytes, NULL))
			goto WINFAIL;
		qsort(lpdwBuf, dwDwordCount, sizeof(DWORD), &CmpDWORD);
		if (!WriteFile(*(lphTemps + i), lpdwBuf, dwReadBytes, &(DWORD){0}, NULL))
			goto WINFAIL;
	}

	return TRUE;

WINFAIL:
	SetErrorStrLastError(__func__);
	return FALSE;
}

BOOL SetBeginAllElementsInTempFileArray(LPHANDLE lphTemps, DWORD dwTempFileCount)
{
	for (DWORD i = 0; i < dwTempFileCount; ++i) {
		if (SetFilePointer(*(lphTemps + i), 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
			SetErrorStrLastError(__func__);
			return FALSE;
		}
	}
	return TRUE;
}

VOID DestroyTempFileArray(LPHANDLE lphTemps, DWORD dwTempFileCount)
{
	for (DWORD i = 0; i < dwTempFileCount; ++i)
		CloseHandle(*(lphTemps + i));

	free(lphTemps);
}