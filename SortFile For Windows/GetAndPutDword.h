#if !defined GET_AND_PUT_DWORD_H
#define GET_AND_PUT_DWORD_H

#include <Windows.h>

#define GET_DWORD_FAILED (UINT_MAX)
#define PUT_DWORD_FAILED (UINT_MAX - 1)
#define GET_AND_PUT_DWORD_EOF (1)
#define GET_DWORD_SUCCESS (2)
#define GET_DWORD_FAILURE (3)
#define PUT_DWORD_SUCCESS (4)
#define PUT_DWORD_FAILURE (5)

DWORD GetDword(HANDLE hFile, DWORD* lpdwSuccess);
DWORD PutDword(DWORD dwVal, HANDLE hFile, DWORD* lpdwSuccess);

#endif
