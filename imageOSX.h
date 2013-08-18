#ifndef IMAGEOSX_H
#define IMAGEOSX_H
#ifdef IOS
#import <UIKit/UIKit.h>
#else
#import <AppKit/AppKit.h>
#endif

enum star_color{POPED = 0, RED, YELLOW, BLUE, GREEN, PURPLE, END_COLOR=PURPLE};

typedef enum {
    P_None = 0x00000000,
    P_Bright = 0x00000001,
    P_Blink  = 0x00000002,
    P_Reverse  = 0x00000004
} StarPrintStyle;

void image2Board(const char* image, uint8_t *board);
void printStar(int color, int style, char shape);

#endif
