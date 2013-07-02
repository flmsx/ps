/* author fengmeng.linux@gmail.com
 * remarks Copyright (c) 2013. All rights reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include "shell.h"
#include "list.h"

/* 1 -> red 
 * 2 -> green
 * 3 -> yellow
 * 4 -> blue
 * 5 -> purple/white
 * 0 -> black
 * */
enum COLORS {POPED, RED, GREEN, YELLOW, BLUE, PURPLE};
enum DIRECTIONS {UP, DOWN, LEFT, RIGHT, NONE};

typedef struct map_t {
    struct star_t **stars;
    int	count; //group count
    struct list_head groups;
} Map;

typedef struct group_t {
    int id;
    int color;
    int count;
    int min_x, max_x, min_y, max_y; //occupied rectange
    struct list_head stars;
    struct list_node list_map;
} Group;

typedef struct star_t {
    int id;
    int color;
    int x;
    int y;
    struct group_t *group;
    struct list_node list_group;
    struct list_node list_falling;
    bool dirty; //not update after falling
} Star;

Star STAR_NULL = {-1, -1, -1, -1, NULL};

typedef struct snapshot_t {
    Star **stars;
    int x;
    int y;
    Map *map;
} Snapshot;

#define left(star) ((star->x) ? *(&(star) - 1) : &STAR_NULL)
#define right(star) ((star->x < x - 1) ? *(&(star) + 1) : &STAR_NULL)
#define up(star) ((star->y) ? *(&(star) - x) : &STAR_NULL)
#define down(star) ((star->y < y - 1) ? *(&(star) + x) : &STAR_NULL)

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
	list_head_init(&(group->stars));
}

#define add_to_group(grp, star)								\
	do {												    \
		star->group = grp;                                  \
        list_add_tail(&(grp->stars), &(star->list_group));  \
        grp->count++;                                       \
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
    list_head_init(&(map->groups));
}

Map* generate_map(Star **stars, int x, int y)
{
    int i, j;
    int group_id = -1;
    Group *group;

    Map *map = (Map*)malloc(sizeof(Map));
	assert(map);
    map->stars = stars;

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
                list_add_tail(&(map->groups), &(group->list_map));
                map->count++;
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
int pop(Star *star, Map *map, int x, int y)
{
    Group* group = star->group;
    if (group->count <= 1)
        return -1;

    Star **stars = map->stars;
    
    int i, j, old_j, falling;

    LIST_HEAD(falling_head);
    /* falling the stars , adding the changing stars to a list */
    for(i = group->min_x; i <= group->max_x; i++) {
        for (j = group->max_y, falling = 0; j >= group->min_y; j--) {
            if (stars[j*x + i]->group == group) {
                falling++;
                if (up(stars[j*x + i]) == &STAR_NULL) {
                    stars[j*x +i] = &STAR_NULL;
                    //goto next i
                    j = -1;
                } else 
                    stars[j*x +i] = &STAR_NULL;
            } else {
                if (falling) {
					stars[(j+falling)*x + i] = stars[j*x + i];
                    old_j = j;
                    if (up(stars[j*x + i]) == &STAR_NULL) 
                        j = -1;
                    stars[old_j*x + i] == &STAR_NULL;
                    //stars[(old_j+falling)*x + i]->y = old_j + falling;
                    //list_add_tail(&falling_head, &(stars[(old_j+falling)*x + i]->list_falling));
                }
            }
        }
        for(; j >= 0; j--) {
            //assert(stars[(j+falling)*x + i] == &STAR_NULL);
			stars[(j+falling)*x + i] = stars[j*x + i];
            old_j = j;
            if (up(stars[j*x + i]) == &STAR_NULL) 
                j = -1;
            stars[old_j*x +i] = &STAR_NULL;
            //stars[(old_j+falling)*x + i]->y = old_j + falling;
            //list_add_tail(&falling_head, &(stars[(old_j+falling)*x + i]->list_falling));
        }
        
    }

    /* update everything to correct the map
     * TODO: consider the *NULL* columns */
    Star *s;
    list_for_each(&falling_head, s, list_falling) {
                
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
    Star** stars= ((Map*)shell->data)->stars;
	print_group(stars, stars[args[0].i + 10*args[1].i]->group, 10, 10);
}

SHELL_MK_CMD(pop)
{
    Map *map= (Map*)shell->data;
	pop(map->stars[args[0].i + 10*args[1].i], map, 10, 10);
    print_stars(map->stars, 10, 10);
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
    shell->data = (void*)map;

    int i;
    for (i = 0; i < sizeof(shell_cmds_table)/sizeof(shell_cmd_t); i++)
        shell_register_cmd(shell_cmds_table[i], shell);

    int ret = shell_run(shell);
    
    shell_destroy(shell);

    return ret;
}
