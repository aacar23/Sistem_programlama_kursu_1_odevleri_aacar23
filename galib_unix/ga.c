#include "ga.h"

#include <stdio.h>
#include <stdlib.h>

#define MIN_ROW(row1, row2) ((row1) < (row2) ? (row1) : (row2))
#define MIN_COL(col1, col2) ((col1) < (col2) ? (col1) : (col2))

#define BOX_LL "\u2514" 
#define BOX_V  "\u2502"
#define BOX_H  "\u2500" 
#define BOX_UL "\u250C"
#define BOX_UR "\u2510"
#define BOX_LR "\u2518"

void pos(int row, int col)
{
	printf("\x1B[%d;%dH", row, col);
}

void writec(int row, int col, int ch)
{
	writecc(row, col, ch, COLOR_CODE(DEFAULT, FOREGROUND));
}

void writecc(int row, int col, int ch, int color)
{
	printf("\x1B[%d;%dH;%dm%c;", row, col, color, ch);
}

void writes(int row, int col, const char *str)
{
	writesc(row, col, str, COLOR_CODE(DEFAULT, FOREGROUND));
}

void writesc(int row, int col, const char *str, int attr)
{
	printf("\x1B[%d;%dH", row, col);
	printf("\x1B[%dm%s", attr, str);
}

void fillc(int row, int col, int ch, int n)
{
	fillcc(row, col, ch, n, COLOR_CODE(DEFAULT, FOREGROUND));
}

void fillcc(int row, int col, int ch, int n, int attr)
{
	printf("\x1B[%d;%dH", row, col);
	for (;n--;)
		printf("\x1B[%dm%c", attr, ch);
}

void vfill(int row, int col, int ch, int n)
{
	vfillc(row, col, ch, n, COLOR_CODE(DEFAULT, FOREGROUND));
}

void vfillc(int row, int col, int ch, int n, int attr)
{
	printf("\x1B[%d;%dH", row, col);
	for (;n--;){
		printf("\x1B[%dG", col);
		printf("\x1B[%dm%c\n", attr, ch);
	}
}

static void frame_set_origin(int row, int col)
{
	printf("\x1B[%d;%dH", row, col);
}

static void frame_head(int cols)
{
	printf(BOX_UL);
	for (int i = 0;i < cols - 2;++i){
		printf(BOX_H);
	}
	printf(BOX_UR);
}

static void frame_body(int col_origin, int rows, int cols)
{
	
	for (int i = 0;i < rows - 2;++i){
		printf("\x1B[%dG",col_origin);
		printf(BOX_V);
		for (int k = 0;k < cols - 2;++k){
			putchar(' ');
		}
		printf(BOX_V);
		putchar('\n');
	}
}

static void frame_end(int col_origin, int cols)
{
	printf("\x1B[%dG",col_origin);
	printf(BOX_LL);
	for (int i = 0;i < cols - 2;++i){
		printf(BOX_H);
	}
	printf(BOX_LR);
}

void frame(int row1, int col1, int row2, int col2)
{
	frame_set_origin(MIN_ROW(row1, row2), MIN_COL(col1, col2));

	int rows = abs(row1 - row2);
	int cols = abs(col1 - col2);

	frame_head(cols);
	putchar('\n');

	frame_body(MIN_COL(col1, col2), rows, cols);
	
	frame_end(MIN_COL(col1, col2), cols);
	putchar('\n');
}







