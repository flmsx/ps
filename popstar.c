#include <stdio.h>

/* 1 -> red 
 * 2 -> green
 * 3 -> yellow
 * 4 -> blue
 * 5 -> purple/white
 * */
typedef struct {
    int total,
    Group *groups
} Map;

typedef struct {
    int color,
    int total,
    Star *stars
} Group;

typedef struct {
    int color,
    int x,
    int y,
    Group *group
} Star;


int stage_test[][10] = {
{1, 2, 2, 1, 3, 1, 2, 4, 2, 4}, 
{2, 2, 2, 4, 3, 2, 5, 2, 5, 1}, 
{3, 3, 4, 5, 5, 2, 2, 5, 2, 1}, 
{1, 4, 3, 3, 3, 2, 5, 1, 1, 5}, 
{2, 5, 5, 4, 4, 2, 3, 1, 1, 4}, 
{1, 1, 4, 5, 4, 4, 4, 2, 3, 5}, 
{1, 5, 5, 3, 1, 1, 1, 2, 2, 1}, 
{4, 1, 3, 2, 2, 2, 2, 4, 5, 2}, 
{2, 2, 4, 2, 2, 3, 1, 4, 2, 3}, 
{3, 4, 2, 4, 1, 2, 2, 3, 1, 4}
};

#define STAR 22
void print_stage(int (*stage)[10])
{
    int i, j;
    printf("  0 1 2 3 4 5 6 7 8 9 \n"); /* X axis */
    for (i=0; i<10; i++) {
        printf("%d ", i); /* Y axis */
        for (j=0; j<10; j++) {
            switch (stage[i][j]) {
            case 1:
                printf("\e[31m\e[1m%c ", STAR);
                //printf("\e[41mR \e[0m ");
                break;
            case 2:
                printf("\e[32m\e[1m%c ", STAR);
                //printf("\e[42m\e[1mG \e[0m ");
                break;
            case 3:
                printf("\e[33m\e[1m%c ", STAR);
                //printf("\e[43m\e[1mY \e[0m ");
                break;
            case 4:
                printf("\e[34m\e[1m%c ", STAR);
                //printf("\e[44m\e[1mB \e[0m ");
                break;
            case 5:
                printf("\e[37m\e[1m%c ", STAR);
                //printf("\e[47m\e[1mW \e[0m ");
                break;
            default:
                printf("  ");
            }
        }
        printf("\n\e[0m");
    }
}

int main()
{
    print_stage(stage_test);
    return 0;
}
