#include "MergeFiles.h"
#include "GetAndPutDword.h"
#include "CreateTempFiles.h"
#include "ExitSys.h"

#include <stdlib.h>
#include <limits.h>

#define DEBUG

#define MERGE_ELEM_FILE(lpObj)								((lpObj) -> hFile)
#define MERGE_ELEM_CURRENT_VALUE(lpObj)						((lpObj) -> dwCurVal)
#define MERGE_ELEM_IS_EOF(lpObj)							((lpObj) -> isEof)
#define SET_MERGE_ELEM_FILE(lpObj, val)						(((lpObj) -> hFile) = (val))
#define SET_MERGE_ELEM_CURRENT_VALUE(lpObj, val)			(((lpObj) -> dwCurVal) = (val))
#define SET_MERGE_ELEM_IS_EOF(lpObj, val)					(((lpObj) -> isEof) = (val))

static LPMERGEELEM CreateMergeElemArray(LPHANDLE lphFileArray, DWORD dwFileCount)
{
	LPMERGEELEM lpMergeElemArray = malloc(sizeof(MERGEELEM) * dwFileCount);
	if (!lpMergeElemArray) {
		SetErrorStrCustom("CreateMergeElem:Cannot allocate merge element array\n");
		return NULL;
	}

	for (DWORD i = 0, dwSuccess; i < dwFileCount; ++i) {
		SET_MERGE_ELEM_FILE(lpMergeElemArray + i, *(lphFileArray + i));
		SET_MERGE_ELEM_CURRENT_VALUE(lpMergeElemArray + i, GetDword(*(lphFileArray + i), &dwSuccess));
		SET_MERGE_ELEM_IS_EOF(lpMergeElemArray + i, FALSE);
		if (dwSuccess == GET_DWORD_FAILURE) {
			free(lpMergeElemArray);
			return NULL;
		}
	}

	return lpMergeElemArray;
}

static INT IterateMergeElemArray(HANDLE hFile, LPMERGEELEM lpMergeElems, DWORD dwMergeElemCount) {
	LPMERGEELEM lpMinMergeElem = &(MERGEELEM) {NULL, UINT_MAX, FALSE};
	
	DWORD dwEofCount = 0;
	for (DWORD i = 0; i < dwMergeElemCount; ++i) {
		if (MERGE_ELEM_IS_EOF(lpMergeElems + i)) {
			++dwEofCount;
			continue;
		}
		if (MERGE_ELEM_CURRENT_VALUE(lpMergeElems + i) < MERGE_ELEM_CURRENT_VALUE(lpMinMergeElem))
			lpMinMergeElem = lpMergeElems + i;
	}

	if (dwEofCount == dwMergeElemCount)
		return EOF;

	DWORD dwPutSuccess;

	PutDword(MERGE_ELEM_CURRENT_VALUE(lpMinMergeElem), hFile, &dwPutSuccess);
	if (dwPutSuccess == PUT_DWORD_FAILURE)
		return FALSE;

	DWORD dwGetSuccess;

	DWORD dwNextCurValue = GetDword(MERGE_ELEM_FILE(lpMinMergeElem), &dwGetSuccess);
	if (dwGetSuccess == GET_DWORD_FAILURE)
		return FALSE;

	if (dwGetSuccess == GET_AND_PUT_DWORD_EOF)
		SET_MERGE_ELEM_IS_EOF(lpMinMergeElem, TRUE);

	SET_MERGE_ELEM_CURRENT_VALUE(lpMinMergeElem, dwNextCurValue);

	return TRUE;
}

BOOL MergeFiles(HANDLE hFile, LPHANDLE lphMergeFiles, DWORD dwMergeFileCount)
{
	LPMERGEELEM lpMergeElemArray = CreateMergeElemArray(lphMergeFiles, dwMergeFileCount);
	if (!lpMergeElemArray)
		return FALSE;

	for (INT iSuccess; (iSuccess = IterateMergeElemArray(hFile, lpMergeElemArray, dwMergeFileCount)) != EOF;)
		if (iSuccess == FALSE)
			return FALSE;

	free(lpMergeElemArray);

	return TRUE;
}
