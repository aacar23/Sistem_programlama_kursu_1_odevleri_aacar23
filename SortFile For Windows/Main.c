#include "SortFile.h"
#include "GetOpt.h"
#include "ExitSys.h"

#include <stdio.h>
#include <ctype.h>

#define DECIMAL (10)
#define OCTAL (8)
#define HEXADECIMAL (16)


int isoctal(int ch)
{
	return ch >= '0' && ch <= '8';
}

static BOOL isNum(LPCSTR lpcszStr, DWORD dwBase)
{
	int (*CharTest)(int ch);

	switch (dwBase) {
	case DECIMAL:
		CharTest = &isdigit;
		break;
	case OCTAL:
		CharTest = &isoctal;
		break;
	case HEXADECIMAL:
		CharTest = &isxdigit;
		break;
	}

	for (; *lpcszStr; ++lpcszStr)
		if (!CharTest(*lpcszStr))
			return FALSE;

	return TRUE;
}

#define CASE_OPT(opt, flagVal)  case (opt):\
								dwBase = (flagVal); \
								lpcszArg = lpszOptArg;\
								goto OUT_OF_LOOP 

#define CASE_ERR(err) case (err):\
					  return 1

#define CASE_OPTS() CASE_OPT('d', DECIMAL);CASE_OPT('o', OCTAL);CASE_OPT('x', HEXADECIMAL)
#define CASE_ERRS() CASE_ERR('?');CASE_ERR('!');CASE_ERR(':');CASE_ERR('/');CASE_ERR('\\')

int main(int argc, char **argv){
	bOptErr = TRUE;
	DWORD dwBase = DECIMAL;
	LPCSTR lpcszArg = "10";
	int count = 0;
	for (DWORD c; (c = GetOpt(argc, argv, "d:o:x:")) != -1;++count) {
		DWORD dwOptCnt = 0;
		switch (c) {
			CASE_OPTS();
			CASE_ERRS();
		}
	}

OUT_OF_LOOP:
	for (DWORD i = dwOptInd;i < (DWORD)(argc);++i)
		if (!SortFile(*(argv + i), (DWORD)atoi(lpcszArg)))
			ExitSysWithErrorStr("Main");

	return 0;
}