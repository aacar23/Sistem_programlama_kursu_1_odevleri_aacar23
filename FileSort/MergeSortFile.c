#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "MergeSortFile.h"
#include "CreateMergeFiles.h"
#include "GetFileIntCount.h"
#include "ExitSys.h"

typedef struct {
	HANDLE hMergeFile;
	INT iCurVal;
	BOOL bEofFlag;
}MERGER, *LPMERGER;

typedef const MERGER* LPCMERGER;

#define SET_MERGER_FILE(lpMergerPtr, hFile)				(((lpMergerPtr) -> hMergeFile) = (hFile))
#define SET_MERGER_CURVAL(lpMergerPtr, iVal)			(((lpMergerPtr) -> iCurVal) = (iVal))
#define SET_MERGER_EOF_FLAG(lpMergerPtr, bVal)			(((lpMergerPtr) -> bEofFlag) = (bVal))

#define SET_MERGER_ALL(lpMergerPtr, hFile, iVal, bVal)	(SET_MERGER_FILE(lpMergerPtr, hFile), SET_MERGER_CURVAL(lpMergerPtr, iVal), SET_MERGER_EOF_FLAG(lpMergerPtr, bVal))

#define GET_MERGER_FILE(lpMergerPtr)					(((LPCMERGER)lpMergerPtr) -> hMergeFile)
#define GET_MERGER_CURVAL(lpMergerPtr)					(((LPCMERGER)lpMergerPtr) -> iCurVal)
#define GET_MERGER_EOF_FLAG(lpMergerPtr)				(((LPCMERGER)lpMergerPtr) -> bEofFlag)

static LPMERGER CreateMergers(HANDLE *lphMergeFiles, size_t mergeFileCount)
{
	void (*SetErrorStr)(LPCSTR lpszMsg);
	LPMERGER lpMergers = malloc(sizeof(MERGER) * mergeFileCount);
	if (!lpMergers) {
		SetErrorStr = &SetErrorStrErrno;
		goto FAIL_LEVEL_1;
	}

	for (size_t i = 0; i < mergeFileCount; ++i) {
		INT iVal;
		DWORD dwReadBytes;
		if (!ReadFile(*(lphMergeFiles + i), &iVal, sizeof(INT), &dwReadBytes, NULL)) {
			SetErrorStr = &SetErrorStrLastError;
			goto FAIL_LEVEL_2;
		}
		SET_MERGER_ALL(lpMergers + i, *(lphMergeFiles + i), iVal, (dwReadBytes < sizeof(INT)));
	}

	return lpMergers;

FAIL_LEVEL_2:
	free(lpMergers);
FAIL_LEVEL_1:
	(*SetErrorStr)(__func__);
	return NULL;
}

static INT ProgressMerge(HANDLE hDestFile, LPMERGER lpMergers, size_t mergerCount)
{
	INT iMinVal = INT_MAX;
	LPMERGER lpMinMerger;
	size_t eofCount = 0;
	for (size_t i = 0; i < mergerCount; ++i){
		if (GET_MERGER_EOF_FLAG(lpMergers + i))
			++eofCount;
		else if (GET_MERGER_CURVAL(lpMergers + i) < iMinVal) {
			iMinVal = GET_MERGER_CURVAL(lpMergers + i);
			lpMinMerger = lpMergers + i;
		}
	}

	if (eofCount == mergerCount)
		return EOF;

	if (!WriteFile(hDestFile, &iMinVal, (DWORD)sizeof(int), &(DWORD) { 0 }, NULL))
		goto FAIL;

	DWORD dwReadBytes;
	if (!ReadFile(GET_MERGER_FILE(lpMinMerger), &lpMinMerger -> iCurVal, (DWORD)sizeof(INT), &dwReadBytes, NULL))
		goto FAIL;

	if (dwReadBytes < sizeof(int))
		SET_MERGER_EOF_FLAG(lpMinMerger, TRUE);

	return TRUE;

FAIL:
	SetErrorStrLastError(__func__);
	return FALSE;
}


BOOL MergeSortFile(LPCSTR lpszDestFileName, size_t mergeFileCount)
{
	void (*SetErrorStr)(LPCSTR lpszMsg) = &PrependMsgToErrorStr;
	BOOL success = FALSE;

	HANDLE* lphMergeFiles = CreateMergeFiles(lpszDestFileName, mergeFileCount);
	if (!lphMergeFiles)
		goto FAIL_LEVEL_1;

	HANDLE hDestFile = CreateFile(lpszDestFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hDestFile == INVALID_HANDLE_VALUE) {
		SetErrorStr = &SetErrorStrLastError;
		goto FAIL_LEVEL_2;
	}

	LPMERGER lpMergers = CreateMergers(lphMergeFiles, mergeFileCount);
	if (!lpMergers)
		goto FAIL_LEVEL_3;

	for (INT res;(res = ProgressMerge(hDestFile, lpMergers, mergeFileCount)) != EOF;)
		if (!res)
			goto FAIL_LEVEL_4;


	success = TRUE;

FAIL_LEVEL_4:
	free(lpMergers);
FAIL_LEVEL_3:
	CloseHandle(hDestFile);
FAIL_LEVEL_2:
	DestroyMergeFiles(lphMergeFiles, mergeFileCount);
FAIL_LEVEL_1:
	if (!success)
		(*SetErrorStr)(__func__);
	return success;

}