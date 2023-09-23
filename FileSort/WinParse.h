#if !defined WIN_PARSE_H
#define WIN_PARSE_H

#include <Windows.h>

#define OPT_ERROR							((LPVOID)1)

extern LPSTR lpszOptArg;
extern size_t iOptIdx;

LPSTR WinParse(int argc, char** argv, LPCSTR lpszOptions);

#endif