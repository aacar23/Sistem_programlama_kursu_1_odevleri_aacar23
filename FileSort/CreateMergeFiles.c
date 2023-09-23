#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <math.h>



#include "CreateMergeFiles.h"
#include "GetFileIntCount.h"
#include "ExitSys.h"



static HANDLE CreateTempFile(void)
{
	HANDLE hTempFile = CreateFile(tmpnam(NULL), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE, NULL);
	if (hTempFile == INVALID_HANDLE_VALUE) {
		SetErrorStrLastError(__func__);
		return INVALID_HANDLE_VALUE;
	}

	return hTempFile;
}

static void DestroyTempFiles(HANDLE* lphTempFiles, size_t tempFileCount)
{
	for (size_t i = 0; i < tempFileCount; ++i)
		CloseHandle(*(lphTempFiles + i));
	free(lphTempFiles);
}

static HANDLE* CreateTempFiles(size_t tempFileCount)
{
	void (*SetErrorStr)(LPCSTR lpszMsg) = &SetErrorStrSuccess;

	HANDLE* lphTempFiles = malloc(sizeof(HANDLE) * tempFileCount);
	if (!lphTempFiles) {
		SetErrorStr = &SetErrorStrErrno;
		goto FAIL_LEVEL_1;
	}

	size_t i = 0;

	for (;i < tempFileCount; ++i)
		if ((*(lphTempFiles + i) = CreateTempFile()) == INVALID_HANDLE_VALUE) {
			SetErrorStr = &PrependMsgToErrorStr;
			goto FAIL_LEVEL_2;
		}

	return lphTempFiles;

FAIL_LEVEL_2:
	DestroyTempFiles(lphTempFiles, i);
FAIL_LEVEL_1:
	(*SetErrorStr)(__func__);
	return NULL;

}


static int cmp(const void* vp1, const void* vp2)
{
	const int* p1 = (const void*)vp1;
	const int* p2 = (const void*)vp2;

	return *p1 > *p2 ? 1 : *p1 < *p2 ? -1 : 0;
}

static BOOL FillFileWithSortedInts(HANDLE hDest,HANDLE hSource, size_t intCount)
{

	void (*SetErrorStr)(LPCSTR lpszMsg) = &SetErrorStrLastError;
	BOOL success = FALSE;
	

	int* lpiTempBuffer = malloc(sizeof(int) * intCount);
	if (!lpiTempBuffer) {
		SetErrorStr = &SetErrorStrErrno;
		goto FAIL_LEVEL_1;
	}

	DWORD dwReadBytes;
	if (!ReadFile(hSource, lpiTempBuffer, (DWORD)(sizeof(int) * intCount), &dwReadBytes, NULL))
		goto FAIL_LEVEL_2;

	

	qsort(lpiTempBuffer, dwReadBytes / sizeof(int), sizeof(int), &cmp);

	if (!WriteFile(hDest, lpiTempBuffer, dwReadBytes, &(DWORD){0}, NULL))
		goto FAIL_LEVEL_2;

	if (SetFilePointer(hDest, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		goto FAIL_LEVEL_2;


	success = TRUE;

FAIL_LEVEL_2:
	free(lpiTempBuffer);
FAIL_LEVEL_1:
	if (!success)
		(*SetErrorStr)(__func__);
	return success;

	
}

HANDLE* CreateMergeFiles(LPCSTR lpszSourceFileName, size_t mergeFileCount)
{

	size_t sourceFileIntCount;
	if (!GetFileIntCount(lpszSourceFileName, &sourceFileIntCount))
		goto FAIL_LEVEL_1;

	if (sourceFileIntCount < mergeFileCount) {
		SetErrorStrCustom(__func__, "Merge file count should be smaller or equal to source files int count\n");
		return NULL;
	}

	size_t intCount = (size_t)ceil((double)sourceFileIntCount / mergeFileCount);

	HANDLE* lphMergeFiles = CreateTempFiles(mergeFileCount);
	if (!lphMergeFiles)
		goto FAIL_LEVEL_1;

	size_t i = 0;

	HANDLE hSource = CreateFile(lpszSourceFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hSource == INVALID_HANDLE_VALUE)
		goto FAIL_LEVEL_2;

	for (;i < mergeFileCount;++i)
		if (!FillFileWithSortedInts(*(lphMergeFiles + i), hSource, intCount)) 
			goto FAIL_LEVEL_3;

	CloseHandle(hSource);
	return lphMergeFiles;

FAIL_LEVEL_3:
	CloseHandle(hSource);
FAIL_LEVEL_2:
	DestroyMergeFiles(lphMergeFiles, i);
FAIL_LEVEL_1:
	PrependMsgToErrorStr(__func__);
	return NULL;
}

void DestroyMergeFiles(HANDLE* lphMergeFiles, size_t mergeFileCount)
{
	DestroyTempFiles(lphMergeFiles, mergeFileCount);
}