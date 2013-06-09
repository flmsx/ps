#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* 1 -> red 
 * 2 -> green
 * 3 -> yellow
 * 4 -> blue
 * 5 -> purple/white
 * */
enum COLORS {RED, GREEN, YELLOW, BLUE, PURPLE};
enum DIRECTIONS {UP, DOWN, LEFT, RIGHT, NONE};

typedef struct map_t {
    int	count;
    struct group_t *groups;
} Map;

typedef struct group_t {
    int id;
    int color;
    int count;
    struct star_t **stars;
} Group;

typedef struct star_t {
    int color;
    int x;
    int y;
    struct group_t *group;
} Star;

#define left(star) ((star->x) ? *(&star - 1) : NULL)
#define right(star) ((star->x < x - 1) ? *(&star + 1) : NULL)
#define up(star) ((star->y) ? *(&star - x) : NULL)
#define down(star) ((star->y < y - 1) ? *(&star + x) : NULL)
//#define alone(star) 

void init_stars(int *stage, int x, int y, Star **stars)
{
    int i, j;
    stars = (Star**)calloc(x*y, sizeof(Star*));
    assert(stars);
    for (i = 0; i < y; i++) {
        for (j = 0; j < x; j++) {
            stars[i*x + j] = (Star*)malloc(sizeof(Star));
            assert(stars[i*x + j]);
            stars[i*x + j]->color = stage[i*x +j];
            stars[i*x + j]->y = i;
            stars[i*x + j]->x = j;
            stars[i*x + j]->group = NULL;
        }
    }
    //test (up down left right) macro
    //printf("right:%p, %p left:%p\n", right(stars[0]), stars[1], left(stars[0]));
    //printf("up:%p, %p down:%d\n", down(stars[9*10+0]), stars[1], up(stars[90])->color);
}

Group group_alone = {-1, 0, 1, NULL};

void init_group(Group *group)
{
    group->id = -1;
    group->count = 0;
    group->color = -1;
    group->stars = NULL;
}

#define is_alone(star)

void generate_map(int *stage, int x, int y, Map *map, Star **stars)
{
    int i, j;
    int group_id = -1;
    Group *group;

    init_stars(stage, x, y, stars);

    for (j = 0; j < y; j++) {
        for (i = 0; i < x; i++) {
            if (!stars[j*x + i]->group) {
                group = (Group*)malloc(sizeof(Group));
                assert(group);
                group->id = ++group_id;
                stars[j*x + i]->group = group;
                
            }
        }
    }
}

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
void print_stage(int *stage, int x, int y)
{
    int i, j;
    printf("  0 1 2 3 4 5 6 7 8 9 \n"); /* X axis */
    for (j=0; j<y; j++) {
        printf("%d ", j); /* Y axis */
        for (i=0; i<x; i++) {
            switch (*(stage+j*x+i)) {
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
    print_stage((int*)stage_test, 10 ,10);
    Star **stars = NULL;
    init_stars((int*)stage_test, 10, 10, stars);
    return 0;
}
