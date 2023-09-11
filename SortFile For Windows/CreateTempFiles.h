#if !defined CREATE_TEMP_FILE_ARRAY_H
#define CREATE_TEMP_FILE_ARRAY_H

#include <Windows.h>

LPHANDLE CreateTempFileArray(DWORD dwFileCount);
BOOL FillSortedTempFileArray(LPHANDLE lphTemps, HANDLE hFile, DWORD dwFileCount);
BOOL SetBeginAllElementsInTempFileArray(LPHANDLE lphTemps, DWORD dwFileCount);
VOID DestroyTempFileArray(LPHANDLE lphTemps, DWORD dwFileCount);

#endif