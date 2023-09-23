#include "GetFileIntCount.h"
#include "ExitSys.h"

BOOL GetFileIntCount(const char *lpszFileName, size_t *lpIntCount)
{
	BOOL success = FALSE;

	HANDLE hFile = CreateFile(lpszFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		goto FAIL_LEVEL_1;
	INT iVal;
	*lpIntCount = 0;
	for (DWORD dwReadBytes = 1; dwReadBytes;++*lpIntCount)
		if (!ReadFile(hFile, &iVal, (DWORD)sizeof(INT), &dwReadBytes, NULL))
			goto FAIL_LEVEL_2;

	success = TRUE;

FAIL_LEVEL_2:
	CloseHandle(hFile);
FAIL_LEVEL_1:
	if (!success)
		SetErrorStrLastError(__func__);
	return success;
}