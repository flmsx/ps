#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "shell.h"

/* 1 -> red 
 * 2 -> green
 * 3 -> yellow
 * 4 -> blue
 * 5 -> purple/white
 * 0 -> black
 * */
enum COLORS {POPED, RED, GREEN, YELLOW, BLUE, PURPLE};
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
    int min_x, max_x, min_y, max_y;
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

typedef struct snapshot_t {
    Star **stars;
    int x;
    int y;
    Map *map;
} Snapshot;

#define left(star) ((star->x) ? *(&(star) - 1) : &star_null)
#define right(star) ((star->x < x - 1) ? *(&(star) + 1) : &star_null)
#define up(star) ((star->y) ? *(&(star) - x) : &star_null)
#define down(star) ((star->y < y - 1) ? *(&(star) + x) : &star_null)

Star** init_stars(int *stage, int x, int y)
{
    int i, j;
    Star **stars = (Star**)calloc(x*y, sizeof(Star*));
    assert(stars);
    for (j = 0; j < y; j++) {
        for (i = 0; i < x; i++) {
            stars[j*x+i] = (Star*)malloc(sizeof(Star));
            assert(stars[j*x+i]);
            stars[j*x+i]->id = j*x + i;
            stars[j*x+i]->color = stage[j*x +i];
            stars[j*x+i]->y = j;
            stars[j*x+i]->x = i;
            stars[j*x+i]->group = NULL;
        }
    }
	return stars;

    //test (up down left right) macro
    //printf("right:%p, %p left:%p\n", right(stars[0]), stars[1], left(stars[0]));
    //printf("up:%p, %p down:%d\n", down(stars[9*10+0]), stars[1], up(stars[90])->color);

}

//Group group_alone = {-1, 0, 1, NULL};

static void init_group(Group *group)
{
    group->id = -1;
    group->count = 0;
    group->color = -1;
    group->min_x = group->max_x = group->min_y = group->max_y = -1;
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

inline void find_next_group_member(Star **stars, int id, int x, int y)
{
    Star *left, *right, *down, *up;
    left = left(stars[id]); 
    right = right(stars[id]);
	down = down(stars[id]);
	up = up(stars[id]);
    Group *group = stars[id]->group;

    if (!right->group && right->color == stars[id]->color) {
        add_to_group(group, right);
        if (right->x > group->max_x) group->max_x = right->x;
		find_next_group_member(stars, right->id, x, y);
    }

	if (!down->group && down->color == stars[id]->color) {
        add_to_group(group, down);
        if (down->y > group->max_y) group->max_y = down->y;
		find_next_group_member(stars, down->id, x, y);
    }

	if (!left->group && left->color == stars[id]->color) {
        add_to_group(group, left);
        if (left->x < group->min_x) group->min_x = left->x;
		find_next_group_member(stars, left->id, x, y);
    }

	if (!up->group && up->color == stars[id]->color) {
        add_to_group(group, up);
        if (up->y < group->min_y) group->min_y = up->y;
		find_next_group_member(stars, up->id, x, y);
    }  
}

static void init_map(Map *map)
{
	map->count = 0;
	map->capacity = MAP_CAPACITY;
	map->groups = (Group**)calloc(MAP_CAPACITY, sizeof(Group*));
    assert(map->groups);
}

Map* generate_map(Star **stars, int x, int y)
{
    int i, j;
    int group_id = -1;
    Group *group;

    Map *map = (Map*)malloc(sizeof(Map));
	assert(map);

	init_map(map);

    for (j = 0; j < y; j++) {
        for (i = 0; i < x; i++) {
            if (!stars[j*x + i]->group) {
                group = (Group*)malloc(sizeof(Group));
                assert(group);
				init_group(group);
                group->id = ++group_id;
                group->color = stars[j*x + i]->color;
                group->count++;
                group->min_x = group->max_x = i;
                group->min_y = group->max_y = j;
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

    return map;
}

void map_destroy(Map *map)
{
}

Snapshot* take_snapshot(Star **stars, int x, int y, Map *map)
{
    int j, i;
    Snapshot *st = (Snapshot*)malloc(sizeof(Snapshot));
    assert(st);
    st->x = x;
    st->y = y;
    st->map = map;
    st->stars = (Star**)calloc(x*y, sizeof(Star*));
    assert(stars);
    for (j = 0; j < y; j++) {
        for (i = 0; i < x; i++) {
            st->stars[j*x+i] = (Star*)malloc(sizeof(Star));
            assert(stars[j*x + i]);
            st->stars[j*x+i] = memcpy(st->stars[j*x+i], stars[j*x+i], sizeof(Star));
        }
    }
}

void destroy_snapshot(Snapshot *st)
{
    
}

/* Retrun 0 if could be poped, otherwise -1 */
int pop(Star *star, Star **stars, int x, int y)
{
    Group* group = star->group;
    if (group->count == 1)
        return -1;
    
    int i, j, falling;
    for(i = group->min_x; i <= group->max_x; i++) {
        for (j = group->max_y, falling = 0; j>= group->min_y; j--) {
            if (stars[j*x + i]->group == group) {
                falling++;
                //stars[j*x + i]->color = POPED;
            } else {
                if (falling) {
                    //stars[(j+falling)*x + i]->color = stars[j*x + i]->color;
					//stars[(j+falling)*x + i]->group = stars[j*x + i]->group;
					stars[(j+falling)*x + i] = stars[j*x + i];
                    //stars[j*x + i]->color = POPED;
                }
            }
        }
        for(; j >= 0; j--) {
            //stars[(j+falling)*x + i]->color = stars[j*x + i]->color;
			stars[(j+falling)*x + i] = stars[j*x + i];
            //stars[j*x + i]->color = POPED;
        }
    }
    return 0;
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

typedef enum {
    P_Bright = 0x00000001,
    P_Blink  = 0x00000002,
    P_Reverse  = 0x00000004
} StarPrintStyle; 

void print_star(int color, int style, char shape)
{
    char *color_str;
    char format_str[50] = {0,};
    const char *close_style_str = "\e[0m";
    const char *blink_style_str = "\e[5m";
    const char *bright_style_str = "\e[1m";
    const char *reverse_style_str = "\e[7m";
    switch (color) {
    case 1:
        color_str = "\e[31m";
        break;
    case 2:
        color_str = "\e[32m";
        break;
    case 3:
        color_str = "\e[33m";
        break;
    case 4:
        color_str = "\e[34m";
        break;
    case 5:
        color_str = "\e[37m";
        break;
    case POPED:
        color_str = "\e[30m";
        break;
    default:
        color_str = "\e[30m";
        fprintf(stderr, "Unkonw color! Black displayed. \n");
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
    printf(format_str, shape);
}

#define STAR 22
void print_stage(int *stage, int x, int y)
{
    int i, j;
    printf("  0 1 2 3 4 5 6 7 8 9 \n"); /* X axis */
    for (j=0; j<y; j++) {
        printf("%d ", j); /* Y axis */
        for (i=0; i<x; i++) {
            print_star(*(stage+j*x+i), P_Bright, STAR);
        }
        printf("\n");
    }
}

void print_stars(Star **stars, int x, int y)
{
    int i, j;
    printf("  0 1 2 3 4 5 6 7 8 9 \n"); /* X axis */
    for (j=0; j<y; j++) {
        printf("%d ", j); /* Y axis */
        for (i=0; i<x; i++) {
            print_star(stars[j*x+i]->color, P_Bright, STAR);
        }
        printf("\n");
    }
}

void print_group(Star** stars, Group* group, int x, int y)
{
    int i, j;
    printf("  0 1 2 3 4 5 6 7 8 9 \n"); /* X axis */
    for (j=0; j<y; j++) {
        printf("%d ", j); /* Y axis */
        for (i=0; i<x; i++) {
            if (stars[j*x+i]->group == group)
                print_star(stars[j*x+i]->color, P_Bright|P_Reverse, STAR);
            else
                print_star(stars[j*x+i]->color, P_Bright, STAR);
        }
        printf("\n");
    }
    printf("Range: (%d,%d), (%d,%d)\n", group->min_x, group->min_y, group->max_x, group->max_y);
}


/* Shell commands */
SHELL_MK_CMD(print)
{
	print_stage((int*)stage_test, 10 ,10);
}

SHELL_MK_CMD(select)
{
    Star** stars= (Star**)shell->data;
	print_group(stars, stars[args[0].i + 10*args[1].i]->group, 10, 10);
}

SHELL_MK_CMD(pop)
{
    Star** stars= (Star**)shell->data;
	pop(stars[args[0].i + 10*args[1].i], stars, 10, 10);
    print_stars(stars, 10, 10);
}

shell_cmd_t shell_cmds_table[] = {
    SHELL_CMD(print, "", "Print the (test) stage"),
    SHELL_CMD(select, "ii", "Highlite the group where star (x,y) living in"),
    SHELL_CMD(pop, "ii", "pop the group where star (x,y) living in")
};

int main()
{
    /* Prepare Star things */
    print_stage((int*)stage_test, 10 ,10);

    Star **stars = init_stars((int*)stage_test, 10, 10);

	Map *map = generate_map(stars, 10, 10);
    
    /* Launch the shell */
    shell_t *shell = shell_new();
    shell->data = (void*)stars;

    int i;
    for (i = 0; i < sizeof(shell_cmds_table)/sizeof(shell_cmd_t); i++)
        shell_register_cmd(shell_cmds_table[i], shell);

    int ret = shell_run(shell);
    
    shell_destroy(shell);

    return ret;
}
