#ifndef IMAGE_H
#define IMAGE_H

enum star_color{POPED = 0, RED, YELLOW, BLUE, GREEN, PURPLE, END_COLOR=PURPLE};

typedef enum {
    P_Bright = 0x00000001,
    P_Blink  = 0x00000002,
    P_Reverse  = 0x00000004
} StarPrintStyle;

void image2board(const char* image, unsigned char *board);
void print_star(int color, int style, char shape);

#endif
