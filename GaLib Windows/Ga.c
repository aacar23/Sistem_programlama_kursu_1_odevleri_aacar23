#include "Ga.h"
#include "ExitSys.h"

#include <stdio.h>
#include <string.h>

#define INITIALIZE_CONSOLE_HANDLE(identifier, statement) HANDLE identifier = GetStdHandle(STD_OUTPUT_HANDLE); \
																do { \
																	if ((identifier) == INVALID_HANDLE_VALUE || !(identifier)) { \
																			statement; \
																	} \
																} while (0)

#define CFunc(funcName, hStdOutHandle, statement, ...) do {if (!funcName((hStdOutHandle), __VA_ARGS__)){statement;}}while (0)
#define CSetConsoleCursorPosition(hStdOutHandle, x, y, statement)	    CFunc(SetConsoleCursorPosition, (hStdOutHandle), statement, ((COORD){(x), (y)}))
#define CSetConsoleTextAttribute(hStdOutHandle, wAttribute, statement) CFunc(SetConsoleTextAttribute, (hStdOutHandle), statement, (wAttribute))
#define CWriteConsole(hStdOutHandle, buffer, numberOfBytes, numberOfBytesWritten, statement) CFunc(WriteConsole, (hStdOutHandle), statement, (buffer), (numberOfBytes), (numberOfBytesWritten), NULL)
#define CWriteStrToConsole(hStdOutHandle, lpcszString, statement) CWriteConsole((hStdOutHandle), (lpcszString), ((DWORD)strlen((lpcszString))), NULL, statement)
#define CWriteChToConsole(hStdOutHandle, Character, statement) CWriteConsole((hStdOutHandle), (&(CHAR){Character}), 1, NULL, statement)

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define ABS(expr) ((expr) > 0 ? (expr) : -(expr))

#define	BOX_LL	"\u2514"
#define	BOX_V	"\u2502"
#define	BOX_H	"\u2500"
#define	BOX_UL	"\u250C"
#define	BOX_UR	"\u2510"
#define	BOX_LR	"\u2518"


BOOL Pos(INT iRow, INT iCol)
{
	INITIALIZE_CONSOLE_HANDLE(hStdOutHandle, SetErrorStrLastError(__func__); goto FAIL);
	CSetConsoleCursorPosition(hStdOutHandle, iCol, iRow, goto FAIL);

	CloseHandle(hStdOutHandle);
	return TRUE;

FAIL:
	SetErrorStrLastError(__func__);
	CloseHandle(hStdOutHandle);
	return FALSE;
}

BOOL WriteC(INT iRow, INT iCol, INT ch)
{
	INITIALIZE_CONSOLE_HANDLE(hStdOutHandle, goto FAIL);
	CSetConsoleCursorPosition(hStdOutHandle, iCol, iRow, goto FAIL);
	CWriteConsole(hStdOutHandle, &(char)ch, 1, NULL, goto FAIL);

	CloseHandle(hStdOutHandle);
	return TRUE;

FAIL:
	SetErrorStrLastError(__func__);
	CloseHandle(hStdOutHandle);
	return FALSE;
}


BOOL WriteCC(INT iRow, INT iCol, INT ch, WORD wAttribute)
{
	INITIALIZE_CONSOLE_HANDLE(hStdOutHandle, goto FAIL);
	CSetConsoleCursorPosition(hStdOutHandle, iCol, iRow, goto FAIL);
	CSetConsoleTextAttribute(hStdOutHandle, wAttribute, goto FAIL);
	CWriteConsole(hStdOutHandle, &(char)ch, 1, NULL, goto FAIL);

	CloseHandle(hStdOutHandle);
	return TRUE;

FAIL:
	SetErrorStrLastError(__func__);
	CloseHandle(hStdOutHandle);
	return FALSE;
}

BOOL WriteS(INT iRow, INT iCol, LPSTR lpszStr)
{
	INITIALIZE_CONSOLE_HANDLE(hStdOutHandle, goto FAIL);
	CSetConsoleCursorPosition(hStdOutHandle, iCol, iRow, goto FAIL);
	CWriteConsole(hStdOutHandle, lpszStr, (DWORD)strlen(lpszStr), NULL, goto FAIL);

	CloseHandle(hStdOutHandle);
	return TRUE;

FAIL:
	SetErrorStrLastError(__func__);
	CloseHandle(hStdOutHandle);
	return FALSE;
}

BOOL WriteSC(INT iRow, INT iCol, LPCSTR lpcszStr, WORD wAttribute)
{
	INITIALIZE_CONSOLE_HANDLE(hStdOutHandle, goto FAIL);
	CSetConsoleCursorPosition(hStdOutHandle, iCol, iRow, goto FAIL);
	CSetConsoleTextAttribute(hStdOutHandle, wAttribute, goto FAIL);
	CWriteStrToConsole(hStdOutHandle, lpcszStr, goto FAIL);

	CloseHandle(hStdOutHandle);
	return TRUE;

FAIL:
	SetErrorStrLastError(__func__);
	CloseHandle(hStdOutHandle);
	return FALSE;
}

BOOL FillC(INT iRow, INT iCol, INT ch, INT n)
{
	INITIALIZE_CONSOLE_HANDLE(hStdOutHandle, goto FAIL);
	CSetConsoleCursorPosition(hStdOutHandle, iCol, iRow, goto FAIL);

	for (; n--;)
		CWriteChToConsole(hStdOutHandle, ch, goto FAIL);

	CloseHandle(hStdOutHandle);
	return TRUE;

FAIL:
	SetErrorStrLastError(__func__);
	CloseHandle(hStdOutHandle);
	return FALSE;
}

BOOL FillCC(INT iRow, INT iCol, INT ch, INT n, WORD wAttribute)
{
	INITIALIZE_CONSOLE_HANDLE(hStdOutHandle, goto FAIL);
	CSetConsoleCursorPosition(hStdOutHandle, iCol, iRow, goto FAIL);
	CSetConsoleTextAttribute(hStdOutHandle, wAttribute, goto FAIL);

	for (; n--;)
		CWriteChToConsole(hStdOutHandle, ch, goto FAIL);

	CloseHandle(hStdOutHandle);
	return TRUE;

FAIL:
	SetErrorStrLastError(__func__);
	CloseHandle(hStdOutHandle);
	return FALSE;
}

static VOID PrintNSpaces(INT n)
{
	for (INT i = 0; i < n; ++i)
		printf(" ");
}

BOOL VFillC(INT iRow, INT iCol, INT ch, INT n)
{
	INITIALIZE_CONSOLE_HANDLE(hStdOutHandle, goto FAIL);
	CSetConsoleCursorPosition(hStdOutHandle, iCol, iRow, goto FAIL);

	do{
		CWriteChToConsole(hStdOutHandle, ch, goto FAIL);
		printf("\n");
	} while (--n && (PrintNSpaces(iCol), 1));

	CloseHandle(hStdOutHandle);
	return TRUE;

FAIL:
	SetErrorStrLastError(__func__);
	CloseHandle(hStdOutHandle);
	return FALSE;
}

BOOL VFillCC(INT iRow, INT iCol, INT ch, INT n, WORD wAttribute)
{
	INITIALIZE_CONSOLE_HANDLE(hStdOutHandle, goto FAIL);
	CSetConsoleCursorPosition(hStdOutHandle, iCol, iRow, goto FAIL);
	CSetConsoleTextAttribute(hStdOutHandle, wAttribute, goto FAIL);

	do {
		CWriteChToConsole(hStdOutHandle, ch, goto FAIL);
		CWriteChToConsole(hStdOutHandle, '\n', goto FAIL);
	} while (--n && (PrintNSpaces(iCol), 1));

	

	CloseHandle(hStdOutHandle);
	return TRUE;

FAIL:
	SetErrorStrLastError(__func__);
	CloseHandle(hStdOutHandle);
	return FALSE;
}

static BOOL FrameHead(HANDLE hStdOutHandle, INT iRow1, INT iCol1, INT iRow2, INT iCol2)
{
	CSetConsoleCursorPosition(hStdOutHandle, MIN(iCol1, iCol2), MIN(iRow1, iRow2), goto FAIL);
	INT iCnt = ABS(iCol1 - iCol2) - 2;


	CWriteStrToConsole(hStdOutHandle, BOX_UL, goto FAIL);
	for (INT i = 0; i < iCnt; ++i)
		CWriteStrToConsole(hStdOutHandle, BOX_H, goto FAIL);
	CWriteStrToConsole(hStdOutHandle, BOX_UR, goto FAIL);
	CWriteChToConsole(hStdOutHandle, '\n', goto FAIL);

	return TRUE;

FAIL:
	SetErrorStrLastError(__func__);
	return FALSE;
}

static BOOL FrameBody(HANDLE hStdOutHandle, INT iRow1, INT iCol1, INT iRow2, INT iCol2)
{
	INT iRowCount = ABS(iRow1 - iRow2) - 2 ;
	INT iColCount = ABS(iCol1 - iCol2) - 2;
	INT iMinCol = MIN(iCol1, iCol2);
	for (INT i = 0; i < iRowCount; ++i) {
		PrintNSpaces(iMinCol);
		CWriteStrToConsole(hStdOutHandle, BOX_V, goto FAIL);
		for (INT k = 0; k < iColCount; ++k)
			CWriteChToConsole(hStdOutHandle, ' ', goto FAIL);
		CWriteStrToConsole(hStdOutHandle, BOX_V, goto FAIL);
		CWriteChToConsole(hStdOutHandle, '\n', goto FAIL);
	}
	PrintNSpaces(iMinCol);

	return TRUE;

FAIL:
	SetErrorStrLastError(__func__);
	return FALSE;
}

static BOOL FrameEnd(HANDLE hStdOutHandle, INT iRow1, INT iCol1 , INT iRow2, INT iCol2) {
	INT iColCount = ABS(iCol1 - iCol2) - 2;

	CWriteStrToConsole(hStdOutHandle, BOX_LL, goto FAIL);
	for (INT i = 0; i < iColCount; ++i) 
		CWriteStrToConsole(hStdOutHandle, BOX_H, goto FAIL);
	CWriteStrToConsole(hStdOutHandle, BOX_LR, goto FAIL);


	return TRUE;

FAIL:
	SetErrorStrLastError(__func__);
	return FALSE;
}


BOOL Frame(INT iRow1, INT iCol1, INT iRow2, INT iCol2)
{
	INITIALIZE_CONSOLE_HANDLE(hStdOutStream, CloseHandle(hStdOutStream); goto WINFAIL);
	if (!FrameHead(hStdOutStream, iRow1, iCol1, iRow2, iCol2))
		goto FUNC_FAIL;
	if (!FrameBody(hStdOutStream, iRow1, iCol1, iRow2, iCol2))
		goto FUNC_FAIL;
	if (!FrameEnd(hStdOutStream, iRow1, iCol1, iRow2, iCol2))
		goto FUNC_FAIL; 

	CloseHandle(hStdOutStream);
	return TRUE;

WINFAIL:
	CloseHandle(hStdOutStream);
	SetErrorStrLastError(__func__);
	return FALSE;
FUNC_FAIL:
	CloseHandle(hStdOutStream);
	PrependTagToErrorStr(__func__);
	return FALSE;
}

