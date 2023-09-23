#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "WinParse.h"

#define NO_ARGUMENT							(0)
#define REQUIRED_ARGUMENT					(1)
#define OPTIONAL_ARGUMENT					(2)

#define OPTION_NAME(opt)					((opt) . lpszOptName)
#define OPTION_TYPE(opt)					((opt) . iOptType)

#define SET_OPTION_NAME(opt, lpszName)		((opt) . lpszOptName = (lpszName))
#define SET_OPTION_TYPE(opt, iType)			((opt) . iOptType = (iType))

LPSTR lpszOptArg;
size_t iOptIdx;
static CHAR szOptErrorStr[100];

typedef struct tagOPTION {
	size_t iOptType;
	LPSTR lpszOptName;
}OPTION;

static int cmp(const void* vp1, const void* vp2)
{
	const char* const* arg1 = (const char* const*)vp1;
	const char* const* arg2 = (const char* const*)vp2;
	return **arg1 != '/' && **arg2 == '/' ? 1 : **arg1 == '/' && **arg2 != '/' ? -1 : 0;
}

static INT IterateValidationOfOptStr(LPCSTR lpszOptString, size_t* lpiCurIdx)
{
	if (!*lpszOptString)
		return EOF;
	size_t IterationValue = strcspn(lpszOptString, ":");
	size_t validationResult = strspn(lpszOptString + IterationValue, ":");
	*lpiCurIdx += IterationValue + validationResult;
	return !(validationResult > 3 || validationResult < 1);
}

static BOOL ValidateOptString(LPCSTR lpszOptString)
{
	size_t iCurIdx = 0;

	INT iValidationResult;
	while ((iValidationResult = IterateValidationOfOptStr(lpszOptString + iCurIdx, &iCurIdx)) != EOF) {
		if (!iValidationResult)
			return FALSE;
	}

	return TRUE;

}

static BOOL IsValid(LPCSTR lpszOptions, LPCSTR lpszOption, size_t* lpiOptType) {

	int success = FALSE;

	LPSTR lpszOptionDup = _strdup(lpszOption);
	if (!lpszOptionDup) {
		strcpy(szOptErrorStr, "Couldn't allocate dynamic string duplicate\n");
		return FALSE;
	}

	LPSTR lpszArgumentColonSearchResult = strchr(lpszOptionDup, ':');
	if (lpszArgumentColonSearchResult)
		*lpszArgumentColonSearchResult = '\0';

	LPSTR lpszSearch = strstr(lpszOptions, lpszOptionDup);

	if (!lpszSearch) {
		sprintf(szOptErrorStr, "Invalid option : %s\n", lpszOptionDup);
		goto FAIL;
	}

	size_t len = strspn(lpszSearch + strlen(lpszOptionDup), ":");
	
	*lpiOptType = len - 1;

	success = TRUE;

FAIL:
	free(lpszOptionDup);
	return success;
}

static BOOL SetOptArg(OPTION* lpOpt)
{
	if (OPTION_TYPE(*lpOpt) == NO_ARGUMENT) {
		lpszOptArg = NULL;
		return TRUE;
	}

	LPSTR lpszCurArg = strchr(OPTION_NAME(*lpOpt), ':');

	if (!lpszCurArg || !*(lpszCurArg + 1)) {
		lpszOptArg = NULL;
		if (OPTION_TYPE(*lpOpt) == REQUIRED_ARGUMENT) {
			sprintf(szOptErrorStr, "Invalid option format : %s\n", OPTION_NAME(*lpOpt));
			return FALSE;
		}
		return TRUE;
	}

	lpszOptArg = *(lpszCurArg + 1) ? lpszCurArg + 1 : NULL;
	return TRUE;
}

LPSTR WinParse(int argc, char** argv, LPCSTR lpszOptions)
{

	static INT count = 1;
	static OPTION curOpt;

	if (count == 1) {
		if (!ValidateOptString(lpszOptions)) {
			fprintf(stderr, "%s : %s\n", strcpy(szOptErrorStr, "Invalid option string : %s"), lpszOptions);
			return OPT_ERROR;
		}
		qsort(argv + 1, argc - 1, sizeof(char*), &cmp);
	}

	if (!*(argv + count) || **(argv + count) != '/') {
		iOptIdx = count;
		return NULL;
	}
	

	SET_OPTION_NAME(curOpt, *(argv + count));

	if (!IsValid(lpszOptions, *(argv + count) + 1, &OPTION_TYPE(curOpt)))
		goto FAIL;

	if (OPTION_TYPE(curOpt) != NO_ARGUMENT)
		if (!SetOptArg(&curOpt))
			goto FAIL;

	*strchr(*(argv + count), ':') = '\0';

	return *(argv + count++);

FAIL:
	fprintf(stderr, "%s\n", szOptErrorStr);
	return OPT_ERROR;
}