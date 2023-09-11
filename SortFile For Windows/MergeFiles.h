#if !defined MERGE_FILES_H
#define MERGE_FILES_H

#include <Windows.h>
#include <stdio.h>

typedef struct tagMERGEELEM {
	HANDLE hFile;
	DWORD dwCurVal;
	BOOL isEof;
}MERGEELEM, * LPMERGEELEM;

BOOL MergeFiles(HANDLE hFile, LPHANDLE lphMergeFiles, DWORD dwMergeFileCount);

#endif