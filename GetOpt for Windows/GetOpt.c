#include "GetOpt.h"

#include <stdio.h>
#include <string.h>

#define NO_ARGUMENT (0)
#define REQUIRED_ARGUMENT (1)
#define OPTIONAL_ARGUMENT (2)
#define INVALID_OPTION (3)

#define CUR_ARG_OPT_CHAR (*(*(argv + dwOptInd) + 1))
#define CUR_ARG_OPT ((*(argv + dwOptInd)))

BOOL bOptErr = 1;
LPSTR lpszOptArg;
INT dwOptInd = 1;
INT dwOptOpt;

static void swap(void* vp1, void* vp2, size_t size)
{
	char* p1 = (char*)vp1;
	char* p2 = (char*)vp2;

	while (size--) {
		int temp = *p1;
		*p1++ = *p2;
		*p2++ = temp;
	}
}

static int cmp(LPCSTR lpcszStr1, LPCSTR lpcszStr2)
{

	BOOL str1IsOpt = !!strchr(lpcszStr1, '/');
	BOOL str2IsOpt = !!strchr(lpcszStr2, '/');

	return !str1IsOpt && str2IsOpt;
}

static VOID SortArguments(int argc, char** argv)
{
	for (int i = 1; i < argc - 1; ++i) {
		for (int j = 1; j < argc - i; ++j)
			if (cmp(*(argv + j), *(argv + j + 1)))
				swap(argv + j, argv + j + 1, sizeof(char*));
	}
}

static DWORD GetOptType(LPCSTR lpcszOptString, CHAR ch)
{
	LPCSTR lpcszSearchResult = strchr(lpcszOptString, ch);
	if (!lpcszSearchResult)
		return INVALID_OPTION;

	if (*(lpcszSearchResult + 1) == ':' && *(lpcszSearchResult + 2) == ':')
		return OPTIONAL_ARGUMENT;

	if (*(lpcszSearchResult + 1) == ':')
		return REQUIRED_ARGUMENT;

	return NO_ARGUMENT;
}

INT GetOpt(int argc, char** argv, LPCSTR lpcszOptString)
{
	if (dwOptInd == 1)
		SortArguments(argc, argv);

	if (!*(argv + dwOptInd++))
		return -1;

	if (!strcmp(*(argv + dwOptInd), "/"))
		return -1;

	if (*(*(argv + dwOptInd)) == '/') {
		if (*(*(argv + dwOptInd) + 1) == ':') {
			fprintf(stderr, "Forbidden option \"/:\" under any circumstances do not enter this option\n");
			return -1;
		}
		switch (GetOptType(lpcszOptString, CUR_ARG_OPT_CHAR)) {

		case INVALID_OPTION:
			if (bOptErr)
				fprintf(stderr, "/%c is not a valid option.\n", dwOptOpt = CUR_ARG_OPT_CHAR);
			return '?';

		case REQUIRED_ARGUMENT:
			lpszOptArg = strchr(CUR_ARG_OPT, ':');
			if (!lpszOptArg || !*(++lpszOptArg)) {
				if (bOptErr)
					fprintf(stderr, "Option /%c does not have an argument.\n", dwOptOpt = CUR_ARG_OPT_CHAR);
				return ':';
			}
			return CUR_ARG_OPT_CHAR;

		case OPTIONAL_ARGUMENT:
			lpszOptArg = strchr(CUR_ARG_OPT, ':');
			if (lpszOptArg)
				++lpszOptArg;
			return CUR_ARG_OPT_CHAR;

		case NO_ARGUMENT:
			if (strchr(CUR_ARG_OPT, ':')) {
				if (bOptErr)
					fprintf(stderr, "%c option cannot have an argument\n", CUR_ARG_OPT_CHAR);
				return '!';
			}
			return CUR_ARG_OPT_CHAR;

		}
	}
	
	return -1;
}

