#include "GetAndPutDword.h"
#include "ExitSys.h"

DWORD GetDword(HANDLE hFile, DWORD* lpdwSuccess)
{
	DWORD dwReadDword;
	DWORD dwReadBytes;
	if (!ReadFile(hFile, &dwReadDword, sizeof(DWORD), &dwReadBytes, NULL)) {
		SetErrorStrLastError(__func__);
		*lpdwSuccess = GET_DWORD_FAILURE;
		return GET_DWORD_FAILED;
	}

	if (dwReadBytes < sizeof(DWORD)) {
		*lpdwSuccess = GET_AND_PUT_DWORD_EOF;
		return GET_DWORD_FAILED;
	}

	*lpdwSuccess = GET_DWORD_SUCCESS;

	return dwReadDword;
}

DWORD PutDword(DWORD dwVal, HANDLE hFile, DWORD* lpdwSuccess)
{
	if (!WriteFile(hFile, &dwVal, sizeof(DWORD), &(DWORD){0}, NULL)) {
		SetErrorStrLastError(__func__);
		*lpdwSuccess = GET_DWORD_FAILURE;
		return PUT_DWORD_FAILED;
	}

	*lpdwSuccess = GET_DWORD_SUCCESS;
	return dwVal;
}