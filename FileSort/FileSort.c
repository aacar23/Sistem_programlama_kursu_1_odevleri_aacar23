#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "MergeSortFile.h"
#include "WinParse.h"
#include "ExitSys.h"

int main(int argc, char** argv)
{
	INT base = 10;
	size_t mergeFileCount = 2;
	for (LPSTR res; (res = WinParse(argc, argv, "merge-file-dec-count::merge-file-hex-count::merge-file-oct-count::"));) {
		if (res == OPT_ERROR)
			return 1;
		if (!strcmp(res, "merge-file-dec-count"))
			goto OUT_OF_LOOP;
		if (!strcmp(res, "merge-file-hex-count")) {
			base = 16;
			goto OUT_OF_LOOP;
		}
		if (!strcmp(res, "merge-file-oct-count")) {
			base = 8;
			goto OUT_OF_LOOP;
		}
	}

	char* end;

OUT_OF_LOOP:
	if (lpszOptArg) {
		mergeFileCount = strtoull(lpszOptArg, &end, base);
		if (end == lpszOptArg || *end != '\0') {
			fprintf(stderr, "Given merge file count argument is not in proper base\n");
			return 1;
		}
	}

	for (size_t i = iOptIdx; i < argc; ++i) {
		if (!MergeSortFile(*(argv + i), mergeFileCount))
			ExitSys();
	}

	return 0;
}