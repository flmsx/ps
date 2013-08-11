/*
For ip5
#5AA2E7 blue
#FF41EF purple
#00E718 green
#D6CB00 yellow
#FF5D5A red

first star pos: (0, 409) 
(31, 31)
*/

//#include <Magick++.h>
#include <stdio.h>
#include <string.h>
#include "image.h"
//using namespace std;
//using namespace Magick;

#define BLUE_HEX    "#5AA2E7"
#define PURPLE_HEX  "#FF41EF"
#define GREEN_HEX   "#00E718"
#define YELLOW_HEX  "#D6CB00"
#define RED_HEX     "#FF5D5A" 

//#define STAR 22
#define STAR '*'

void print_star(int color, int style, char shape)
{
    const char *color_str;
    char format_str[50] = {0,};
    const char *close_style_str = "\e[0m";
    const char *blink_style_str = "\e[5m";
    const char *bright_style_str = "\e[1m";
    const char *reverse_style_str = "\e[7m";
    switch (color) {
    case RED:
        color_str = "\e[31m";
        break;
    case GREEN:
        color_str = "\e[32m";
        break;
    case YELLOW:
        color_str = "\e[33m";
        break;
    case BLUE:
        color_str = "\e[34m";
        break;
    case PURPLE:
        color_str = "\e[35m";
        break;
    case POPED:
        color_str = "\e[30m";
        break;
    default:
        color_str = "\e[30m";
        //fprintf(stderr, "Unkonw color! Black displayed. \n");
    }
    if (style & P_Bright)
        strcat(format_str, bright_style_str);
    if (style & P_Blink)
        strcat(format_str, blink_style_str);
    if (style & P_Reverse)
        strcat(format_str, reverse_style_str);
    strcat(format_str, color_str);
    strcat(format_str, "%c ");
    strcat(format_str, close_style_str);
#ifdef TEST
	printf("%s :", format_str);
#endif
    printf(format_str, shape);
}

#if 0
void image2board(const char* image, unsigned char *board)
{
    InitializeMagick(0);
    Image im(image);
    for (int j = 0; j < 10; j++) {
        for (int i = 0; i < 10; i++) {
            string color = (string)im.pixelColor(i*64+31, 409+j*64+31);
            if (color == BLUE_HEX)
                board[(j+1)*12+i+1] = BLUE;
            else if (color == RED_HEX)
                board[(j+1)*12+i+1] = RED; 
            else if (color == GREEN_HEX)
                board[(j+1)*12+i+1] = GREEN;
            else if (color == YELLOW_HEX)
                board[(j+1)*12+i+1] = YELLOW;
            else if (color == PURPLE_HEX)
                board[(j+1)*12+i+1] = PURPLE;
            else {
                printf("Something wrong when convert image to board!\n");
                exit(-1);
            }
        }
    }
}
#endif

#if 0
int main(int argc,char **argv)
{
    InitializeMagick(0);
    Image im(argv[1]);
    /*for (int j = 0; j < 1136; j++) {
        for (int i = 0; i < 640; i++)
            printf("%d , %d  %s", i, j, ((string)im.pixelColor(i,j)).c_str());
        printf("\n");
    }*/
    for (int j = 0; j < 10; j++) {
        for (int i = 0; i < 10; i++) {
            string color = (string)im.pixelColor(i*64+31, 409+j*64+31);
            if (color == BLUE_HEX)
                print_star(BLUE, P_Bright, STAR);
            else if (color == RED_HEX)
                print_star(RED, P_Bright, STAR);
            else if (color == GREEN_HEX)
                print_star(GREEN, P_Bright, STAR);
            else if (color == YELLOW_HEX)
                print_star(YELLOW, P_Bright, STAR);
            else if (color == PURPLE_HEX)
                print_star(PURPLE, P_Bright, STAR);
            else
                print_star(POPED, P_Bright, STAR);
        }
        printf("\n");
    }
    return 0;
}
#endif

#ifdef TEST
int main(int argc,char **argv)
{
	print_star(RED, P_Bright, 22);
}
#endif

