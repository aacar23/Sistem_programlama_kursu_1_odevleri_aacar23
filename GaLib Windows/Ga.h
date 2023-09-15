#if !defined GA_H
#define GA_H

#include <Windows.h>

BOOL Pos(INT iRow, INT iCol);
BOOL WriteC(INT iRow, INT iCol, INT ch);
BOOL WriteCC(INT iRow, INT iCol, INT ch, WORD wAttribute);
BOOL WriteS(INT iRow, INT iCol, LPSTR lpszStr);
BOOL WriteSC(INT iRow, INT iCol, LPCSTR lpcszStr, WORD wAttribute);
BOOL FillC(INT iRow, INT iCol, INT ch, INT n);
BOOL FillCC(INT iRow, INT iCol, INT ch, INT n, WORD wAttribute);
BOOL VFillC(INT iRow, INT iCol, INT ch, INT n);
BOOL VFillCC(INT iRow, INT iCol, INT ch, INT n, WORD wAttribute);
BOOL Frame(INT iRow1, INT iCol1, INT iRow2, INT iCol2);

#endif