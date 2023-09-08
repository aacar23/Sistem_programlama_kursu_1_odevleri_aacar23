#if !defined GET_OPT_H
#define GET_OPT_H

#include <Windows.h>

extern BOOL bOptErr;
extern LPSTR lpszOptArg;
extern INT dwOptInd;
extern INT dwOptOpt;

INT GetOpt(int argc, char** argv, LPCSTR lpcszOptString);

#endif