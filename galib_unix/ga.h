#if !defined GA_H
#define GA_H

#define FOREGROUND (0)
#define BACKGROUND (1)
#define COLOR_CODE(COLOR, GROUND) (((COLOR) + (GROUND) * 10 + 30)) 

enum COLOR {BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE, PLACEHOLDER, DEFAULT};


void pos(int row, int col);
void writec(int row, int col, int ch);
void writecc(int row, int col, int ch, int color);
void writes(int row, int col, const char *str);
void writesc(int row, int col, const char *str, int attr);
void fillc(int row, int col, int ch, int n);
void fillcc(int row, int col, int ch, int n, int attr);
void vfill(int row, int col, int ch, int n);
void vfillc(int row, int col, int ch, int n, int attr);
void frame(int row1, int col1, int row2, int col2);

#endif