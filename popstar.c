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
    int poped_count;
    struct list_head poped;
} Map;

typedef struct group_t {
    int id;
    int color;
    int count;
    int min_x, max_x, min_y, max_y; //occupied rectange
    struct list_head stars;
    struct list_node list_map;
    bool dirty;
} Group;

typedef struct star_t {
    int id;
    int color;
    int x;
    int y;
    struct group_t *group;
    struct list_node list_group;
    struct list_node list_regroup;
    struct list_node list_falling;
    bool regrouped;
    bool dirty; //not update after falling
} Star;

Star STAR_NULL = {-1, -1, -1, -1, NULL};

typedef struct snapshot_t {
    Star **stars;
    int x;
    int y;
    Map *map;
} Snapshot;

#define left(star, stars) ((star->x) ? *(stars + star->y*x + star->x - 1) : &STAR_NULL)
#define right(star, stars) ((star->x < x - 1) ? *(stars + star->y*x + star->x + 1) : &STAR_NULL)
#define up(star, stars) ((star->y) ? *(stars + (star->y-1)*x + star->x) : &STAR_NULL)
#define down(star, stars) ((star->y < y - 1) ? *(stars + (star->y+1)*x + star->x) : &STAR_NULL)

/*
#define left_dirty(star) ((star->x) ? *(&(star) - 1) : &STAR_NULL)
#define right_dirty(star) ((star->x < x - 1) ? *(&(star) + 1) : &STAR_NULL)
#define up_dirty(star) ((star->y) ? *(&(star) - x) : &STAR_NULL)
#define down_dirty(star) ((star->y < y - 1) ? *(&(star) + x) : &STAR_NULL)
*/

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
            stars[j*x+i]->dirty = false;
            stars[j*x+i]->regrouped = false;
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
    group->dirty = false;
}

#define add_to_group(grp, star)								\
	do {												    \
		star->group = grp;                                  \
        list_add_tail(&(grp->stars), &(star->list_group));  \
        grp->count++;                                       \
	} while (0)

static inline void find_next_group_member(Star **stars, int id, int x, int y)
{
    Star *neighbors[] = {
        left(stars[id], stars),
        right(stars[id], stars),
	    down(stars[id], stars),
	    up(stars[id], stars),
        NULL
    };
    Star **s;

    Group *group = stars[id]->group;

    for (s = neighbors; *s; s++) {
        if (!(*s)->group && (*s)->color == stars[id]->color) {
            add_to_group(group, (*s));
            if ((*s)->x > group->max_x) 
				group->max_x = (*s)->x;
			else if ((*s)->x < group->min_x)
				group->min_x = (*s)->x;
			if ((*s)->y > group->max_y) 
				group->max_y = (*s)->y;
			else if ((*s)->y < group->min_y)
				group->min_y = (*s)->y;
	    	find_next_group_member(stars, (*s)->id, x, y);
        }
    }
}

static void init_map(Map *map)
{
	map->count = 0;
	map->poped_count = 0;
    list_head_init(&(map->groups));
    list_head_init(&(map->poped));
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
				list_add_tail(&group->stars, &stars[j*x + i]->list_group);
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


struct regroup_t {
        struct list_node list_regroup;
        int count;
        struct list_head head_stars;
    };

static inline void find_next_group_member_in_dirty_group(Star *star, struct list_head *h, Star **stars, int x, int y)
{
	Star *neighbors[] = {
        left(star, stars),
        right(star, stars),
	    down(star, stars),
	    up(star, stars),
        NULL
    };

    Star **s;

    for (s = neighbors; *s; s++) {
        if ( (*s)->group == star->group
				&& !(*s)->regrouped 
				&& (*s)->color == star->color) {
			list_add_tail(h, &(*s)->list_regroup);
			container_of(h, struct regroup_t, head_stars)->count++;
			(*s)->regrouped = true;
	    	find_next_group_member_in_dirty_group(*s, h, stars, x, y);
        }
    }
}

/* Retrun true if could be poped, otherwise false */
bool pop(Star *star, Map *map, int x, int y)
{
    Group* group = star->group;
    if (group->count <= 1)
        return false;

    Star **stars = map->stars;
    
    int i, j, j_save, falling;

    LIST_HEAD(falling_head);
    /* falling the stars , adding the changing stars to a list */
    for(i = group->min_x; i <= group->max_x; i++) {
        for (j = group->max_y, falling = 0; j >= group->min_y; j--) { //inner
            if (stars[j*x + i]->group == group) {
                falling++;
				j_save = j;
                if (up(stars[j*x + i], stars) == &STAR_NULL)
                    j = -1; //goto next i
                stars[j_save*x +i] = &STAR_NULL;
            } else {
                if (falling) {
					stars[(j+falling)*x + i] = stars[j*x + i];
                    j_save = j;
                    if (up(stars[j*x + i], stars) == &STAR_NULL) 
                        j = -1;
                    stars[j_save*x + i] = &STAR_NULL;
                    stars[(j_save+falling)*x + i]->y = j_save + falling;
                    stars[(j_save+falling)*x + i]->dirty = true;
            		stars[(j_save+falling)*x + i]->group->dirty = true;
                    list_add_tail(&falling_head, &stars[(j_save+falling)*x + i]->list_falling);
                }
            }
        }
        for(; j >= 0; j--) { //outer
            //assert(stars[(j+falling)*x + i] == &STAR_NULL);
			stars[(j+falling)*x + i] = stars[j*x + i];
            j_save = j;
            if (up(stars[j*x + i], stars) == &STAR_NULL)
                j = -1;
            stars[j_save*x +i] = &STAR_NULL;
            stars[(j_save+falling)*x + i]->y = j_save + falling;
            stars[(j_save+falling)*x + i]->dirty = true;
            stars[(j_save+falling)*x + i]->group->dirty = true;
            list_add_tail(&falling_head, &stars[(j_save+falling)*x + i]->list_falling);
        }
    }

    /* move the poped group to poped list */
    map->count--;
    list_del_from(&map->groups, &group->list_map);
    map->poped_count++;
    list_add_tail(&map->poped, &group->list_map);

    /* update everything to correct the map
     * TODO: consider the *NULL* columns */
    
    LIST_HEAD(regroup_list);
    int regroup_count;

    Group *g;
    Star *s;
    list_for_each(&map->groups, g, list_map) {
        if (!g->dirty) continue;
        
		/* re-group the stars in the dirty group, ignore the outside */
		regroup_count = 0;
        list_for_each(&g->stars, s, list_group) {
            if(s->regrouped) continue;
            struct regroup_t *regrp = (struct regroup_t *)malloc(sizeof(struct regroup_t));
			assert(regrp);
			regrp->count = 0;
			list_head_init(&regrp->head_stars);
			find_next_group_member_in_dirty_group(s, &regrp->head_stars, stars, x, y);
			list_add(&regroup_list, &regrp->list_regroup);
			regroup_count++;
        }
		//printf("group_id = %d, regroup_count = %d\n", g->id, regroup_count);
		printf("group_id = %d, (%d, %d), regroup_count = %d\n", g->id, (list_top(&g->stars, Star, list_group))->x, (list_top(&g->stars, Star, list_group))->y, regroup_count);
    }

    return true;
}

int stage_test0[][10] = {
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

int stage_test1[][10] = {
{1, 2, 2, 1, 3, 1, 2, 4, 2, 4},
{2, 2, 2, 4, 3, 2, 5, 2, 5, 1}, 
{3, 3, 4, 5, 5, 2, 2, 5, 2, 1}, 
{1, 4, 3, 3, 3, 2, 5, 1, 1, 5}, 
{2, 5, 4, 4, 4, 2, 3, 1, 1, 4}, 
{1, 1, 4, 5, 4, 4, 4, 2, 3, 5}, 
{1, 5, 4, 4, 1, 1, 1, 2, 2, 1}, 
{4, 1, 3, 2, 2, 2, 2, 4, 5, 2}, 
{2, 2, 4, 2, 2, 3, 1, 4, 2, 3}, 
{3, 4, 2, 4, 1, 2, 2, 3, 1, 4}
};

int stage_test[][10] = {
{1, 2, 2, 1, 3, 1, 2, 4, 2, 4},
{2, 2, 2, 4, 3, 2, 5, 2, 5, 1}, 
{3, 3, 4, 5, 5, 2, 2, 5, 2, 1}, 
{1, 3, 3, 3, 3, 2, 5, 1, 1, 5}, 
{2, 3, 4, 4, 4, 2, 3, 1, 1, 4}, 
{1, 3, 4, 5, 4, 4, 4, 2, 3, 5}, 
{1, 3, 4, 4, 1, 1, 1, 2, 2, 1}, 
{4, 3, 3, 2, 2, 2, 2, 4, 5, 2}, 
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
