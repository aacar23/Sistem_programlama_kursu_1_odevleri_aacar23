#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <ncurses.h>

#define error_exit() (error_exit(__func__))
#define TIME_PTR() (&(time_t){time(NULL)})
#define GET_CURRENT_TIME_TM() (localtime(TIME_PTR()))

void (error_exit)(const char *msg)
{
    static char time_str[100];

    strftime(time_str, 99, "%d %B %Y %H:%M:%S", GET_CURRENT_TIME_TM());
    fprintf(stderr, "%s:%s:%s", msg, strerror(errno), time_str);

    exit(EXIT_FAILURE);
}


void print_c_file_comments_in_bold(const char *c_file_name)
{
    FILE *c_file = fopen(c_file_name, "r");
    if (!c_file)
        error_exit();

    int attr_flag = 0;

    for (int ch, prev_ch = EOF;(ch = fgetc(c_file)) != EOF;prev_ch = ch){
        attr_flag = (prev_ch == '/' &&  ch == '*') ? 1 : (prev_ch == '*' && ch == '/') ? 0 : attr_flag;
        addch(ch | ((attr_flag) ? (A_BOLD) : (0)));
    }

    fclose(c_file);
}


int main(int argc, char **argv)
{
    initscr();
    for (;*++argv;print_c_file_comments_in_bold(*argv))
        ;

    printw("<press any key>");

    noecho();

    getch();

    endwin();
}