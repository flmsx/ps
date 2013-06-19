#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "shell.h"

/* 1 -> red 
 * 2 -> green
 * 3 -> yellow
 * 4 -> blue
 * 5 -> purple/white
 * */
enum COLORS {RED, GREEN, YELLOW, BLUE, PURPLE};
enum DIRECTIONS {UP, DOWN, LEFT, RIGHT, NONE};

#define MAP_CAPACITY 40
#define MAP_CAPACITY_STEP 5
typedef struct map_t {
    int	count;
	int capacity;
    struct group_t **groups;
} Map;

#define GROUP_CAPACITY 20
#define GROUP_CAPACITY_STEP 5
typedef struct group_t {
    int id;
    int color;
    int count;
	int capacity;
    struct star_t **stars;
} Group;

typedef struct star_t {
    int id;
    int color;
    int x;
    int y;
    struct group_t *group;
} Star;

Star star_null = {-1, -1, -1, -1, NULL};

#define left(star) ((star->x) ? *(&(star) - 1) : &star_null)
#define right(star) ((star->x < x - 1) ? *(&(star) + 1) : &star_null)
#define up(star) ((star->y) ? *(&(star) - x) : &star_null)
#define down(star) ((star->y < y - 1) ? *(&(star) + x) : &star_null)
//#define alone(star) 

Star** init_stars(int *stage, int x, int y)
{
    int i, j;
    Star **stars = (Star**)calloc(x*y, sizeof(Star*));
    assert(stars);
    for (i = 0; i < y; i++) {
        for (j = 0; j < x; j++) {
            stars[i*x + j] = (Star*)malloc(sizeof(Star));
            assert(stars[i*x + j]);
            stars[i*x + j]->id = i*x + j;
            stars[i*x + j]->color = stage[i*x +j];
            stars[i*x + j]->y = i;
            stars[i*x + j]->x = j;
            stars[i*x + j]->group = NULL;
        }
    }
    //test (up down left right) macro
    printf("right:%p, %p left:%p\n", right(stars[0]), stars[1], left(stars[0]));
    printf("up:%p, %p down:%d\n", down(stars[9*10+0]), stars[1], up(stars[90])->color);
	return stars;
}

//Group group_alone = {-1, 0, 1, NULL};

void init_group(Group *group)
{
    group->id = -1;
    group->count = 0;
    group->color = -1;
	group->capacity = GROUP_CAPACITY;
	group->stars = NULL;
	group->stars = (Star**)calloc(GROUP_CAPACITY, sizeof(Star*));
	assert(group->stars);
}

#define add_to_group(grp, star)																	\
	do {																						\
		star->group = grp;																		\
		if (++grp->count > grp->capacity) {														\
			grp->capacity += GROUP_CAPACITY_STEP;												\
			grp->stars = (Star**)realloc(grp->stars, grp->capacity * sizeof(Star*));			\
			assert(grp->stars);																	\
		}																						\
		grp->stars[star->group->count-1] = star;												\
	} while (0)

void find_next_group_member(Star **stars, int id, int x, int y)
{
    Star *left, *right, *down, *up;
    left = left(stars[id]); 
    right = right(stars[id]);
	down = down(stars[id]);
	up = up(stars[id]);

    if (!right->group && right->color == stars[id]->color) {
        add_to_group(stars[id]->group, right);
		find_next_group_member(stars, right->id, x, y);
    }

	if (!down->group && down->color == stars[id]->color) {
        add_to_group(stars[id]->group, down);
		find_next_group_member(stars, down->id, x, y);
    }

	if (!left->group && left->color == stars[id]->color) {
        add_to_group(stars[id]->group, left);
		find_next_group_member(stars, left->id, x, y);
    }

	if (!up->group && up->color == stars[id]->color) {
        add_to_group(stars[id]->group, up);
		find_next_group_member(stars, up->id, x, y);
    }  
}

void init_map(Map *map)
{
	map->count = 0;
	map->capacity = MAP_CAPACITY;
	map->groups = (Group**)calloc(MAP_CAPACITY, sizeof(Group*));
}

void generate_map(int x, int y, Map *map, Star **stars)
{
    int i, j;
    int group_id = -1;
    Group *group;

	init_map(map);

    for (j = 0; j < y; j++) {
        for (i = 0; i < x; i++) {
            if (!stars[j*x + i]->group) {
                group = (Group*)malloc(sizeof(Group));
                assert(group);
				init_group(group);
                group->id = ++group_id;
                stars[j*x + i]->group = group;
                
				if (++map->count > map->capacity) {
					map->capacity += MAP_CAPACITY_STEP;
					map->groups = (Group**)realloc(map->groups, map->capacity * sizeof(Group*));
					assert(map->groups);
				}
				map->groups[map->count-1] = group;
				find_next_group_member(stars, j*x + i, x, y);
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

SHELL_MK_CMD(print)
{
	print_stage((int*)stage_test, 10 ,10);
}

SHELL_MK_CMD(select)
{
	
}

shell_cmd_t cmds_table[] ={
    SHELL_CMD(print, "", "Print the (test) stage"),
    SHELL_CMD(select, "cc", "Select the group including star (x,y)")
};

int main()
{
    print_stage((int*)stage_test, 10 ,10);
    Star **stars = NULL;
    stars = init_stars((int*)stage_test, 10, 10);

	Map *map = (Map*)malloc(sizeof(Map));
	assert(map);
	generate_map(10, 10, map, stars);

	printf("stars = %p\n", stars);
    return 0;
}
