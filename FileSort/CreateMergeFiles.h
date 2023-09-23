#if !defined CREATE_MERGE_FILES_H
#define CREATE_MERGE_FILES_H

#include <Windows.h>

HANDLE* CreateMergeFiles(LPCSTR lpszSourceFileName, size_t mergeFileCount);
void DestroyMergeFiles(HANDLE* lphMergeFiles, size_t mergeFileCount);

#endif
