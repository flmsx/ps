#include <stdio.h>

/* 1 -> red 
 * 2 -> green
 * 3 -> yellow
 * 4 -> blue
 * 5 -> purple/white
 * */

typedef struct map_t {
    int	count;
    struct group_t *groups;
} Map;

typedef struct group_t {
    int id;
    int color;
    int count;
    struct star_t **stars;
    int *x;
    int *y;
} Group;

struct star_t {
    int color;
    int x;
    int y;
    struct group_t *group;
} Star;

void init_stars(int *stage, int x, int y, Star **stars)
{
    int i, j;
    stars = (star**)calloc(x*y, sizeof(*Star));
    assert(stars);
    for (i = 0; i < y; i++) {
        (for j = 0; j < x; j++) {
            stars[i*x + j] = (Star*)malloc(sizeof(Star));
            assert(stars[i*x + j]);
            stars[i*x + j]->color = stage[i*x +j];
            stars[i*x + j]->y = i;
            stars[i*x + j]->x = j;
            stars[i*x + j]->group = NULL;
        }
    }
}

void init_group(Group *group)
{
    group->id = -1;
    group->count = 0;
    group->color = -1;
    group->stars = NULL;
}

void generate_map(int *stage, int x, int y, Map *map, Star **stars)
{
    int i, j;
    int group_id = -1;
    Group *group;

    init_stars(stage, x, y, stars);

    for (i = 0; i < y; i++) {
        (for j = 0; j < x; j++) {
            if (!stars[i*x + j]->group) {
                group = (Group*)malloc(sizeof(Group));
                assert(group);
                group->id = ++group_id;
                stars[i*x + j]->group = group;
                
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
    for (i=0; i<y; i++) {
        printf("%d ", i); /* Y axis */
        for (j=0; j<x; j++) {
            switch (*(stage+i*x+j)) {
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
    return 0;
}
