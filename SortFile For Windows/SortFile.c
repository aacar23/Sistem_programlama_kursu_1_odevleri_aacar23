#include "SortFile.h"
#include "MergeFiles.h"
#include "CreateTempFiles.h"
#include "ExitSys.h"


BOOL SortFile(LPCSTR lpcszFileName, DWORD dwMergeFileCount)
{
	HANDLE hFile = CreateFile(lpcszFileName, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		SetErrorStrLastError(__func__);
		return FALSE;
	}

	LPHANDLE lphMergeFiles = CreateTempFileArray(dwMergeFileCount);
	if (!lphMergeFiles)
		goto FAIL1;

	if (!FillSortedTempFileArray(lphMergeFiles, hFile, dwMergeFileCount))
		goto FAIL2;

	if (!SetBeginAllElementsInTempFileArray(lphMergeFiles, dwMergeFileCount))
		goto FAIL2;
		

	if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
		SetErrorStrLastError(__func__);
		goto FAIL2;
	}

	if (!MergeFiles(hFile, lphMergeFiles, dwMergeFileCount))
		goto FAIL2;

	DestroyTempFileArray(lphMergeFiles, dwMergeFileCount);
	CloseHandle(hFile);

	return TRUE;

FAIL2:
	DestroyTempFileArray(lphMergeFiles, dwMergeFileCount);
FAIL1:
	CloseHandle(hFile);
	return FALSE;
}
