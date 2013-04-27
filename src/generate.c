/* File: generate.c */

/* Purpose: Dungeon generation */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"
#define SAFE_MAX_ATTEMPTS 5000

int template_race;

/*
 * Note that Level generation is *not* an important bottleneck,
 * though it can be annoyingly slow on older machines...  Thus
 * we emphasize "simplicity" and "correctness" over "speed".
 *
 * This entire file is only needed for generating levels.
 * This may allow smart compilers to only load it when needed.
 *
 * Consider the "v_info.txt" file for vault generation.
 *
 * In this file, we use the "special" granite and perma-wall sub-types,
 * where "basic" is normal, "inner" is inside a room, "outer" is the
 * outer wall of a room, and "solid" is the outer wall of the dungeon
 * or any walls that may not be pierced by corridors.  Thus the only
 * wall type that may be pierced by a corridor is the "outer granite"
 * type.  The "basic granite" type yields the "actual" corridors.
 *
 * Note that we use the special "solid" granite wall type to prevent
 * multiple corridors from piercing a wall in two adjacent locations,
 * which would be messy, and we use the special "outer" granite wall
 * to indicate which walls "surround" rooms, and may thus be "pierced"
 * by corridors entering or leaving the room.
 *
 * Note that a tunnel which attempts to leave a room near the "edge"
 * of the dungeon in a direction toward that edge will cause "silly"
 * wall piercings, but will have no permanently incorrect effects,
 * as long as the tunnel can *eventually* exit from another side.
 * And note that the wall may not come back into the room by the
 * hole it left through, so it must bend to the left or right and
 * then optionally re-enter the room (at least 2 grids away).  This
 * is not a problem since every room that is large enough to block
 * the passage of tunnels is also large enough to allow the tunnel
 * to pierce the room itself several times.
 *
 * Note that no two corridors may enter a room through adjacent grids,
 * they must either share an entryway or else use entryways at least
 * two grids apart.  This prevents "large" (or "silly") doorways.
 *
 * To create rooms in the dungeon, we first divide the dungeon up
 * into "blocks" of 11x11 grids each, and require that all rooms
 * occupy a rectangular group of blocks.  As long as each room type
 * reserves a sufficient number of blocks, the room building routines
 * will not need to check bounds.  Note that most of the normal rooms
 * actually use 23x11 grids, and so reserve 33x11 grids.
 */


/*
 * Dungeon generation values
 */
#define DUN_UNUSUAL 194 /* Level/chance of unusual room (was 200) */
#define DUN_DEST    18  /* 1/chance of having a destroyed level */
#define SMALL_LEVEL 3   /* 1/chance of smaller size (3)*/
#define EMPTY_LEVEL 15  /* 1/chance of being 'empty' (15)*/
#define DARK_EMPTY  5   /* 1/chance of arena level NOT being lit (2)*/

/*
 * Dungeon tunnel generation values
 */
#define DUN_TUN_RND	10	/* Chance of random direction */
#define DUN_TUN_CHG	30	/* Chance of changing direction */
#define DUN_TUN_CON	15	/* Chance of extra tunneling */
#define DUN_TUN_PEN	25	/* Chance of doors at room entrances */
#define DUN_TUN_JCT	90	/* Chance of doors at tunnel junctions */

/*
 * Dungeon streamer generation values
 */
#define DUN_STR_DEN	5	/* Density of streamers */
#define DUN_STR_RNG	2	/* Width of streamers */
#define DUN_STR_MAG	3	/* Number of magma streamers */
#define DUN_STR_MC	11	/* Chance/1000 of treasure per magma */
#define DUN_STR_QUA	2	/* Number of quartz streamers */
#define DUN_STR_QC	25	/* Chance/1000 of treasure per quartz */
#define DUN_MOS_DEN	2	/* Density of moss streamers */
#define DUN_MOS_RNG	10	/* Width of moss streamers */
#define DUN_STR_MOS	2	/* Number of moss streamers */
#define DUN_LIC_DEN	3	/* Density of lichen streamers */
#define DUN_LIC_RNG	20	/* Width of lichen streamers */
#define DUN_STR_LIC	1	/* Number of lichen streamers */
#define DUN_WAT_DEN	15	/* Density of rivers */
#define DUN_WAT_RNG	3	/* Width of rivers */
#define DUN_STR_WAT	3	/* Max number of rivers */

/*
 * Dungeon treausre allocation values
 */
#define DUN_AMT_ROOM		9	/* Amount of objects for rooms */
#define DUN_AMT_ITEM		3	/* Amount of objects for rooms/corridors */
#define DUN_AMT_GOLD		3	/* Amount of treasure for rooms/corridors */
#define DUN_AMT_BARREL  5	/* Amount of barrels for rooms */

#define DUN_CON_BARREL	20 /* 1/chance of converting goo barrel to flaming barrel */

/*
 * Hack -- Dungeon allocation "places"
 */
#define ALLOC_SET_ANY			0x00	/* Anywhere */
#define ALLOC_SET_CORR			0x01	/* Hallway */
#define ALLOC_SET_ROOM			0x02	/* Room */
#define ALLOC_SET_WALL			0x04	/* Near a wall */
#define ALLOC_SET_WALL_NICE   0x08	/* Near good walls */
#define ALLOC_SET_IN_ROOM		0x10	/* Fully within a room */
#define ALLOC_SET_BARREL		0x20	/* Near a barrel */

/*
 * Hack -- Dungeon allocation "types"
 */
#define ALLOC_TYP_RUBBLE	1	/* Rubble */
#define ALLOC_TYP_TRAP		3	/* Trap */
#define ALLOC_TYP_GOLD		4	/* Gold */
#define ALLOC_TYP_OBJECT	5	/* Object */
#define ALLOC_TYP_BARREL	6	/* Barrel */


/*
 * The "size" of a "generation block" in grids
 */
#define BLOCK_HGT	11
#define BLOCK_WID	11

/*
 * Maximum numbers of rooms along each axis (currently 6x6)
 */
#define MAX_ROOMS_ROW	(MAX_HGT / BLOCK_HGT)
#define MAX_ROOMS_COL	(MAX_WID / BLOCK_WID)


/*
 * Bounds on some arrays used in the "dun_data" structure.
 * These bounds are checked, though usually this is a formality.
 */
#define CENT_MAX	100
#define DOOR_MAX	200
#define WALL_MAX	500
#define TUNN_MAX	900


/*
 * Maximal number of room types
 */
#define ROOM_MAX	12



/*
 * Room type information
 */

typedef struct room_data room_data;

struct room_data
{
	/* Required size in blocks */
	s16b dy1, dy2, dx1, dx2;

	/* Hack -- minimum level */
	s16b level;
};


/*
 * Structure to hold all "dungeon generation" data
 */

typedef struct dun_data dun_data;

struct dun_data
{
	/* Array of centers of rooms */
	sint cent_n;
	byte cent_y[CENT_MAX];
	byte cent_x[CENT_MAX];

	/* Array of possible door locations */
	sint door_n;
	byte door_y[DOOR_MAX];
	byte door_x[DOOR_MAX];

	/* Array of wall piercing locations */
	sint wall_n;
	byte wall_y[WALL_MAX];
	byte wall_x[WALL_MAX];

	/* Array of tunnel grids */
	sint tunn_n;
	byte tunn_y[TUNN_MAX];
	byte tunn_x[TUNN_MAX];

	/* Number of blocks along each axis */
	sint row_rooms;
	sint col_rooms;

	/* Array of which blocks are used */
	bool room_map[MAX_ROOMS_ROW][MAX_ROOMS_COL];

	/* Hack -- there is a pit/nest on this level */
	bool crowded;
};


/*
 * Dungeon generation data -- see "cave_gen()"
 */
static dun_data *dun;


/*
 * Array of room types (assumes 11x11 blocks)
 */
static room_data room[ROOM_MAX] =
{
	{  0, 0,  0, 0,  0 },		/*  0 = Nothing */
	{  0, 0, -1, 1,  1 },		/*  1 = Simple (33x11) */
	{  0, 0, -1, 1,  1 },		/*  2 = Overlapping (33x11) */
	{  0, 0, -1, 1,  3 },		/*  3 = Crossed (33x11) */
	{  0, 0, -1, 1,  3 },		/*  4 = Large (33x11) */
	{  0, 0, -1, 1,  5 },		/*  5 = Monster nest (33x11) */
	{  0, 0, -1, 1,  5 },		/*  6 = Monster pit (33x11) */
	{  0, 1, -1, 1,  5 },		/*  7 = Lesser vault (33x22) */
	{ -1, 2, -2, 3, 10 },		/*  8 = Greater vault (66x44) */
	{  0, 1,  0, 1,  1 }, 		/*  9 = Quest rooms (22x22) */
	{  0, 0,  0, 1,  7 },			/* 10 = Circular (22x11) */
	{  0, 0, -1, 1, 10 },			/* 11 = Acyclic maze (33x11) */
};


/*** Room building helper functions ***/


/*
 * Draw a grid of a given feature type
 */
void draw_grid_full(int y, int x, byte feat, byte info)
{
	cave_feat[y][x] = feat;
	cave_info[y][x] = info;

	/* Notice/Redraw */
	if (character_dungeon)
	{
		/* Notice */
		note_spot(y, x);

		/* Redraw */
		lite_spot(y, x);
	}
}


/*
 * Draw a grid of a given feature
 */
void draw_grid_feat(int y, int x, byte feat)
{
	cave_feat[y][x] = feat;

	/* Notice/Redraw */
	if (character_dungeon)
	{
		/* Notice */
		note_spot(y, x);

		/* Redraw */
		lite_spot(y, x);
	}
}


/*
 * Draw a grid of a given type
 */
void draw_grid_info(int y, int x, byte info)
{
	cave_info[y][x] = info;

	/* Notice/Redraw */
	if (character_dungeon)
	{
		/* Notice */
		note_spot(y, x);

		/* Redraw */
		lite_spot(y, x);
	}
}


/*
 * Draw a rectangle of a given feature type
 */
static void draw_rect_full(int y1, int x1, int y2, int x2, byte feat, byte info)
{
	int y = y1, x = x1;

	for(; y <= y2; y++)
	{
		draw_grid_full(y, x1, feat, info);
		draw_grid_full(y, x2, feat, info);
	}

	for(; x <= x2; x++)
	{
		draw_grid_full(y1, x, feat, info);
		draw_grid_full(y2, x, feat, info);
	}
}


/*
 * Draw a filled rectangle of a given feature type
 */
static void fill_rect_full(int y1, int x1, int y2, int x2, byte feat, byte info)
{
	int y, x;

	for(y = y1; y <= y2; y++)
	{
		for(x = x1; x <= x2; x++)
		{
			draw_grid_full(y, x, feat, info);
		}
	}
}


/*
 * Draw a rectangle of a given feature
 */
static void draw_rect_feat(int y1, int x1, int y2, int x2, byte feat)
{
	int y = y1, x = x1;

	for(; y <= y2; y++)
	{
		draw_grid_feat(y, x1, feat);
		draw_grid_feat(y, x2, feat);
	}

	for(; x <= x2; x++)
	{
		draw_grid_feat(y1, x, feat);
		draw_grid_feat(y2, x, feat);
	}
}


/*
 * Draw a filled rectangle of a given feature
 */
static void fill_rect_feat(int y1, int x1, int y2, int x2, byte feat)
{
	int y, x;

	for(y = y1; y <= y2; y++)
	{
		for(x = x1; x <= x2; x++)
		{
			draw_grid_feat(y, x, feat);
		}
	}
}


/*
 * Draw a rectangle of a given type
 */
static void draw_rect_info(int y1, int x1, int y2, int x2, byte info)
{
	int y = y1, x = x1;

	for(; y <= y2; y++)
	{
		draw_grid_info(y, x1, info);
		draw_grid_info(y, x2, info);
	}

	for(; x <= x2; x++)
	{
		draw_grid_info(y1, x, info);
		draw_grid_info(y2, x, info);
	}
}


/*
 * Draw a filled rectangle of a given type
 */
static void fill_rect_info(int y1, int x1, int y2, int x2, byte info)
{
	int y, x;

	for(y = y1; y <= y2; y++)
	{
		for(x = x1; x <= x2; x++)
		{
			/* Set the flags */
			draw_grid_info(y, x, info);
		}
	}
}


/*
 * Draw a box of two given feature types
 */
static void draw_rect_room(int y1, int x1, int y2, int x2, byte feat_main, byte feat_edge, byte info)
{
	/* Fill the inside */
	fill_rect_full(y1 + 1, x1 + 1, y2 - 1, x2 - 1, feat_main, info);

	/* Draw the border */
	draw_rect_full(y1, x1, y2, x2, feat_edge, info);
}


/* Convert a maze coordinate into a dungeon coordinate */
#define YPOS(y, y1)		((y1) + (y) * 2 + 1)
#define XPOS(x, x1)		((x1) + (x) * 2 + 1)


/*
 * Build an acyclic maze inside the given rectangle.
 * Construct the maze from a given pair of features.
 *
 * Note that the edge lengths should be odd.
 */
static void draw_maze(int y1, int x1, int y2, int x2, byte feat_wall, byte feat_path, byte info)
{
	int i, j;
	int ydim, xdim;
	int grids;

	int y, x;
	int ty, tx;
	int dy, dx;

	byte dir[4];
	byte dirs;

	/* Start with a solid granite rectangle */
	fill_rect_full(y1, x1, y2, x2, feat_wall, info);

	/* Calculate dimensions */
	ydim = (y2 - y1) / 2;
	xdim = (x2 - x1) / 2;

	/* Number of unexamined grids */
	grids = ydim * xdim - 1;

	/* Set the initial position */
	y = rand_int(ydim);
	x = rand_int(xdim);

	/* Place a floor here */
	draw_grid_feat(YPOS(y, y1), XPOS(x, x1), feat_path);

	/* Now build the maze */
	while(grids)
	{
		/* Only use maze grids */
		if (cave_feat[YPOS(y, y1)][XPOS(x, x1)] == feat_path)
		{
			/* Pick a target */
			ty = rand_int(ydim);
			tx = rand_int(xdim);

			while(TRUE)
			{
				dirs = 0;
				dy = 0;
				dx = 0;

				/* Calculate the dungeon position */
				j = YPOS(y, y1);
				i = XPOS(x, x1);

				/** Enumerate possible directions **/

				/* Up */
				if (y && (cave_feat[j - 2][i] == feat_wall)) dir[dirs++] = 1;

				/* Down */
				if ((y < ydim - 1) && (cave_feat[j + 2][i] == feat_wall)) dir[dirs++] = 2;

				/* Left */
				if (x && (cave_feat[j][i - 2] == feat_wall)) dir[dirs++] = 3;

				/* Right */
				if ((x < xdim - 1) && (cave_feat[j][i + 2] == feat_wall)) dir[dirs++] = 4;

				/* Dead end; go to the next valid grid */
				if (!dirs) break;

				/* Pick a random direction */
				switch(dir[rand_int(dirs)])
				{
					/* Move up */
					case 1:	dy = -1; break;

					/* Move down */
					case 2:	dy = 1;	break;

					/* Move left */
					case 3:	dx = -1; break;

					/* Move right */
					case 4:	dx = 1;	break;
				}

				/* Place floors */
				draw_grid_feat(j + dy, i + dx, feat_path);
				draw_grid_feat(j + dy * 2, i + dx * 2, feat_path);

				/* Advance */
				y += dy;
				x += dx;

				/* One less grid to examine */
				grids--;

				/* Check for completion */
				if ((y == ty) && (x == tx)) break;
			}
		}

		/* Find a new position */
		y = rand_int(ydim);
		x = rand_int(xdim);
	}
}


#undef YPOS
#undef XPOS


/*
 * Always picks a correct direction
 */
static void correct_dir(int *rdir, int *cdir, int y1, int x1, int y2, int x2)
{
	/* Extract vertical and horizontal directions */
	*rdir = (y1 == y2) ? 0 : (y1 < y2) ? 1 : -1;
	*cdir = (x1 == x2) ? 0 : (x1 < x2) ? 1 : -1;

	/* Never move diagonally */
	if (*rdir && *cdir)
	{
		if (rand_int(100) < 50)
		{
			*rdir = 0;
		}
		else
		{
			*cdir = 0;
		}
	}
}


/*
 * Pick a random direction
 */
static void rand_dir(int *rdir, int *cdir)
{
	/* Pick a random direction */
	int i = rand_int(4);

	/* Extract the dy/dx components */
	*rdir = ddy_ddd[i];
	*cdir = ddx_ddd[i];
}


/*
 * Returns random co-ordinates for player/monster/object
 */
static bool new_player_spot(void)
{
	int	y, x;
	int max_attempts = 5000;

	/* Place the player */
	while (max_attempts--)
	{
		/* Pick a legal spot */
		y = rand_range(1, cur_hgt - 2);
		x = rand_range(1, cur_wid - 2);

		/* Must be a "naked" floor grid */
		if (!cave_naked_bold(y, x)) continue;

		/* Refuse to start on anti-teleport grids */
		if (cave_info[y][x] & (CAVE_ICKY)) continue;

		/* Done */
		break;

	}

	if (max_attempts < 1) /* Should be -1, actually if we failed... */
		return FALSE;


	/* Save the new player grid */
   player_place(y, x);

	return TRUE;
}


typedef bool (*grid_test_func)(int y, int x);


/*
 * Return TRUE if this grid contains a wall
 */
static bool is_hard_wall(int y, int x)
{
	/* Paranoia */
	if (!in_bounds(y, x)) return (FALSE);

	switch (cave_feat[y][x])
	{
		case FEAT_WALL_EXTRA:
		case FEAT_WALL_INNER:
		case FEAT_WALL_OUTER:
		case FEAT_WALL_SOLID:
		case FEAT_PERM_EXTRA:
		case FEAT_PERM_INNER:
		case FEAT_PERM_OUTER:
		case FEAT_PERM_SOLID:
		case FEAT_CRUSHER_UP:
		case FEAT_CRUSHER_DOWN:
		{
			/* A wall */
			return (TRUE);
		}
	}

	/* Not a wall */
	return (FALSE);
}


/*
 * Return TRUE if this grid contains a barrel
 */
static bool is_barrel(int y, int x)
{
	/* Paranoia */
	if (!in_bounds(y, x)) return (FALSE);

	switch (cave_feat[y][x])
	{
		case FEAT_BARREL_GOO:
		case FEAT_BARREL_FLAMING:
		{
			/* A barrel */
			return (TRUE);
		}
	}

	/* Not a barrel */
	return (FALSE);
}


/*
 * Return TRUE if this grid is part of a room
 */
static bool is_room(int y, int x)
{
	/* Paranoia */
	if (!in_bounds(y, x)) return (FALSE);

	/* A room */
	if (cave_info[y][x] & (CAVE_ROOM)) return (TRUE);

	/* Not a room */
	return (FALSE);
}


/*
 * Count the number of features orthogonally adjacent to the given grid.
 */
static int next_to_grid_ort(int y, int x, grid_test_func test)
{
	int k = 0;

	if (test(y + 1, x)) k++;
	if (test(y - 1, x)) k++;

	if (test(y, x + 1)) k++;
	if (test(y, x - 1)) k++;

	return (k);
}


/*
 * Count the number of walls adjacent to the given grid.
 */
static int next_to_grid_adj(int y, int x, grid_test_func test)
{
	int k = 0;
	int cy, cx;

	for (cy = y - 1; cy <= y + 1; cy++)
	{
		for (cx = x - 1; cx <= x + 1; cx++)
		{
			/* Ignore central grid */
			if ((cy == y) && (cx == x)) continue;

			/* Count adjacent features */
			if (test(cy, cx)) k++;
		}
	}

	return (k);
}


/*
 * Convert existing terrain type to rubble
 */
static void place_rubble(int y, int x)
{
	/* Create rubble */
	draw_grid_feat(y, x, FEAT_RUBBLE);
}


/*
 * Convert existing terrain type to barrel
 */
static void place_barrel(int y, int x)
{
	/* Usually create barrel of goo */
	if (rand_int(DUN_CON_BARREL))
	{
		draw_grid_feat(y, x, FEAT_BARREL_GOO);
	}
	/* Occasionally make a flaming barrel */
	else
	{
		draw_grid_feat(y, x, FEAT_BARREL_FLAMING);
	}
}


/*
 * Convert existing terrain type to "up stairs"
 */
static void place_up_stairs(int y, int x)
{
	/* Create up stairs */
	draw_grid_feat(y, x, FEAT_LESS);
}


/*
 * Convert existing terrain type to "down stairs"
 */
static void place_down_stairs(int y, int x)
{
	/* Create down stairs */
	draw_grid_feat(y, x, FEAT_MORE);
}


/*
 * Place an up/down staircase at given location
 */
static void place_random_stairs(int y, int x)
{
	/* Paranoia */
	if (!cave_clean_bold(y, x)) return;

	/* Choose a staircase */
	if (!p_ptr->depth)
	{
		place_down_stairs(y, x);
	}
	else if (is_quest(p_ptr->depth) || (p_ptr->depth >= MAX_DEPTH-1))
	{
		place_up_stairs(y, x);
	}
	else if (rand_int(100) < 50)
	{
		place_down_stairs(y, x);
	}
	else
	{
		place_up_stairs(y, x);
	}
}


/*
 * Place a locked door at the given location
 */
static void place_locked_door(int y, int x)
{
	/* Create locked door */
	draw_grid_feat(y, x, FEAT_DOOR_HEAD + randint(7));
}


/*
 * Place a secret door at the given location
 */
static void place_secret_door(int y, int x)
{
	/* Create secret door */
	draw_grid_feat(y, x, FEAT_SECRET);
}


/*
 * Place a random type of door at the given location
 */
static void place_random_door(int y, int x)
{
	int tmp;

	/* Choose an object */
	tmp = rand_int(1000);

	/* Open doors (300/1000) */
	if (tmp < 300)
	{
		/* Create open door */
		draw_grid_feat(y, x, FEAT_OPEN);
	}

	/* Broken doors (100/1000) */
	else if (tmp < 400)
	{
		/* Create broken door */
		draw_grid_feat(y, x, FEAT_BROKEN);
	}

	/* Secret doors (200/1000) */
	else if (tmp < 600)
	{
		/* Create secret door */
		draw_grid_feat(y, x, FEAT_SECRET);
	}

	/* Closed doors (300/1000) */
	else if (tmp < 900)
	{
		/* Create closed door */
		draw_grid_feat(y, x, FEAT_DOOR_HEAD + 0x00);
	}

	/* Locked doors (99/1000) */
	else if (tmp < 999)
	{
		/* Create locked door */
		draw_grid_feat(y, x, FEAT_DOOR_HEAD + randint(7));
	}

	/* Stuck doors (1/1000) */
	else
	{
		/* Create jammed door */
		draw_grid_feat(y, x, FEAT_DOOR_HEAD + 0x08 + rand_int(8));
	}
}


/*
 * Places some staircases near walls
 */
static void alloc_stairs(int feat, int num, int walls)
{
	int		y, x, i, j, flag;

	/* Place "num" stairs */
	for (i = 0; i < num; i++)
	{
		/* Place some stairs */
		for (flag = FALSE; !flag; )
		{
			/* Try several times, then decrease "walls" */
			for (j = 0; !flag && j <= 3000; j++)
			{
				/* Pick a random grid */
				y = rand_int(cur_hgt);
				x = rand_int(cur_wid);

				/* Require "naked" floor grid */
				if (!cave_naked_bold(y, x)) continue;

				/* Require a certain number of adjacent walls */
				if (next_to_grid_ort(y, x, is_hard_wall) < walls) continue;

				/* Town -- must go down */
				if (!p_ptr->depth)
				{
					/* Clear previous contents, add down stairs */
					draw_grid_feat(y, x, FEAT_MORE);
				}

				/* Quest -- must go up */
				else if (is_quest(p_ptr->depth) || (p_ptr->depth >= MAX_DEPTH-1))
				{
					/* Clear previous contents, add up stairs */
					draw_grid_feat(y, x, FEAT_LESS);
				}

				/* Requested type */
				else
				{
					/* Clear previous contents, add stairs */
					draw_grid_feat(y, x, feat);
				}

				/* All done */
				flag = TRUE;
			}

			/* Require fewer walls */
			if (walls) walls--;
		}
	}
}


/*
 * Allocates some objects (using "place" and "type")
 */
static void alloc_object(byte set, int typ, int num)
{
	int y, x, k, walls = 0;
	int dummy = 0;

	/* Place some objects */
	for (k = 0; k < num; k++)
	{
		/* Pick a "legal" spot */
		while (dummy < SAFE_MAX_ATTEMPTS)
		{
			bool room;

			dummy++;

			/* Location */
			y = rand_int(cur_hgt);
			x = rand_int(cur_wid);

			/* Require "naked" floor grid */
			if (!cave_naked_bold(y, x)) continue;

			/* Accept */
			if (set == ALLOC_SET_ANY) break;

			/* Check for "room" */
			room = (cave_info[y][x] & (CAVE_ROOM)) ? TRUE : FALSE;

			/* Count walls */
			if (set & (ALLOC_SET_WALL | ALLOC_SET_WALL_NICE))
			{
				walls = next_to_grid_ort(y, x, is_hard_wall);
			}

			/* Require corridor? */
			if ((set & (ALLOC_SET_CORR)) && room) continue;

			/* Require room? */
			if ((set & (ALLOC_SET_ROOM)) && !room) continue;

			/* Require fully within room? */
			if ((set & (ALLOC_SET_IN_ROOM)) && (next_to_grid_adj(y, x, is_room) < 8)) continue;

			/* Allow barrels? */
			if ((set & (ALLOC_SET_BARREL)) && next_to_grid_adj(y, x, is_barrel)) break;

			/* Require a wall? */
			if ((set & (ALLOC_SET_WALL)) && !walls) continue;

			/* Require appropriate walls? */
			if ((set & (ALLOC_SET_WALL_NICE)) && (walls > 2)) continue;

			/* Accept it */
			break;
		}

		if (dummy >= SAFE_MAX_ATTEMPTS)
		{
			if (cheat_room)
			{
				msg_print("Warning! Could not place object!");
			}
		return;
		}


		/* Place something */
		switch (typ)
		{
			case ALLOC_TYP_RUBBLE:
			{
				place_rubble(y, x);
				break;
			}

			case ALLOC_TYP_TRAP:
			{
				place_trap(y, x);
				break;
			}

			case ALLOC_TYP_GOLD:
			{
				place_gold(y, x);
				break;
			}

			case ALLOC_TYP_OBJECT:
			{
				place_object(y, x, FALSE, FALSE);
				break;
			}

			case ALLOC_TYP_BARREL:
			{
				place_barrel(y, x);
				break;
			}
		}
	}
}


/* Redefinable streamer feature hook */
static int (*streamer_feat_hook)(int feat, int chance) = NULL;


/*
 * Streamer feature hook - moss
 *
 * Place mossy floors on floors, and mossy walls on walls.
 */
static int streamer_feat_hook_moss(int feat, int chance)
{
	switch(feat)
	{
		case FEAT_FLOOR:
		{
			/* Mossy floor */
			return FEAT_FLOOR_MOSS;
		}
		case FEAT_WALL_OUTER:
		case FEAT_WALL_SOLID:
		{
			/* Mossy wall */
			return FEAT_WALL_MOSS;
		}
	}

	/* Not okay */
	return FEAT_NONE;
}


/*
 * Streamer feature hook - lichen
 *
 * Place lichen floors on floors, and lichen walls on walls.
 */
static int streamer_feat_hook_lichen(int feat, int chance)
{
	switch(feat)
	{
		case FEAT_FLOOR:
		{
			/* Lichen floor */
			return FEAT_FLOOR_LICHEN;
		}
		case FEAT_WALL_OUTER:
		case FEAT_WALL_SOLID:
		{
			/* Lichen wall */
			return FEAT_WALL_LICHEN;
		}
	}

	/* Not okay */
	return FEAT_NONE;
}


/*
 * Streamer feature hook - magma vein
 *
 * Place magma veins in granite walls.
 */
static int streamer_feat_hook_magma(int feat, int chance)
{
	switch(feat)
	{
		case FEAT_WALL_EXTRA:
		case FEAT_WALL_INNER:
		case FEAT_WALL_OUTER:
		case FEAT_WALL_SOLID:
		{
			/* Magma vein with known treasure */
			if (rand_int(1000) < chance) return FEAT_MAGMA_K;

			/* Magma vein */
			else return FEAT_MAGMA;
		}
	}

	/* Not okay */
	return FEAT_NONE;
}


/*
 * Streamer feature hook - quartz vein
 *
 * Place quartz veins in granite walls.
 */
static int streamer_feat_hook_quartz(int feat, int chance)
{
	switch(feat)
	{
		case FEAT_WALL_EXTRA:
		case FEAT_WALL_INNER:
		case FEAT_WALL_OUTER:
		case FEAT_WALL_SOLID:
		{
			/* Magma vein with known treasure */
			if (rand_int(1000) < chance) return FEAT_QUARTZ_K;

			/* Magma vein */
			else return FEAT_QUARTZ;
		}
	}

	/* Not okay */
	return FEAT_NONE;
}



/*
 * Places "streamers" of rock through dungeon
 *
 * Note that their are actually six different terrain features used
 * to represent streamers.  Three each of magma and quartz, one for
 * basic vein, one with hidden gold, and one with known gold.  The
 * hidden gold types are currently unused.
 */
static void build_streamer(int chance, int density, int range)
{
	int i, tx, ty;
	int y, x, dir;

	int feat;

	/* Paranoia */
	if (!streamer_feat_hook) return;

	/* Hack -- Choose starting point */
	y = rand_spread(cur_hgt / 2, cur_hgt / 2 - 10);
	x = rand_spread(cur_wid / 2, cur_wid / 2 - 10);

	/* Choose a random compass direction */
	dir = ddd[rand_int(8)];

	/* Place streamer into dungeon */
	while (TRUE)
	{
		/* One grid per density */
		for (i = 0; i < density; i++)
		{
			/* Pick a nearby grid */
			while (1)
			{
				ty = rand_spread(y, range);
				tx = rand_spread(x, range);
				if (!in_bounds(ty, tx)) continue;
				if (distance(y, x, ty, tx) > range) continue;
				break;
			}

			/* Examine this feature */
			feat = (*streamer_feat_hook)(cave_feat[ty][tx], chance);

			/* Skip terrain if requested */
			if (feat == FEAT_NONE) continue;

			/* Clear previous contents, add proper feature */
			draw_grid_feat(ty, tx, feat);
		}

		/* Advance the streamer */
		y += ddy[dir];
		x += ddx[dir];

		/* Stop at dungeon edge */
		if (!in_bounds(y, x)) break;
	}
}


/*
 * Build a destroyed level
 */
static void destroy_level(void)
{
	int y1, x1, y, x, k, t, n;


	/* Note destroyed levels */
	if (cheat_room) msg_print("Destroyed Level");

	/* Drop a few epi-centers (usually about two) */
	for (n = 0; n < randint(5); n++)
	{
		/* Pick an epi-center */
		x1 = rand_range(5, cur_wid-1 - 5);
		y1 = rand_range(5, cur_hgt-1 - 5);

		/* Big area of affect */
		for (y = (y1 - 15); y <= (y1 + 15); y++)
		{
			for (x = (x1 - 15); x <= (x1 + 15); x++)
			{
				/* Skip illegal grids */
				if (!in_bounds_fully(y, x)) continue;

				/* Extract the distance */
				k = distance(y1, x1, y, x);

				/* Stay in the circle of death */
				if (k >= 16) continue;

				/* Delete the monster (if any) */
				delete_monster(y, x);

				/* Destroy valid grids */
				if (cave_valid_bold(y, x))
				{
					/* Delete objects */
					delete_object(y, x);

					/* Wall (or floor) type */
					t = rand_int(200);

					/* Granite */
					if (t < 20)
					{
						/* Create granite wall */
						draw_grid_feat(y, x, FEAT_WALL_EXTRA);
					}

					/* Quartz */
					else if (t < 70)
					{
						/* Create quartz vein */
						draw_grid_feat(y, x, FEAT_QUARTZ);
					}

					/* Magma */
					else if (t < 100)
					{
						/* Create magma vein */
						draw_grid_feat(y, x, FEAT_MAGMA);
					}

					/* Floor */
					else
					{
						/* Create floor */
						draw_grid_feat(y, x, FEAT_FLOOR);
					}

					/* No longer part of a room or vault */
					cave_info[y][x] &= ~(CAVE_ROOM | CAVE_ICKY);

					/* No longer illuminated */
					cave_info[y][x] &= ~(CAVE_GLOW);
				}
			}
		}
	}
}



/*
 * Create up to "num" objects near the given coordinates
 * Only really called by some of the "vault" routines.
 */
static void vault_objects(int y, int x, int num)
{
	int i, j, k;

	/* Attempt to place 'num' objects */
	for (; num > 0; --num)
	{
		/* Try up to 11 spots looking for empty space */
		for (i = 0; i < 11; ++i)
		{
			/* Pick a random location */
			while (1)
			{
				j = rand_spread(y, 2);
				k = rand_spread(x, 3);
				if (!in_bounds(j, k)) continue;
				break;
			}

			/* Require "clean" floor space */
			if (!cave_clean_bold(j, k)) continue;

			/* Place an item */
			if (rand_int(100) < 75)
			{
				place_object(j, k, FALSE, FALSE);
			}

			/* Place gold */
			else
			{
				place_gold(j, k);
			}

			/* Placement accomplished */
			break;
		}
	}
}


/*
 * Place a trap with a given displacement of point
 */
static void vault_trap_aux(int y, int x, int yd, int xd)
{
	int count, y1, x1;

	/* Place traps */
	for (count = 0; count <= 5; count++)
	{
		/* Get a location */
		while (1)
		{
			y1 = rand_spread(y, yd);
			x1 = rand_spread(x, xd);
			if (!in_bounds(y1, x1)) continue;
			break;
		}

		/* Require "naked" floor grids */
		if (!cave_naked_bold(y1, x1)) continue;

		/* Place the trap */
		place_trap(y1, x1);

		/* Done */
		break;
	}
}


/*
 * Place some traps with a given displacement of given location
 */
static void vault_traps(int y, int x, int yd, int xd, int num)
{
	int i;

	for (i = 0; i < num; i++)
	{
		vault_trap_aux(y, x, yd, xd);
	}
}


/*
 * Hack -- Place some sleeping monsters near the given location
 */
static void vault_monsters(int y1, int x1, int num)
{
	int k, i, y, x;

	/* Try to summon "num" monsters "near" the given location */
	for (k = 0; k < num; k++)
	{
		/* Try nine locations */
		for (i = 0; i < 9; i++)
		{
			int d = 1;

			/* Pick a nearby location */
			scatter(&y, &x, y1, x1, d, 0);

			/* Require "empty" floor grids */
			if (!cave_empty_bold(y, x)) continue;

			/* Place the monster (allow groups) */
			monster_level = p_ptr->depth + 2;
			(void)place_monster(y, x, TRUE, TRUE);
			monster_level = p_ptr->depth;
		}
	}
}


/*
 * Room building routines.
 *
 * Six basic room types:
 *   1 -- normal
 *   2 -- overlapping
 *   3 -- cross shaped
 *   4 -- large room with features
 *   5 -- monster nests
 *   6 -- monster pits
 *   7 -- simple vaults
 *   8 -- greater vaults
 */


/*
 * Type 1 -- normal rectangular rooms
 */
static void build_type1(int yval, int xval)
{
	int y, x, y2, x2;
	int y1, x1;

	int vault_int = rand_range(2, 4);
	int y_off = 0;
	int x_off = 0;
	int t;

	bool light = (p_ptr->depth <= randint(25));
	byte flag = (light ? (CAVE_ROOM | CAVE_GLOW) : CAVE_ROOM);

	bool vaulted = (rand_int(100) < 10);
	bool ragged = (rand_int(100) < 5);

	/* Pick a good room size */
	if (vaulted)
	{
		/* Pick number of pillars */
		int y_num = rand_range(2, 9 / vault_int);
		int x_num = rand_range(2, 23 / vault_int);

		/* Room dimensions */
		int dy = y_num * vault_int;
		int dx = x_num * vault_int;

		/* Pillar offsets - 50% chance of staggered pillars */
		y_off = ((y_num == 2) || (vault_int % 2) ||
				  (rand_int(100) >= 29)) ? 0 : (2 * vault_int);
		x_off = ((x_num == 2) || (vault_int % 2) ||
				  (rand_int(100) >= 29)) ? 0 : (2 * vault_int);

		/* Disallow both */
		if (y_off && x_off)
		{
			if (rand_int(100) < 50) y_off = 0;
			else x_off = 0;
		}

		/* Calculate distance */
		y1 = yval - (dy / 2 - 1);
		y2 = yval + (dy / 2 - 1 + (dy % 2));
		x1 = xval - (dx / 2 - 1);
		x2 = xval + (dx / 2 - 1 + (dx % 2));
	}
	/* Pick a good room size */
	else if (ragged)
	{
		/* Pick number of pillars */
		int y_num = rand_range(1, 8 / vault_int);
		int x_num = rand_range(1, 22 / vault_int);

		/* Room dimensions */
		int dy = y_num * vault_int;
		int dx = x_num * vault_int;

		/* Calculate distance */
		y1 = yval - (dy / 2);
		y2 = yval + (dy / 2 + (dy % 2));
		x1 = xval - (dx / 2);
		x2 = xval + (dx / 2 + (dx % 2));
	}
	/* Pick any room size */
	else
	{
		y1 = yval - randint(4);
		y2 = yval + randint(4);
		x1 = xval - randint(11);
		x2 = xval + randint(11);
	}

	/* Build the basic room */
	draw_rect_room(y1 - 1, x1 - 1, y2 + 1, x2 + 1, FEAT_FLOOR, FEAT_WALL_OUTER, flag);

	/* Hack -- Occasional vaulted room */
	if (vaulted)
	{
		/* Stagger some pillars horizontally */
		if (x_off)
		{
			for (y = y1 + vault_int - 1; y <= y2 - vault_int + 1; y += vault_int)
			{
				t = ((y - (y1 + vault_int - 1)) % x_off) / 2;

				/* Solidify some edges */
				if (t)
				{
					cave_feat[y][x1 - 1] = FEAT_WALL_SOLID;
					cave_feat[y][x2 + 1] = FEAT_WALL_SOLID;
				}

				for (x = x1 + vault_int - t - 1; x <= x2 - vault_int + t + 1; x += vault_int)
				{
					cave_feat[y][x] = FEAT_WALL_INNER;
				}
			}
		}

		/* Stagger some pillars vertically */
		else if (y_off)
		{
			for (x = x1 + vault_int - 1; x <= x2 - vault_int + 1; x += vault_int)
			{
				t = ((x - (x1 + vault_int - 1)) % y_off) / 2;

				/* Solidify some edges */
				if (t)
				{
					cave_feat[y1 - 1][x] = FEAT_WALL_SOLID;
					cave_feat[y2 + 1][x] = FEAT_WALL_SOLID;
				}

				for (y = y1 + vault_int - t - 1; y <= y2 - vault_int + t + 1; y += vault_int)
				{
					cave_feat[y][x] = FEAT_WALL_INNER;
				}
			}
		}

		/* Place some pillars orthogonally */
		else
		{
			for (y = y1 + vault_int - 1; y <= y2 - vault_int + 1; y += vault_int)
			{
				for (x = x1 + vault_int - 1; x <= x2 - vault_int + 1; x += vault_int)
				{
					cave_feat[y][x] = FEAT_WALL_INNER;
				}
			}
		}
	}

	/* Hack -- Occasional ragged-edge room */
	else if (ragged)
	{
		for (y = y1; y <= y2; y += vault_int)
		{
			cave_feat[y][x1] = FEAT_WALL_INNER;
			cave_feat[y][x2] = FEAT_WALL_INNER;
			cave_feat[y][x1 - 1] = FEAT_WALL_SOLID;
			cave_feat[y][x2 + 1] = FEAT_WALL_SOLID;
		}
		for (x = x1; x <= x2; x += vault_int)
		{
			cave_feat[y1][x] = FEAT_WALL_INNER;
			cave_feat[y2][x] = FEAT_WALL_INNER;
			cave_feat[y1 - 1][x] = FEAT_WALL_SOLID;
			cave_feat[y2 + 1][x] = FEAT_WALL_SOLID;
		}
	}
}


/*
 * Type 2 -- Overlapping rectangular rooms
 */
static void build_type2(int yval, int xval)
{
	int y1a, x1a, y2a, x2a;
	int y1b, x1b, y2b, x2b;

	bool light = (p_ptr->depth <= randint(25));
	byte flag = (light ? (CAVE_ROOM | CAVE_GLOW) : CAVE_ROOM);

	/* Determine extents of the first room */
	y1a = yval - randint(4);
	y2a = yval + randint(4);
	x1a = xval - randint(11);
	x2a = xval + randint(11);

	/* Determine extents of the second room */
	y1b = yval - randint(4);
	y2b = yval + randint(4);
	x1b = xval - randint(11);
	x2b = xval + randint(11);

	/* Build room 'a' */
	draw_rect_room(y1a - 1, x1a - 1, y2a + 1, x2a + 1, FEAT_FLOOR, FEAT_WALL_OUTER, flag);

	/* Build room 'b' */
	draw_rect_room(y1b - 1, x1b - 1, y2b + 1, x2b + 1, FEAT_FLOOR, FEAT_WALL_OUTER, flag);

	/* Replace the floor for room 'a' */
	fill_rect_feat(y1a, x1a, y2a, x2a, FEAT_FLOOR);

	/* Replace the floor for room 'b' */
	fill_rect_feat(y1b, x1b, y2b, x2b, FEAT_FLOOR);
}



/*
 * Type 3 -- Cross shaped rooms
 *
 * Builds a room at a row, column coordinate
 *
 * Room "a" runs north/south, and Room "b" runs east/east
 * So the "central pillar" runs from x1a,y1b to x2a,y2b.
 *
 * Note that currently, the "center" is always 3x3, but I think that
 * the code below will work (with "bounds checking") for 5x5, or even
 * for unsymetric values like 4x3 or 5x3 or 3x4 or 3x5, or even larger.
 */
static void build_type3(int yval, int xval)
{
	int y, x, dy, dx, ww;
	int y1a, x1a, y2a, x2a;
	int y1b, x1b, y2b, x2b;

	bool light = (p_ptr->depth <= randint(25));
	byte flag = (light ? (CAVE_ROOM | CAVE_GLOW) : CAVE_ROOM);

	/* 3x3, 5x5 */
	ww = randint(2);

	/* Pick max vertical size (at most 4) */
	dy = rand_range(ww + 2, 4);

	/* Pick max horizontal size (at most 11) */
	dx = rand_range(ww + 3, 11);

	/* Determine extents of the north/south room */
	y1a = yval - dy;
	y2a = yval + dy;
	x1a = xval - ww;
	x2a = xval + ww;

	/* Determine extents of the east/west room */
	y1b = yval - ww;
	y2b = yval + ww;
	x1b = xval - dx;
	x2b = xval + dx;

	/* Build room 'a' */
	draw_rect_room(y1a - 1, x1a - 1, y2a + 1, x2a + 1, FEAT_FLOOR, FEAT_WALL_OUTER, flag);

	/* Build room 'b' */
	draw_rect_room(y1b - 1, x1b - 1, y2b + 1, x2b + 1, FEAT_FLOOR, FEAT_WALL_OUTER, flag);

	/* Replace the floor for room 'a' */
	fill_rect_feat(y1a, x1a, y2a, x2a, FEAT_FLOOR);

	/* Replace the floor for room 'b' */
	fill_rect_feat(y1b, x1b, y2b, x2b, FEAT_FLOOR);

	/* Special features (4/5) */
	switch (rand_int(5))
	{
		/* Large solid middle pillar */
		case 1:
		{
			fill_rect_feat(y1b, x1a, y2b, x2a, FEAT_WALL_INNER);

			/* Pinch the corners */
			if (ww > 1)
			{
				cave_feat[y1b][x1a] = FEAT_FLOOR;
				cave_feat[y1b][x2a] = FEAT_FLOOR;
				cave_feat[y2b][x1a] = FEAT_FLOOR;
				cave_feat[y2b][x2a] = FEAT_FLOOR;

				cave_feat[y1b + 1][x1a + 1] = FEAT_WALL_INNER;
				cave_feat[y1b + 1][x2a - 1] = FEAT_WALL_INNER;
				cave_feat[y2b - 1][x1a + 1] = FEAT_WALL_INNER;
				cave_feat[y2b - 1][x2a - 1] = FEAT_WALL_INNER;
			}

			break;
		}

		/* Inner treasure vault */
		case 2:
		{
			/* Build the vault */
			draw_rect_feat(y1b, x1a, y2b, x2a, FEAT_WALL_INNER);

			/* Pinch the corners */
			if (ww > 1)
			{
				cave_feat[y1b][x1a] = FEAT_FLOOR;
				cave_feat[y1b][x2a] = FEAT_FLOOR;
				cave_feat[y2b][x1a] = FEAT_FLOOR;
				cave_feat[y2b][x2a] = FEAT_FLOOR;

				cave_feat[y1b + 1][x1a + 1] = FEAT_WALL_INNER;
				cave_feat[y1b + 1][x2a - 1] = FEAT_WALL_INNER;
				cave_feat[y2b - 1][x1a + 1] = FEAT_WALL_INNER;
				cave_feat[y2b - 1][x2a - 1] = FEAT_WALL_INNER;
			}

			/* Place a secret door on the inner room */
			switch (rand_int(4))
			{
				case 0: place_secret_door(y1b, xval); break;
				case 1: place_secret_door(y2b, xval); break;
				case 2: place_secret_door(yval, x1a); break;
				case 3: place_secret_door(yval, x2a); break;
			}

			/* Place a treasure in the vault */
			vault_objects(yval, xval, ww);

			/* Let's guard the treasure well */
			vault_monsters(yval, xval, rand_int(2) + 3 * ww);

			/* Traps naturally */
			vault_traps(yval, xval, 4, 4, rand_int(3) + ww);

			break;
		}

		/* Something else */
		case 3:
		{
			/* Occasionally pinch the center shut */
			if (rand_int(3) == 0)
			{
				/* Pinch the east/west sides */
				for (y = y1b; y <= y2b; y++)
				{
					if (y == yval) continue;
					cave_feat[y][x1a - 1] = FEAT_WALL_INNER;
					cave_feat[y][x2a + 1] = FEAT_WALL_INNER;
				}

				/* Pinch the north/south sides */
				for (x = x1a; x <= x2a; x++)
				{
					if (x == xval) continue;
					cave_feat[y1b - 1][x] = FEAT_WALL_INNER;
					cave_feat[y2b + 1][x] = FEAT_WALL_INNER;
				}

				/* Pinch the corners */
				if (ww > 1)
				{
					cave_feat[y1b][x1a] = FEAT_WALL_INNER;
					cave_feat[y1b][x2a] = FEAT_WALL_INNER;
					cave_feat[y2b][x1a] = FEAT_WALL_INNER;
					cave_feat[y2b][x2a] = FEAT_WALL_INNER;
				}

				/* Sometimes shut using secret doors */
				if (rand_int(3) == 0)
				{
					place_secret_door(yval, x1a - 1);
					place_secret_door(yval, x2a + 1);
					place_secret_door(y1b - 1, xval);
					place_secret_door(y2b + 1, xval);
				}
			}

			/* Occasionally put a "plus" in the center */
			else if (rand_int(3) == 0)
			{
				cave_feat[yval][xval] = FEAT_WALL_INNER;
				cave_feat[yval - 1][xval] = FEAT_WALL_INNER;
				cave_feat[yval + 1][xval] = FEAT_WALL_INNER;
				cave_feat[yval][xval - 1] = FEAT_WALL_INNER;
				cave_feat[yval][xval + 1] = FEAT_WALL_INNER;
			}

			/* Occasionally put a pillar in the center */
			else if (rand_int(3) == 0)
			{
				cave_feat[yval][xval] = FEAT_WALL_INNER;
			}

			break;
		}

		/* Larger treasure vault */
		case 4:
		{
			/* Build the vault */
			draw_rect_feat(y1b - 1, x1a - 1, y2b + 1, x2a + 1, FEAT_WALL_INNER);

			/* Pinch the corners */
			if (ww > 1)
			{
				cave_feat[y1b][x1a] = FEAT_WALL_INNER;
				cave_feat[y1b][x2a] = FEAT_WALL_INNER;
				cave_feat[y2b][x1a] = FEAT_WALL_INNER;
				cave_feat[y2b][x2a] = FEAT_WALL_INNER;
			}

			/* Place secret doors on the inner room */
			place_secret_door(y1b - 1, xval);
			place_secret_door(y2b + 1, xval);
			place_secret_door(yval, x1a - 1);
			place_secret_door(yval, x2a + 1);

			/* Place some treasure in the vault */
			vault_objects(yval, xval, rand_int(3) + 4 * ww);

			/* Let's guard the treasure well */
			vault_monsters(yval, xval, rand_int(3) + 4 * ww);

			/* Traps naturally */
			vault_traps(yval, xval, 4, 4, rand_int(3) + ww);

			break;
		}
	}
}


/*
 * Type 4 -- Large room with inner features
 *
 * Possible sub-types:
 *	1 - Just an inner room with one door
 *	2 - An inner room within an inner room
 *	3 - An inner room with pillar(s)
 *	4 - Inner room has an acyclic maze
 *	5 - A set of four inner rooms
 */
static void build_type4(int yval, int xval)
{
	int y, x, y1, x1;
	int y2, x2, tmp;

	bool light = (p_ptr->depth <= randint(25));
	byte flag = (light ? (CAVE_ROOM | CAVE_GLOW) : CAVE_ROOM);

	/* Large room */
	y1 = yval - 4;
	y2 = yval + 4;
	x1 = xval - 11;
	x2 = xval + 11;

	/* Build the outer room */
	draw_rect_room(y1 - 1, x1 - 1, y2 + 1, x2 + 1, FEAT_FLOOR, FEAT_WALL_OUTER, flag);

	/* The inner room */
	y1 = y1 + 2;
	y2 = y2 - 2;
	x1 = x1 + 2;
	x2 = x2 - 2;

	/* The inner walls */
	draw_rect_feat(y1 - 1, x1 - 1, y2 + 1, x2 + 1, FEAT_WALL_INNER);

	/* Inner room variations */
	switch (randint(5))
	{
		/* Just an inner room with a monster */
		case 1:

		/* Place a secret door */
		switch (randint(4))
		{
			case 1: place_secret_door(y1 - 1, xval); break;
			case 2: place_secret_door(y2 + 1, xval); break;
			case 3: place_secret_door(yval, x1 - 1); break;
			case 4: place_secret_door(yval, x2 + 1); break;
		}

		/* Place a monster in the room */
		vault_monsters(yval, xval, 1);

		break;


		/* Treasure Vault (with a door) */
		case 2:

		/* Place a secret door */
		switch (randint(4))
		{
			case 1: place_secret_door(y1 - 1, xval); break;
			case 2: place_secret_door(y2 + 1, xval); break;
			case 3: place_secret_door(yval, x1 - 1); break;
			case 4: place_secret_door(yval, x2 + 1); break;
		}

		/* Place another inner room */
		draw_rect_feat(yval - 1, xval - 1, yval + 1, xval + 1, FEAT_WALL_INNER);

		/* Place a locked door on the inner room */
		switch (randint(4))
		{
			case 1: place_locked_door(yval - 1, xval); break;
			case 2: place_locked_door(yval + 1, xval); break;
			case 3: place_locked_door(yval, xval - 1); break;
			case 4: place_locked_door(yval, xval + 1); break;
		}

		/* Monsters to guard the "treasure" */
		vault_monsters(yval, xval, randint(3) + 2);

		/* Object (80%) */
		if (rand_int(100) < 80)
		{
			place_object(yval, xval, FALSE, FALSE);
		}

		/* Stairs (20%) */
		else
		{
			place_random_stairs(yval, xval);
		}

		/* Traps to protect the treasure */
		vault_traps(yval, xval, 4, 10, 2 + randint(3));

		break;


		/* Inner pillar(s). */
		case 3:

		/* Place a secret door */
		switch (randint(4))
		{
			case 1: place_secret_door(y1 - 1, xval); break;
			case 2: place_secret_door(y2 + 1, xval); break;
			case 3: place_secret_door(yval, x1 - 1); break;
			case 4: place_secret_door(yval, x2 + 1); break;
		}

		/* Large Inner Pillar */
		fill_rect_feat(yval - 1, xval - 1, yval + 1, xval + 1, FEAT_WALL_INNER);

		/* Occasionally, two more Large Inner Pillars */
		if (rand_int(2) == 0)
		{
			tmp = randint(2);

			fill_rect_feat(yval - 1, xval - 5 - tmp, yval + 1, xval - 3 - tmp, FEAT_WALL_INNER);
			fill_rect_feat(yval - 1, xval + 3 + tmp, yval + 1, xval + 5 + tmp, FEAT_WALL_INNER);
		}

		/* Occasionally, some Inner rooms */
		if (rand_int(3) == 0)
		{
			draw_rect_feat(yval - 1, xval - 5, yval + 1, xval + 5, FEAT_WALL_INNER);

			/* Secret doors (random top/bottom) */
			place_secret_door(yval - 3 + (randint(2) * 2), xval - 3);
			place_secret_door(yval - 3 + (randint(2) * 2), xval + 3);

			/* Monsters */
			vault_monsters(yval, xval - 2, randint(2));
			vault_monsters(yval, xval + 2, randint(2));

			/* Objects */
			if (rand_int(3) == 0) place_object(yval, xval - 2, FALSE, FALSE);
			if (rand_int(3) == 0) place_object(yval, xval + 2, FALSE, FALSE);
		}

		break;


		/* Maze inside. */
		case 4:

		/* Acyclic maze */
		draw_maze(y1 - 1, x1 - 1, y2 + 1, x2 + 1, FEAT_WALL_SOLID, FEAT_FLOOR, 0);

		/* Place two secret doors (vertical) */
		if (rand_int(100) < 50)
		{
			int t = rand_int(10);
			place_secret_door(y1 - 1, x1 + t * 2);

			t = rand_int(10);
			place_secret_door(y2 + 1, x1 + t * 2);
		}

		/* Place two secret doors (horizontal) */
		else
		{
			int t = rand_int(3);
			place_secret_door(y1 + t * 2, x1 - 1);

			t = rand_int(3);
			place_secret_door(y1 + t * 2, x2 + 1);
		}

		/* Monsters just love mazes. */
		vault_monsters(yval, xval - 5, randint(3));
		vault_monsters(yval, xval + 5, randint(3));

		/* Traps make them entertaining. */
		vault_traps(yval, xval - 3, 2, 8, randint(3));
		vault_traps(yval, xval + 3, 2, 8, randint(3));

		/* Mazes should have some treasure too. */
		vault_objects(yval, xval, 3);

		break;


		/* Four small rooms. */
		case 5:

		/* Inner "cross" */
		for (y = y1; y <= y2; y++)
		{
			cave_feat[y][xval] = FEAT_WALL_INNER;
		}
		for (x = x1; x <= x2; x++)
		{
			cave_feat[yval][x] = FEAT_WALL_INNER;
		}

		/* Doors into the rooms */
		if (rand_int(100) < 50)
		{
			int i = randint(10);
			place_secret_door(y1 - 1, xval - i);
			place_secret_door(y1 - 1, xval + i);
			place_secret_door(y2 + 1, xval - i);
			place_secret_door(y2 + 1, xval + i);
		}
		else
		{
			int i = randint(3);
			place_secret_door(yval + i, x1 - 1);
			place_secret_door(yval - i, x1 - 1);
			place_secret_door(yval + i, x2 + 1);
			place_secret_door(yval - i, x2 + 1);
		}

		/* Treasure, centered at the center of the cross */
		vault_objects(yval, xval, 2 + randint(2));

		/* Gotta have some monsters. */
		vault_monsters(yval + 1, xval - 4, randint(4));
		vault_monsters(yval + 1, xval + 4, randint(4));
		vault_monsters(yval - 1, xval - 4, randint(4));
		vault_monsters(yval - 1, xval + 4, randint(4));

		break;
	}
}


/*
 * The following functions are used to determine if the given monster
 * is appropriate for inclusion in a monster nest or monster pit or
 * the given type.
 *
 * None of the pits/nests are allowed to include "unique" monsters.
 */


/*
 * Monster validation macro
 */
#define vault_monster_okay(I) \
	 (!(r_info[I].flags1 & (RF1_UNIQUE)))


/* Race index for "monster pit (clone)" */
static int vault_aux_race;

/* Race index for "monster pit (symbol clone)" */
static char vault_aux_char;

/* Breath mask for "monster pit (dragon)" */
static u32b vault_aux_dragon_mask4;


typedef struct vault_aux_type vault_aux_type;


struct vault_aux_type
{
	cptr name;
	bool (*hook_func)(int r_idx);
	void (*prep_func)(void);
	int level;
	int chance;
};


/*
 * Helper monster selection function
 */
static bool vault_aux_simple(int r_idx)
{
	/* Okay */
	return (vault_monster_okay(r_idx));
}


/*
 * Helper function for "monster nest (jelly)"
 */
static bool vault_aux_jelly(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Validate the monster */
	if (!vault_monster_okay(r_idx)) return (FALSE);

	/* Also decline evil jellies (like death molds and shoggoths) */
	if (r_ptr->flags3 & (RF3_EVIL)) return (FALSE);

	/* Require icky thing, jelly, mold, or mushroom */
	if (!strchr("ijm,", r_ptr->d_char)) return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster nest (doom)"
 */
static bool vault_aux_lo_doom(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Validate the monster */
	if (!vault_monster_okay(r_idx)) return (FALSE);

	/* Decline powerful doom monsters */
	if (!(r_ptr->flags3 & (RF3_DOOM)) || (r_ptr->flags1 & (RF1_FORCE_MAXHP))) return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster nest (doom)"
 */
static bool vault_aux_hi_doom(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Validate the monster */
	if (!vault_monster_okay(r_idx)) return (FALSE);

	/* Accept doom monsters */
	return ((r_ptr->flags3 & (RF3_DOOM)) != 0);
}



/*
 * Helper function for "monster nest (animal)"
 */
static bool vault_aux_animal(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Validate the monster */
	if (!vault_monster_okay(r_idx)) return (FALSE);

	/* Decline unique monsters */
	if (r_ptr->flags1 & (RF1_UNIQUE)) return (FALSE);

	/* Require "animal" flag */
	if (!(r_ptr->flags3 & (RF3_ANIMAL))) return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster nest (undead)"
 */
static bool vault_aux_undead(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Validate the monster */
	if (!vault_monster_okay(r_idx)) return (FALSE);

	/* Require Undead */
	if (!(r_ptr->flags3 & (RF3_UNDEAD))) return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster nest (chapel)"
 */
static bool vault_aux_chapel(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Validate the monster */
	if (!vault_monster_okay(r_idx)) return (FALSE);

	/* Require "priest" or Angel */
	if ((r_ptr->d_char != 'A') && !strstr((r_name + r_ptr->name), "riest"))
	{
		return (FALSE);
	}

	/* Okay */
	return (TRUE);
}

/*
 * Helper function for "monster nest (kennel)"
 */
static bool vault_aux_kennel(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Validate the monster */
	if (!vault_monster_okay(r_idx)) return (FALSE);

	/* Require a Zephyr Hound or a dog */
	if (!strchr("CZ", r_ptr->d_char)) return (FALSE);

	/* Okay */
	return (TRUE);
}

/*
 * Helper function for "monster nest (mimic)"
 */
static bool vault_aux_mimic(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Validate the monster */
	if (!vault_monster_okay(r_idx)) return (FALSE);

	/* Require mimic */
	if (!strchr("!|$?=", r_ptr->d_char)) return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster nest (clone)"
 */
static bool vault_aux_clone(int r_idx)
{
	return (r_idx == vault_aux_race);
}


/*
 * Helper function for "monster nest (symbol clone)"
 */
static bool vault_aux_symbol(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Validate the monster */
	if (!vault_monster_okay(r_idx)) return (FALSE);

	/* Decline incorrect symbol */
	if (r_ptr->d_char != vault_aux_char) return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster pit (orc)"
 */
static bool vault_aux_orc(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Validate the monster */
	if (!vault_monster_okay(r_idx)) return (FALSE);

	/* Require orc */
	if (!(r_ptr->flags3 & RF3_ORC)) return (FALSE);

	/* Decline undead */
	if (r_ptr->flags3 & RF3_UNDEAD) return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster pit (troll)"
 */
static bool vault_aux_troll(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Validate the monster */
	if (!vault_monster_okay(r_idx)) return (FALSE);

	/* Require troll */
	if (!(r_ptr->flags3 & RF3_TROLL)) return (FALSE);

	/* Decline undead */
	if (r_ptr->flags3 & RF3_UNDEAD) return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster pit (giant)"
 */
static bool vault_aux_giant(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Validate the monster */
	if (!vault_monster_okay(r_idx)) return (FALSE);

	/* Require giant */
	if (!(r_ptr->flags3 & RF3_GIANT)) return (FALSE);

	/* Decline undead */
	if (r_ptr->flags3 & RF3_UNDEAD) return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster pit (dragon)"
 */
static bool vault_aux_dragon(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Validate the monster */
	if (!vault_monster_okay(r_idx)) return (FALSE);

	/* Require dragon */
	if (!(r_ptr->flags3 & RF3_DRAGON)) return (FALSE);

	/* Hack -- Require correct "breath attack" */
	if (r_ptr->flags4 != vault_aux_dragon_mask4) return (FALSE);

	/* Decline undead */
	if (r_ptr->flags3 & RF3_UNDEAD) return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster pit (demon)"
 */
static bool vault_aux_demon(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Validate the monster */
	if (!vault_monster_okay(r_idx)) return (FALSE);

	/* Require demon */
	if (!(r_ptr->flags3 & RF3_DEMON)) return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster pit (clone)"
 */
static void vault_prep_clone(void)
{
	/* Apply the monster restriction */
	get_mon_num_prep(vault_aux_simple);

	/* Pick a race to clone */
	vault_aux_race = get_mon_num(p_ptr->depth + 10);

	/* Remove the monster restriction */
	get_mon_num_prep(NULL);
}


/*
 * Helper function for "monster pit (symbol clone)"
 */
static void vault_prep_symbol(void)
{
	int r_idx;

	/* Apply the monster restriction */
	get_mon_num_prep(vault_aux_simple);

	/* Pick a race to clone */
	r_idx = get_mon_num(p_ptr->depth + 10);

	/* Remove the monster restriction */
	get_mon_num_prep(NULL);

	/* Extract the symbol */
	vault_aux_char = r_info[r_idx].d_char;
}


/*
 * Helper function for "monster pit (dragon)"
 */
static void vault_prep_dragon(void)
{
	/* Pick dragon type */
	switch (rand_int(6))
	{
		/* Black */
		case 0:
		{
			/* Restrict dragon breath type */
			vault_aux_dragon_mask4 = RF4_BR_ACID;

			/* Done */
			break;
		}

		/* Blue */
		case 1:
		{
			/* Restrict dragon breath type */
			vault_aux_dragon_mask4 = RF4_BR_ELEC;

			/* Done */
			break;
		}

		/* Red */
		case 2:
		{
			/* Restrict dragon breath type */
			vault_aux_dragon_mask4 = RF4_BR_FIRE;

			/* Done */
			break;
		}

		/* White */
		case 3:
		{
			/* Restrict dragon breath type */
			vault_aux_dragon_mask4 = RF4_BR_COLD;

			/* Done */
			break;
		}

		/* Green */
		case 4:
		{
			/* Restrict dragon breath type */
			vault_aux_dragon_mask4 = RF4_BR_POIS;

			/* Done */
			break;
		}

		/* Multi-hued */
		default:
		{
			/* Restrict dragon breath type */
			vault_aux_dragon_mask4 = (RF4_BR_ACID | RF4_BR_ELEC | RF4_BR_FIRE | RF4_BR_COLD | RF4_BR_POIS);

			/* Done */
			break;
		}
	}
}


static vault_aux_type *pick_vault_type(vault_aux_type *l_ptr)
{
	int tmp, total;

	vault_aux_type *n_ptr;

	/* Calculate the total possibilities */
	for (n_ptr = l_ptr, total = 0; TRUE; n_ptr++)
	{
		/* Note end */
		if (!n_ptr->name) break;

		/* Ignore excessive depth */
		if (n_ptr->level > p_ptr->depth) continue;

		/* Count this possibility */
		total += n_ptr->chance * MAX_DEPTH / (p_ptr->depth - n_ptr->level + 5);
	}

	/* Pick a random type */
	tmp = rand_int(total);

	/* Find this type */
	for (n_ptr = l_ptr, total = 0; TRUE; n_ptr++)
	{
		/* Note end */
		if (!n_ptr->name) break;

		/* Ignore excessive depth */
		if (n_ptr->level > p_ptr->depth) continue;

		/* Count this possibility */
		total += n_ptr->chance * MAX_DEPTH / (p_ptr->depth - n_ptr->level + 5);

		/* Found the type */
		if (tmp < total) break;
	}

	return (n_ptr->name ? n_ptr : NULL);
}


static vault_aux_type nest_types[] =
{
	{"clone",			vault_aux_clone,		vault_prep_clone,		5,		3},
	{"jelly",			vault_aux_jelly,		NULL,						5,		6},
	{"doom",          vault_aux_lo_doom,   NULL,                10,   8},
	{"symbol clone",	vault_aux_symbol,		vault_prep_symbol,	25,	3},
	{"mimic",			vault_aux_mimic,		NULL,						25,	6},
	{"kennel",			vault_aux_kennel,		NULL,						50,	2},
	{"animal",			vault_aux_animal,		NULL,						50,	4},
	{"doom",          vault_aux_hi_doom,   NULL,                55,   6},
	{"chapel",			vault_aux_chapel,		NULL,						65,	2},
	{"undead",			vault_aux_undead,		NULL,						65,	4},
	{NULL,				NULL,						NULL,						0,		0},
};


/*
 * Type 5 -- Monster nests
 *
 * A monster nest is a "big" room, with an "inner" room, containing
 * a "collection" of monsters of a given type strewn about the room.
 *
 * The monsters are chosen from a set of 64 randomly selected monster
 * races, to allow the nest creation to fail instead of having "holes".
 *
 * Note the use of the "get_mon_num_prep()" function, and the special
 * "get_mon_num_hook()" restriction function, to prepare the "monster
 * allocation table" in such a way as to optimize the selection of
 * "appropriate" non-unique monsters for the nest.
 *
 * Note that the "get_mon_num()" function may (rarely) fail, in which
 * case the nest will be empty, and will not affect the level rating.
 *
 * Note that "monster nests" will never contain "unique" monsters.
 */
static void build_type5(int yval, int xval)
{
	int y, x, y1, x1, y2, x2;
	int i;
	int what[64];

	int align = 0;

	vault_aux_type *n_ptr = pick_vault_type(nest_types);

	/* No type available */
	if (!n_ptr) return;

	/* Process a preparation function if necessary */
	if (n_ptr->prep_func) (*(n_ptr->prep_func))();

	/* Large room */
	y1 = yval - 4;
	y2 = yval + 4;
	x1 = xval - 11;
	x2 = xval + 11;

	/* Build the outer room */
	draw_rect_room(y1 - 1, x1 - 1, y2 + 1, x2 + 1, FEAT_FLOOR, FEAT_WALL_OUTER, CAVE_ROOM);

	/* Advance to the center room */
	y1 = y1 + 2;
	y2 = y2 - 2;
	x1 = x1 + 2;
	x2 = x2 - 2;

	/* The inner walls */
	draw_rect_feat(y1 - 1, x1 - 1, y2 + 1, x2 + 1, FEAT_WALL_INNER);

	/* Place a secret door */
	switch (randint(4))
	{
		case 1: place_secret_door(y1 - 1, xval); break;
		case 2: place_secret_door(y2 + 1, xval); break;
		case 3: place_secret_door(yval, x1 - 1); break;
		case 4: place_secret_door(yval, x2 + 1); break;
	}

		/* Prepare allocation table */
	get_mon_num_prep(n_ptr->hook_func);

	/* Pick some monster types */
	for (i = 0; i < 64; i++)
		{
		int r_idx = 0, attempts = 100;

		while (attempts--)
		{
			/* Get a (hard) monster type */
			r_idx = get_mon_num(p_ptr->depth + 10);

			/* Decline incorrect alignment */
			if (((align < 0) && (r_info[r_idx].flags3 & RF3_GOOD)) ||
				 ((align > 0) && (r_info[r_idx].flags3 & RF3_EVIL)))
		{
				continue;
		}

			/* Accept this monster */
			break;
	}

		/* Notice failure */
		if (!r_idx || !attempts) return;

		/* Note the alignment */
		if (r_info[r_idx].flags3 & RF3_GOOD) align++;
		else if (r_info[r_idx].flags3 & RF3_EVIL) align--;

		what[i] = r_idx;
	}

   get_mon_num_prep(NULL);

	/* Describe */
	if (cheat_room)
	{
		/* Room type */
		msg_format("Monster nest (%s)", n_ptr->name);
	}

	/* Place some monsters */
	for (y = yval - 2; y <= yval + 2; y++)
	{
		for (x = xval - 9; x <= xval + 9; x++)
		{
			int r_idx = what[rand_int(64)];

			/* Place that "random" monster (no groups) */
			(void)place_monster_aux(y, x, r_idx, FALSE, FALSE, FALSE);
		}
	}
}


static vault_aux_type pit_types[] =
{
	{"orc",				vault_aux_orc,			NULL,						5,		4},
	{"doom",          vault_aux_lo_doom,   NULL,                10,   6},
	{"troll",			vault_aux_troll,		NULL,						20,	4},
	{"giant",			vault_aux_giant,		NULL,						40,	4},
	{"clone",			vault_aux_symbol,		vault_prep_symbol,	55,	3},
	{"chapel",			vault_aux_chapel,		NULL,						55,	1},
	{"doom",          vault_aux_hi_doom,   NULL,                60,   6},
	{"dragon",			vault_aux_dragon,		vault_prep_dragon,	70,	4},
	{"demon",			vault_aux_demon,		NULL,             	80,	4},
	{NULL,				NULL,						NULL,						0,		0},
};


/*
 * Type 6 -- Monster pits
 *
 * A monster pit is a "big" room, with an "inner" room, containing
 * a "collection" of monsters of a given type organized in the room.
 *
 * The inside room in a monster pit appears as shown below, where the
 * actual monsters in each location depend on the type of the pit
 *
 *   #####################
 *   #0000000000000000000#
 *   #0112233455543322110#
 *   #0112233467643322110#
 *   #0112233455543322110#
 *   #0000000000000000000#
 *   #####################
 *
 * Note that the monsters in the pit are now chosen by using "get_mon_num()"
 * to request 16 "appropriate" monsters, sorting them by level, and using
 * the "even" entries in this sorted list for the contents of the pit.
 *
 * Hack -- all of the "dragons" in a "dragon" pit must be the same "color",
 * which is handled by requiring a specific "breath" attack for all of the
 * dragons.  This may include "multi-hued" breath.  Note that "wyrms" may
 * be present in many of the dragon pits, if they have the proper breath.
 *
 * Note the use of the "get_mon_num_prep()" function, and the special
 * "get_mon_num_hook()" restriction function, to prepare the "monster
 * allocation table" in such a way as to optimize the selection of
 * "appropriate" non-unique monsters for the pit.
 *
 * Note that the "get_mon_num()" function may (rarely) fail, in which case
 * the pit will be empty, and will not effect the level rating.
 *
 * Note that "monster pits" will never contain "unique" monsters.
 */
static void build_type6(int yval, int xval)
{
	int y, x, y1, x1, y2, x2;
	int i, j;

	int what[16];

	int align = 0;

	vault_aux_type *n_ptr = pick_vault_type(pit_types);

	/* No type available */
	if (!n_ptr) return;

	/* Process a preparation function if necessary */
	if (n_ptr->prep_func) (*(n_ptr->prep_func))();

	/* Large room */
	y1 = yval - 4;
	y2 = yval + 4;
	x1 = xval - 11;
	x2 = xval + 11;

	/* Build the outer room */
	draw_rect_room(y1 - 1, x1 - 1, y2 + 1, x2 + 1, FEAT_FLOOR, FEAT_WALL_OUTER, CAVE_ROOM);

	/* Advance to the center room */
	y1 = y1 + 2;
	y2 = y2 - 2;
	x1 = x1 + 2;
	x2 = x2 - 2;

	/* The inner walls */
	draw_rect_feat(y1 - 1, x1 - 1, y2 + 1, x2 + 1, FEAT_WALL_INNER);

	/* Place a secret door */
	switch (randint(4))
	{
		case 1: place_secret_door(y1 - 1, xval); break;
		case 2: place_secret_door(y2 + 1, xval); break;
		case 3: place_secret_door(yval, x1 - 1); break;
		case 4: place_secret_door(yval, x2 + 1); break;
	}

		/* Prepare allocation table */
	get_mon_num_prep(n_ptr->hook_func);

	/* Pick some monster types */
	for (i = 0; i < 16; i++)
			{
		int r_idx = 0, attempts = 100;

		while (attempts--)
			{
			/* Get a (hard) monster type */
			r_idx = get_mon_num(p_ptr->depth + 10);

			/* Decline incorrect alignment */
			if (((align < 0) && (r_info[r_idx].flags3 & RF3_GOOD)) ||
				 ((align > 0) && (r_info[r_idx].flags3 & RF3_EVIL)))
			{
				continue;
			}

			/* Accept this monster */
				break;
			}

		/* Notice failure */
		if (!r_idx || !attempts) return;

		/* Note the alignment */
		if (r_info[r_idx].flags3 & RF3_GOOD) align++;
		else if (r_info[r_idx].flags3 & RF3_EVIL) align--;

		what[i] = r_idx;
	}

	get_mon_num_prep(NULL);

	/* Sort the entries */
	for (i = 0; i < 16 - 1; i++)
	{
		/* Sort the entries */
		for (j = 0; j < 16 - 1; j++)
		{
			int i1 = j;
			int i2 = j + 1;

			int p1 = r_info[what[i1]].level;
			int p2 = r_info[what[i2]].level;

			/* Bubble */
			if (p1 > p2)
			{
				int tmp = what[i1];
				what[i1] = what[i2];
				what[i2] = tmp;
			}
		}
	}

	/* Message */
	if (cheat_room)
	{
		/* Room type */
		msg_format("Monster pit (%s)", n_ptr->name);
	}

	/* Select the entries */
		for (i = 0; i < 8; i++)
		{
		/* Every other entry */
		what[i] = what[i * 2];

		if (cheat_hear)
		{
			/* Message */
			msg_print(r_name + r_info[what[i]].name);
		}
	}

	/* Top and bottom rows */
	for (x = xval - 9; x <= xval + 9; x++)
	{
		place_monster_aux(yval - 2, x, what[0], FALSE, FALSE, FALSE);
		place_monster_aux(yval + 2, x, what[0], FALSE, FALSE, FALSE);
	}

	/* Middle columns */
	for (y = yval - 1; y <= yval + 1; y++)
	{
		place_monster_aux(y, xval - 9, what[0], FALSE, FALSE, FALSE);
		place_monster_aux(y, xval + 9, what[0], FALSE, FALSE, FALSE);

		place_monster_aux(y, xval - 8, what[1], FALSE, FALSE, FALSE);
		place_monster_aux(y, xval + 8, what[1], FALSE, FALSE, FALSE);

		place_monster_aux(y, xval - 7, what[1], FALSE, FALSE, FALSE);
		place_monster_aux(y, xval + 7, what[1], FALSE, FALSE, FALSE);

		place_monster_aux(y, xval - 6, what[2], FALSE, FALSE, FALSE);
		place_monster_aux(y, xval + 6, what[2], FALSE, FALSE, FALSE);

		place_monster_aux(y, xval - 5, what[2], FALSE, FALSE, FALSE);
		place_monster_aux(y, xval + 5, what[2], FALSE, FALSE, FALSE);

		place_monster_aux(y, xval - 4, what[3], FALSE, FALSE, FALSE);
		place_monster_aux(y, xval + 4, what[3], FALSE, FALSE, FALSE);

		place_monster_aux(y, xval - 3, what[3], FALSE, FALSE, FALSE);
		place_monster_aux(y, xval + 3, what[3], FALSE, FALSE, FALSE);

		place_monster_aux(y, xval - 2, what[4], FALSE, FALSE, FALSE);
		place_monster_aux(y, xval + 2, what[4], FALSE, FALSE, FALSE);
	}

	/* Above/Below the center monster */
	for (x = xval - 1; x <= xval + 1; x++)
	{
		place_monster_aux(yval + 1, x, what[5], FALSE, FALSE, FALSE);
		place_monster_aux(yval - 1, x, what[5], FALSE, FALSE, FALSE);
	}

	/* Next to the center monster */
	place_monster_aux(yval, xval + 1, what[6], FALSE, FALSE, FALSE);
	place_monster_aux(yval, xval - 1, what[6], FALSE, FALSE, FALSE);

	/* Center monster */
	place_monster_aux(yval, xval, what[7], FALSE, FALSE, FALSE);
}


/*
 * Hack -- fill in "vault" rooms
 */
static void build_vault(int yval, int xval, int ymax, int xmax, cptr data)
{
	int dx, dy, x, y, c, len;

	cptr t;

	/* Place dungeon features and objects */
	for (t = data, dy = 0; dy < ymax; dy++)
	{
		for (dx = 0; dx < xmax; t++)
		{
			len = (byte)*t;

			/* Hack -- high bit indicates a run */
			if (len & 0x80)
			{
				len ^= 0x80;
				t++;
			}
			else
			{
				len = 1;
			}

			/* Extract encoded run */
			for (c = 0; c < len; dx++, c++)
			{
				/* Extract the location */
				x = xval - (xmax / 2) + dx;
				y = yval - (ymax / 2) + dy;

				/* Hack -- skip "non-grids" */
				if (*t == ' ') continue;

				/* Lay down a floor */
				draw_grid_feat(y, x, FEAT_FLOOR);

				/* Part of a vault */
				cave_info[y][x] |= (CAVE_ROOM | CAVE_ICKY);

				/* Analyze the grid */
				switch (*t)
				{
					/* Granite wall (outer) */
					case '%':
					draw_grid_feat(y, x, FEAT_WALL_OUTER);
					break;

					/* Granite wall (inner) */
					case '#':
					draw_grid_feat(y, x, FEAT_WALL_INNER);
					break;

					/* Permanent wall (inner) */
					case 'X':
					draw_grid_feat(y, x, FEAT_PERM_INNER);
					break;

					/* Treasure/trap */
					case '*':
					if (rand_int(100) < 75)
					{
						place_object(y, x, FALSE, FALSE);
					}
					else
					{
						place_trap(y, x);
					}
					break;

					/* Secret doors */
					case '+':
					place_secret_door(y, x);
					break;

					/* Trap */
					case '^':
					place_trap(y, x);
					break;
				}
			}
		}
	}


	/* Place dungeon monsters and objects */
	for (t = data, dy = 0; dy < ymax; dy++)
	{
		for (dx = 0; dx < xmax; t++)
		{
			len = (byte)*t;

			/* Hack -- high bit indicates a run */
			if (len & 0x80)
			{
				len ^= 0x80;
				t++;
			}
			else
			{
				len = 1;
			}

			/* Extract encoded run */
			for (c = 0; c < len; dx++, c++)
			{
				/* Extract the grid */
				x = xval - (xmax/2) + dx;
				y = yval - (ymax/2) + dy;

				/* Hack -- skip "non-grids" */
				if (*t == ' ') continue;

				/* Analyze the symbol */
				switch (*t)
				{
					/* Monster */
					case '&':
					{
						monster_level = p_ptr->depth + 5;
						place_monster(y, x, TRUE, TRUE);
						monster_level = p_ptr->depth;
						break;
					}

					/* Meaner monster */
					case '@':
					{
						monster_level = p_ptr->depth + 11;
						place_monster(y, x, TRUE, TRUE);
						monster_level = p_ptr->depth;
						break;
					}

					/* Meaner monster, plus treasure */
					case '9':
					{
						monster_level = p_ptr->depth + 9;
						place_monster(y, x, TRUE, TRUE);
						monster_level = p_ptr->depth;
						object_level = p_ptr->depth + 7;
						place_object(y, x, TRUE, FALSE);
						object_level = p_ptr->depth;
						break;
					}

					/* Nasty monster and treasure */
					case '8':
					{
						monster_level = p_ptr->depth + 40;
						place_monster(y, x, TRUE, TRUE);
						monster_level = p_ptr->depth;
						object_level = p_ptr->depth + 20;
						place_object(y, x, TRUE, TRUE);
						object_level = p_ptr->depth;
						break;
					}

					/* Monster and/or object */
					case ',':
					{
						if (rand_int(100) < 50)
						{
							monster_level = p_ptr->depth + 3;
							place_monster(y, x, TRUE, TRUE);
							monster_level = p_ptr->depth;
						}
						if (rand_int(100) < 50)
						{
							object_level = p_ptr->depth + 7;
							place_object(y, x, FALSE, FALSE);
							object_level = p_ptr->depth;
						}
						break;
					}

					case 'p':
					draw_grid_feat(y, x, FEAT_PATTERN_START);
					break;

					case 'a':
					draw_grid_feat(y, x, FEAT_PATTERN_1);
					break;

					case 'b':
					draw_grid_feat(y, x, FEAT_PATTERN_2);
					break;

					case 'c':
					draw_grid_feat(y, x, FEAT_PATTERN_3);
					break;

					case 'd':
					draw_grid_feat(y, x, FEAT_PATTERN_4);
					break;

					case 'P':
					draw_grid_feat(y, x, FEAT_PATTERN_END);
					break;

					case 'B':
					draw_grid_feat(y, x, FEAT_PATTERN_XTRA1);
					break;

					case 'A':
					{
						object_level = p_ptr->depth + 12;
						place_object(y, x, TRUE, FALSE);
						object_level = p_ptr->depth;
					}
					break;
				}
			}
		}
	}
}


/*
 * Hack -- fill in "quest" rooms
 * Heino Vander Sanden
 */
static void build_quest(int yval, int xval, int ymax, int xmax, cptr data)
{
	quest_type *q_ptr = &q_list[level_quest[p_ptr->depth]];

	int dx, dy, x, y, c, len;
	int r_idx = q_ptr->r_idx;

	cptr t;

	/* Place dungeon features and objects */
	for (t = data, dy = 0; dy < ymax; dy++)
	{
		for (dx = 0; dx < xmax; t++)
		{
			len = (byte)*t;

			/* Hack -- high bit indicates a run */
			if (len & 0x80)
			{
				len ^= 0x80;
				t++;
			}
			else
			{
				len = 1;
			}

			/* Extract encoded run */
			for (c = 0; c < len; dx++, c++)
			{
				/* Extract the location */
				x = xval - (xmax / 2) + dx;
				y = yval - (ymax / 2) + dy;

				/* Hack -- skip "non-grids" */
				if (*t == ' ') continue;

				/* Lay down a floor */
				cave_feat[y][x] = FEAT_FLOOR;

				/* Part of a vault */
				cave_info[y][x] |= (CAVE_ROOM | CAVE_ICKY);

				/* Analyze the grid */
				switch (*t)
				{
					/* Granite wall (outer) */
					case '%':
					draw_grid_feat(y, x, FEAT_WALL_OUTER);
					break;

					/* Granite wall (inner) */
					case '#':
					draw_grid_feat(y, x, FEAT_WALL_INNER);
					break;

					/* Permanent wall (inner) */
					case 'X':
					draw_grid_feat(y, x, FEAT_PERM_INNER);
					break;

					/* Treasure/trap */
					case '*':
					if (rand_int(100) < 75)
					{
						place_object(y, x, FALSE, FALSE);
					}
					else
					{
						place_trap(y, x);
					}
					break;

					/* Secret doors */
					case '+':
					place_secret_door(y, x);
					break;

					/* Trap */
					case '^':
					place_trap(y, x);
					break;
				}
			}
		}
	}

	/* Place quest monsters */
	for (t = data, dy = 0; dy < ymax; dy++)
	{
		for (dx = 0; dx < xmax; t++)
		{
			len = (byte)*t;

			/* Hack -- high bit indicates a run */
			if (len & 0x80)
			{
				len ^= 0x80;
				t++;
			}
			else
			{
				len = 1;
			}

			/* Extract encoded run */
			for (c = 0; c < len; dx++, c++)
			{
				/* Extract the grid */
				x = xval - (xmax/2) + dx;
				y = yval - (ymax/2) + dy;

				/* Hack -- skip "non-grids" */
				if (*t == ' ') continue;

				/* Analyze the symbol */
				switch (*t)
				{
					/* Quest monster */
					case '8':
					{
						place_monster_aux(y, x, r_idx, FALSE, FALSE, FALSE);
					}
				}
			}
		}
	}
}


/*
 * Type 7 -- simple vaults (see "v_info.txt")
 */
static void build_type7(int yval, int xval)
{
	vault_type	*v_ptr;
	int dummy = 0;

	/* Pick a lesser vault */
	while (dummy < SAFE_MAX_ATTEMPTS)
	{
		dummy++;

		/* Access a random vault record */
		v_ptr = &v_info[rand_int(MAX_V_IDX)];

		/* Accept the first lesser vault */
		if (v_ptr->typ == 7) break;
	}

	if (dummy >= SAFE_MAX_ATTEMPTS)
	{
		if (cheat_room)
		{
			msg_print("Warning! Could not place lesser vault!");
		}
		return;
	}

	/* Message */
	if (cheat_room) msg_print("Lesser Vault");

	/* Boost the rating */
	rating += v_ptr->rat;

	/* (Sometimes) Cause a special feeling */
	if ((p_ptr->depth <= 50) ||
		(randint((p_ptr->depth-40) * (p_ptr->depth-40) + 50) < 400))
	{
		good_item_flag = TRUE;
	}

	/* Hack -- Build the vault */
	build_vault(yval, xval, v_ptr->hgt, v_ptr->wid, v_text + v_ptr->text);
}



/*
 * Type 8 -- greater vaults (see "v_info.txt")
 */
static void build_type8(int yval, int xval)
{
	vault_type	*v_ptr;
	int dummy = 0;

	/* Pick a lesser vault */
	while (dummy < SAFE_MAX_ATTEMPTS)
	{
		dummy++;

		/* Access a random vault record */
		v_ptr = &v_info[rand_int(MAX_V_IDX)];

		/* Accept the first greater vault */
		if (v_ptr->typ == 8) break;
	}

	if (dummy >= SAFE_MAX_ATTEMPTS)
	{
		if (cheat_room)
		{
			msg_print("Warning! Could not place greater vault!");
		}
		return;
	}

	/* Message */
	if (cheat_room) msg_print("Greater Vault");

	/* Boost the rating */
	rating += v_ptr->rat;

	/* (Sometimes) Cause a special feeling */
	if ((p_ptr->depth <= 50) ||
        (randint((p_ptr->depth-40) * (p_ptr->depth-40) + 50) < 400))
	{
		good_item_flag = TRUE;
	}

	/* Hack -- Build the vault */
	build_vault(yval, xval, v_ptr->hgt, v_ptr->wid, v_text + v_ptr->text);
}


/*
 * Type 9 -- quest room
 * Heino Vander Sanden
 */
static void build_type9(int yval, int xval)
{
	vault_type *v_ptr;

	/* Pick a quest room */
	while (TRUE)
	{
		/* Access a random vault record */
		v_ptr = &v_info[rand_int(MAX_V_IDX)];

		/* Accept the first quest room */
		if (v_ptr->typ == 9) break;
	}

	/* Message */
	if (cheat_room) msg_print("Quest Room");

	/* Hack -- Build the vault */
	build_quest(yval, xval, v_ptr->hgt, v_ptr->wid, v_text + v_ptr->text);
}


/*
 * Type 10 --- "circular" rooms (technically ellipses...)
 * Probably this should be reordered.
 */
static void build_type10(int yval, int xval)
{
	int y, x, k;

	bool light = (p_ptr->depth <= randint(25));
	byte flag = (light ? (CAVE_ROOM | CAVE_GLOW) : CAVE_ROOM);

	for (y = yval - 5; y <= yval + 5; y++)
	{
		for (x = xval - 9; x <= xval + 9; x++)
		{
			/* Extract the distance */
			k = distance(0, 0, (y - yval) * 2, x - xval);

			/* Stay in the "circle" */
			if (k > 10) continue;

			cave_info[y][x] |= flag;

			if (k >= 9)
			{
				cave_feat[y][x] = FEAT_WALL_OUTER;
			}
			else
			{
				cave_feat[y][x] = FEAT_FLOOR;
			}
		}
	}


	switch (rand_int(4))
	{
		/* Central pillar */
		case 1:
		{
			if (rand_int(3))
			{
				fill_rect_feat(yval - 1, xval - 1, yval + 1, xval + 1, FEAT_WALL_INNER);
			}

			/* Occasional cross-shaped center */
			else
			{
				cave_feat[yval][xval] = FEAT_WALL_INNER;
				cave_feat[yval - 1][xval] = FEAT_WALL_INNER;
				cave_feat[yval + 1][xval] = FEAT_WALL_INNER;
				cave_feat[yval][xval - 1] = FEAT_WALL_INNER;
				cave_feat[yval][xval + 1] = FEAT_WALL_INNER;
			}

			break;
		}

		/* Central vault */
		case 2:
		{
			for (y = (yval - 2); y <= (yval + 2); y++)
			{
				for (x = (xval - 3); x <= (xval + 3); x++)
				{
					/* Extract the distance */
					k = distance(0, 0, (y - yval) * 2, x - xval);

					/* Stay in the "circle" */
					if (k > 3) continue;

					if (k >= 2)
					{
						cave_feat[y][x] = FEAT_WALL_INNER;
						continue;
					}
				}
			}

			/* Make a door */
			if (rand_int(2)) place_secret_door(yval - 1, xval);
			else place_secret_door(yval + 1, xval);

			/* Put some treasure in the vault */
			vault_objects(yval, xval, 2);

			/* Guard the treasure */
			vault_monsters(yval, xval, 4);

			/* Gotta have traps */
			vault_traps(yval, xval, 4, 4, 4);

			break;
		}

		/* Four small pillars */
		case 3:
		{
			cave_feat[yval - 1][xval - 2] = FEAT_WALL_INNER;
			cave_feat[yval + 1][xval - 2] = FEAT_WALL_INNER;
			cave_feat[yval - 1][xval + 2] = FEAT_WALL_INNER;
			cave_feat[yval + 1][xval + 2] = FEAT_WALL_INNER;

			break;
		}
	}
}


/*
 * Type 11 --- Acyclic mazes
 * Probably this should be reordered.
 */
static void build_type11(int yval, int xval)
{
	int y2, x2;
	int y1, x1;

	/* Pick a room size */
	y1 = yval - 5;
	y2 = yval + 5;
	x1 = xval - 12;
	x2 = xval + 12;

	/* Build an acyclic maze */
	draw_maze(y1, x1, y2, x2, FEAT_WALL_SOLID, FEAT_FLOOR, 0);

	/* Allow piercings of the edges */
	draw_rect_feat(y1, x1, y2, x2, FEAT_WALL_OUTER);

	/* Monsters just love mazes. */
	vault_monsters(yval, xval - 6, randint(3) + 2);
	vault_monsters(yval, xval + 6, randint(3) + 2);

	/* Traps make them entertaining. */
	vault_traps(yval, xval - 5, 2, 8, randint(4));
	vault_traps(yval, xval + 5, 2, 8, randint(4));

	/* Mazes should have some treasure too. */
	vault_objects(yval, xval - 4, randint(2));
	vault_objects(yval, xval + 4, randint(2));
}


/*
 * Constructs a tunnel between two points
 *
 * This function must be called BEFORE any streamers are created,
 * since we use the special "granite wall" sub-types to keep track
 * of legal places for corridors to pierce rooms.
 *
 * We use "door_flag" to prevent excessive construction of doors
 * along overlapping corridors.
 *
 * We queue the tunnel grids to prevent door creation along a corridor
 * which intersects itself.
 *
 * We queue the wall piercing grids to prevent a corridor from leaving
 * a room and then coming back in through the same entrance.
 *
 * We "pierce" grids which are "outer" walls of rooms, and when we
 * do so, we change all adjacent "outer" walls of rooms into "solid"
 * walls so that no two corridors may use adjacent grids for exits.
 *
 * The "solid" wall check prevents corridors from "chopping" the
 * corners of rooms off, as well as "silly" door placement, and
 * "excessively wide" room entrances.
 *
 * Useful "feat" values:
 *   FEAT_WALL_EXTRA -- granite walls
 *   FEAT_WALL_INNER -- inner room walls
 *   FEAT_WALL_OUTER -- outer room walls
 *   FEAT_WALL_SOLID -- solid room walls
 *   FEAT_PERM_EXTRA -- shop walls (perma)
 *   FEAT_PERM_INNER -- inner room walls (perma)
 *   FEAT_PERM_OUTER -- outer room walls (perma)
 *   FEAT_PERM_SOLID -- dungeon border (perma)
 */
static void build_tunnel(int y1, int x1, int y2, int x2)
{
	int i, y, x;
	int ty, tx;
	int dy, dx;
	int sy, sx;
	int main_loop_count = 0;

	bool door_flag = FALSE;



	/* Reset the arrays */
	dun->tunn_n = 0;
	dun->wall_n = 0;

	/* Save the starting location */
	sy = y1;
	sx = x1;

	/* Start out in the correct direction */
	correct_dir(&dy, &dx, y1, x1, y2, x2);

	/* Keep going until done (or bored) */
	while ((y1 != y2) || (x1 != x2))
	{
		/* Mega-Hack -- Paranoia -- prevent infinite loops */
		if (main_loop_count++ > 2000) break;

		/* Allow bends in the tunnel */
		if (rand_int(100) < DUN_TUN_CHG)
		{
			/* Acquire the correct direction */
			correct_dir(&dy, &dx, y1, x1, y2, x2);

			/* Random direction */
			if (rand_int(100) < DUN_TUN_RND)
			{
				rand_dir(&dy, &dx);
			}
		}

		/* Get the next location */
		ty = y1 + dy;
		tx = x1 + dx;


		/* Do not leave the dungeon!!! XXX XXX */
		while (!in_bounds_fully(ty, tx))
		{
			/* Acquire the correct direction */
			correct_dir(&dy, &dx, y1, x1, y2, x2);

			/* Random direction */
			if (rand_int(100) < DUN_TUN_RND)
			{
				rand_dir(&dy, &dx);
			}

			/* Get the next location */
			ty = y1 + dy;
			tx = x1 + dx;
		}


		/* Avoid the edge of the dungeon */
		if (cave_feat[ty][tx] == FEAT_PERM_SOLID) continue;

		/* Avoid the edge of vaults */
		if (cave_feat[ty][tx] == FEAT_PERM_OUTER) continue;

		/* Avoid "solid" granite walls */
		if (cave_feat[ty][tx] == FEAT_WALL_SOLID) continue;

		/* Pierce "outer" walls of rooms */
		if (cave_feat[ty][tx] == FEAT_WALL_OUTER)
		{
			/* Acquire the "next" location */
			y = ty + dy;
			x = tx + dx;

			/* Hack -- Avoid outer/solid permanent walls */
			if (cave_feat[y][x] == FEAT_PERM_SOLID) continue;
			if (cave_feat[y][x] == FEAT_PERM_OUTER) continue;

			/* Hack -- Avoid outer/solid granite walls */
			if (cave_feat[y][x] == FEAT_WALL_OUTER) continue;
			if (cave_feat[y][x] == FEAT_WALL_SOLID) continue;

			/* Accept this location */
			y1 = ty;
			x1 = tx;

			/* Save the wall location */
			if (dun->wall_n < WALL_MAX)
			{
				dun->wall_y[dun->wall_n] = y1;
				dun->wall_x[dun->wall_n] = x1;
				dun->wall_n++;
			}

			/* Forbid re-entry near this piercing */
			for (y = y1 - 1; y <= y1 + 1; y++)
			{
				for (x = x1 - 1; x <= x1 + 1; x++)
				{
					/* Convert adjacent "outer" walls as "solid" walls */
					if (cave_feat[y][x] == FEAT_WALL_OUTER)
					{
						/* Change the wall to a "solid" wall */
						draw_grid_feat(y, x, FEAT_WALL_SOLID);
					}
				}
			}
		}

		/* Travel quickly through rooms */
		else if (cave_info[ty][tx] & (CAVE_ROOM))
		{
			/* Accept the location */
			y1 = ty;
			x1 = tx;
		}

		/* Tunnel through all other walls */
		else if (cave_feat[ty][tx] >= FEAT_WALL_EXTRA)
		{
			/* Accept this location */
			y1 = ty;
			x1 = tx;

			/* Save the tunnel location */
			if (dun->tunn_n < TUNN_MAX)
			{
				dun->tunn_y[dun->tunn_n] = y1;
				dun->tunn_x[dun->tunn_n] = x1;
				dun->tunn_n++;
			}

			/* Allow door in next grid */
			door_flag = FALSE;
		}

		/* Handle corridor intersections or overlaps */
		else
		{
			/* Accept the location */
			y1 = ty;
			x1 = tx;

			/* Collect legal door locations */
			if (!door_flag)
			{
				/* Save the door location */
				if (dun->door_n < DOOR_MAX)
				{
					dun->door_y[dun->door_n] = y1;
					dun->door_x[dun->door_n] = x1;
					dun->door_n++;
				}

				/* No door in next grid */
				door_flag = TRUE;
			}

			/* Hack -- allow pre-emptive tunnel termination */
			if (rand_int(100) >= DUN_TUN_CON)
			{
				/* Distance between y1 and sy */
				ty = y1 - sy;
				if (ty < 0) ty = (-ty);

				/* Distance between x1 and sx */
				tx = x1 - sx;
				if (tx < 0) tx = (-tx);

				/* Terminate the tunnel */
				if ((ty > 10) || (tx > 10)) break;
			}
		}
	}


	/* Turn the tunnel into corridor */
	for (i = 0; i < dun->tunn_n; i++)
	{
		/* Access the grid */
		y = dun->tunn_y[i];
		x = dun->tunn_x[i];

		/* Clear previous contents, add a floor */
		draw_grid_feat(y, x, FEAT_FLOOR);
	}


	/* Apply the piercings that we found */
	for (i = 0; i < dun->wall_n; i++)
	{
		/* Access the grid */
		y = dun->wall_y[i];
		x = dun->wall_x[i];

		/* Convert to floor grid */
		draw_grid_feat(y, x, FEAT_FLOOR);

		/* Occasional doorway */
		if (rand_int(100) < DUN_TUN_PEN)
		{
			/* Place a random door */
			place_random_door(y, x);
		}
	}
}




/*
 * Count the number of "corridor" grids adjacent to the given grid.
 *
 * Note -- Assumes "in_bounds(y1, x1)"
 *
 * XXX XXX This routine currently only counts actual "empty floor"
 * grids which are not in rooms.  We might want to also count stairs,
 * open doors, closed doors, etc.
 */
static int next_to_corr(int y1, int x1)
{
	int i, y, x, k = 0;

	/* Scan adjacent grids */
	for (i = 0; i < 4; i++)
	{
		/* Extract the location */
		y = y1 + ddy_ddd[i];
		x = x1 + ddx_ddd[i];

		/* Skip non floors */
		if (!cave_floor_bold(y, x)) continue;

		/* Skip non "empty floor" grids */
		if (cave_feat[y][x] != FEAT_FLOOR) continue;

		/* Skip grids inside rooms */
		if (cave_info[y][x] & (CAVE_ROOM)) continue;

		/* Count these grids */
		k++;
	}

	/* Return the number of corridors */
	return (k);
}


/*
 * Determine if the given location is "between" two walls,
 * and "next to" two corridor spaces.  XXX XXX XXX
 *
 * Assumes "in_bounds(y,x)"
 */
static bool possible_doorway(int y, int x)
{
	/* Count the adjacent corridors */
	if (next_to_corr(y, x) >= 2)
	{
		/* Check Vertical */
		if ((cave_feat[y-1][x] >= FEAT_MAGMA) &&
			 (cave_feat[y+1][x] >= FEAT_MAGMA))
		{
			return (TRUE);
		}

		/* Check Horizontal */
		if ((cave_feat[y][x-1] >= FEAT_MAGMA) &&
		    (cave_feat[y][x+1] >= FEAT_MAGMA))
		{
			return (TRUE);
		}
	}

	/* No doorway */
	return (FALSE);
}


/*
 * Places door at y, x position if at least 2 walls found
 */
static void try_door(int y, int x)
{
	/* Paranoia */
	if (!in_bounds(y, x)) return;

	/* Ignore walls */
	if (cave_feat[y][x] >= FEAT_MAGMA) return;

	/* Ignore room grids */
	if (cave_info[y][x] & (CAVE_ROOM)) return;

	/* Occasional door (if allowed) */
	if ((rand_int(100) < DUN_TUN_JCT) && possible_doorway(y, x))
	{
		/* Place a door */
		place_random_door(y, x);
	}
}




/*
 * Attempt to build a room of the given type at the given block
 *
 * Note that we restrict the number of "crowded" rooms to reduce
 * the chance of overflowing the monster list during level creation.
 */
static bool room_build(int y0, int x0, int typ)
{
	int y, x, y1, x1, y2, x2;


	/* Restrict level */
	if (p_ptr->depth < room[typ].level) return (FALSE);

	/* Restrict "crowded" rooms */
	if (dun->crowded && ((typ == 5) || (typ == 6))) return (FALSE);

	/* Extract blocks */
	y1 = y0 + room[typ].dy1;
	y2 = y0 + room[typ].dy2;
	x1 = x0 + room[typ].dx1;
	x2 = x0 + room[typ].dx2;

	/* Never run off the screen */
	if ((y1 < 0) || (y2 >= dun->row_rooms)) return (FALSE);
	if ((x1 < 0) || (x2 >= dun->col_rooms)) return (FALSE);

	/* Verify open space */
	for (y = y1; y <= y2; y++)
	{
		for (x = x1; x <= x2; x++)
		{
			if (dun->room_map[y][x]) return (FALSE);
		}
	}

	/* XXX XXX XXX It is *extremely* important that the following */
	/* calculation is *exactly* correct to prevent memory errors */

	/* Acquire the location of the room */
	y = ((y1 + y2 + 1) * BLOCK_HGT) / 2;
	x = ((x1 + x2 + 1) * BLOCK_WID) / 2;

	/* Build a room */
	switch (typ)
	{
		/* Build an appropriate room */
		case 11: build_type11(y, x); break;
		case 10: build_type10(y, x); break;
		case  9: build_type9 (y, x); break;
		case  8: build_type8 (y, x); break;
		case  7: build_type7 (y, x); break;
		case  6: build_type6 (y, x); break;
		case  5: build_type5 (y, x); break;
		case  4: build_type4 (y, x); break;
		case  3: build_type3 (y, x); break;
		case  2: build_type2 (y, x); break;
		case  1: build_type1 (y, x); break;

		/* Paranoia */
		default: return (FALSE);
	}

	/* Save the room location */
	if (dun->cent_n < CENT_MAX)
	{
		dun->cent_y[dun->cent_n] = y;
		dun->cent_x[dun->cent_n] = x;
		dun->cent_n++;
	}

	/* Reserve some blocks */
	for (y = y1; y <= y2; y++)
	{
		for (x = x1; x <= x2; x++)
		{
			dun->room_map[y][x] = TRUE;
		}
	}

	/* Success */
	return (TRUE);
}


/*
 * Generate a new dungeon level
 *
 * Note that "dun_body" adds about 4000 bytes of memory to the stack.
 */
static bool cave_gen(void)
{
	int i, k, y, x, y1, x1;

	bool destroyed = FALSE;
	bool empty_level = FALSE;

	dun_data dun_body;

	int q_idx = level_quest[p_ptr->depth];
	quest_type *q_ptr = &q_list[q_idx];

	bool cur_quest = is_quest(p_ptr->depth);

	int dun_rooms;

	/* Global data */
	dun = &dun_body;


	if (!rand_int(EMPTY_LEVEL) && empty_levels)
	{
		empty_level = TRUE;
		if (cheat_room) msg_print("Arena level.");
	}

	/* Hack -- Start with basic granite */
	draw_rect_room(0, 0, cur_hgt - 1, cur_wid - 1,
						(empty_level) ? FEAT_FLOOR : FEAT_WALL_EXTRA, FEAT_PERM_SOLID, 0);

	/* Possible "destroyed" level */
	if (!cur_quest && (p_ptr->depth > 10) && !rand_int(DUN_DEST))
	{
		destroyed = TRUE;
	}

	/* Quest Level => make quest monster (Heino Vander Sanden) */
	if (cur_quest) r_info[q_ptr->r_idx].flags1 |= (RF1_QUESTOR);

	/* Actual maximum number of rooms on this level */
	dun->row_rooms = cur_hgt / BLOCK_HGT;
	dun->col_rooms = cur_wid / BLOCK_WID;

	/* Assume three blocks per room */
	dun_rooms = dun->row_rooms * dun->col_rooms / 3;

	/* Initialize the room table */
	for (y = 0; y < dun->row_rooms; y++)
	{
		for (x = 0; x < dun->col_rooms; x++)
		{
			dun->room_map[y][x] = FALSE;
		}
	}


	/* No "crowded" rooms yet */
	dun->crowded = FALSE;


	/* No rooms yet */
	dun->cent_n = 0;


	/*
	 * Quest level gives chance for a quest room
	 * Heino Vander Sanden
	 */
	if (cur_quest)
	{
		int r_idx = q_ptr->r_idx;
		int cur_num = q_ptr->max_num;
		int attempts = dun_rooms;

		while (attempts-- && cur_num && (rand_range(p_ptr->depth, 100) >= 25))
		{
			/* Pick a block for the room */
			y = rand_int(dun->row_rooms);
			x = rand_int(dun->col_rooms);

			/* Align dungeon rooms */
			if (dungeon_align)
			{
				/* Slide some rooms right */
				if ((x % 3) == 0) x++;

				/* Slide some rooms left */
				if ((x % 3) == 2) x--;
			}

			room_build(y, x, 9);
			cur_num = q_ptr->max_num - r_info[r_idx].cur_num;
		}
	}

	/* Build some rooms */
	for (i = 0; i < dun_rooms; i++)
	{
		/* Pick a block for the room */
		y = rand_int(dun->row_rooms);
		x = rand_int(dun->col_rooms);

		if (MAX_M_IDX - m_max < 190) dun->crowded = TRUE;

		/* Align dungeon rooms */
		if (dungeon_align)
		{
			/* Slide some rooms right */
			if ((x % 3) == 0) x++;

			/* Slide some rooms left */
			if ((x % 3) == 2) x--;
		}

		/* Attempt an "unusual" room */
		if (rand_int(DUN_UNUSUAL) < p_ptr->depth)
		{
			/* Roll for room type */
			k = rand_int(100);

			/* Attempt a very unusual room */
			if (rand_int(DUN_UNUSUAL) < p_ptr->depth)
			{
				/* Type 8 -- Greater vault (10%) */
				if ((k < 10) && room_build(y, x, 8)) continue;

				/* Type 7 -- Lesser vault (15%) */
				if ((k < 25) && room_build(y, x, 7)) continue;

				/* Type 6 -- Monster pit (15%) */
				if ((k < 40) && room_build(y, x, 6)) continue;

				/* Type 5 -- Monster nest (10%) */
				if ((k < 50) && room_build(y, x, 5)) continue;
			}

			/* Type 11 -- Acyclic mazes (10%) */
			if ((k < 10) && room_build(y, x, 11)) continue;

			/* Type 10 -- Circular room (10%) */
			if ((k < 20) && room_build(y, x, 10)) continue;

			/* Type 4 -- Large room (20%) */
			if ((k < 40) && room_build(y, x, 4)) continue;

			/* Type 3 -- Cross room (20%) */
			if ((k < 60) && room_build(y, x, 3)) continue;

			/* Type 2 -- Overlapping (40%) */
			if ((k < 100) && room_build(y, x, 2)) continue;
		}

		/* Attempt a trivial room */
		if (room_build(y, x, 1)) continue;
	}

	/* Draw the boundary walls */
	draw_rect_feat(0, 0, cur_hgt - 1, cur_wid - 1, FEAT_PERM_SOLID);

	/* Hack -- Scramble the room order */
	for (i = 0; i < dun->cent_n; i++)
	{
		int pick1 = rand_int(dun->cent_n);
		int pick2 = rand_int(dun->cent_n);
		y1 = dun->cent_y[pick1];
		x1 = dun->cent_x[pick1];
		dun->cent_y[pick1] = dun->cent_y[pick2];
		dun->cent_x[pick1] = dun->cent_x[pick2];
		dun->cent_y[pick2] = y1;
		dun->cent_x[pick2] = x1;
	}

	/* Start with no tunnel doors */
	dun->door_n = 0;

	/* Hack -- connect the first room to the last room */
	y = dun->cent_y[dun->cent_n-1];
	x = dun->cent_x[dun->cent_n-1];

	/* Connect all the rooms together */
	for (i = 0; i < dun->cent_n; i++)
	{
		/* Connect the room to the previous room */
		build_tunnel(dun->cent_y[i], dun->cent_x[i], y, x);

		/* Remember the "previous" room */
		y = dun->cent_y[i];
		x = dun->cent_x[i];
	}

	/* Place intersection doors	 */
	for (i = 0; i < dun->door_n; i++)
	{
		/* Extract junction location */
		y = dun->door_y[i];
		x = dun->door_x[i];

		/* Try placing doors */
		try_door(y, x - 1);
		try_door(y, x + 1);
		try_door(y - 1, x);
		try_door(y + 1, x);
	}


	/* Use the magma vein hook */
	streamer_feat_hook = streamer_feat_hook_magma;

	/* Hack -- Add some magma streamers */
	for (i = 0; i < DUN_STR_MAG; i++)
	{
		build_streamer(DUN_STR_MC, DUN_STR_DEN, DUN_STR_RNG);
	}

	/* Use the quartz vein hook */
	streamer_feat_hook = streamer_feat_hook_quartz;

	/* Hack -- Add some quartz streamers */
	for (i = 0; i < DUN_STR_QUA; i++)
	{
		build_streamer(DUN_STR_QC, DUN_STR_DEN, DUN_STR_RNG);
	}

#if 0
	/* Use the moss hook */
	streamer_feat_hook = streamer_feat_hook_moss;

	/* Hack -- Add some moss streamers */
	for (i = 0; i < DUN_STR_MOS; i++)
	{
		build_streamer(0, DUN_MOS_DEN, DUN_MOS_RNG);
	}

	/* Use the lichen hook */
	streamer_feat_hook = streamer_feat_hook_lichen;

	/* Hack -- Add some lichen streamers */
	for (i = 0; i < DUN_STR_LIC; i++)
	{
		build_streamer(0, DUN_LIC_DEN, DUN_LIC_RNG);
	}
#endif

	/* Destroy the level if necessary */
	if (destroyed) destroy_level();


	/* Place 3 or 4 down stairs near some walls */
	alloc_stairs(FEAT_MORE, rand_range(3, 4), 3);

	/* Place 1 or 2 up stairs near some walls */
	alloc_stairs(FEAT_LESS, rand_range(1, 2), 3);


	/* Determine the character location */
	if (!new_player_spot())
		return FALSE;


	/* Basic "amount" */
	k = (p_ptr->depth / 3);
	if (k > 10) k = 10;
	if (k < 2) k = 2;

	/*
	 * Put the quest monster(s) in the dungeon
	 * Heino Vander Sanden
	 */
	if (cur_quest)
	{
		monster_race *r_ptr = &r_info[q_ptr->r_idx];

		q_ptr->cur_num = 0;

		while (r_ptr->cur_num < q_ptr->max_num)
		{
			put_quest_monster(q_ptr->r_idx);
		}
	}

	/* Pick a base number of monsters */
	i = MIN_M_ALLOC_LEVEL;

	/* Scale the monsters */
	if ((cur_hgt != DUN_HGT) || (cur_wid != DUN_WID))
	{
		int small_tester = i;

		i = (i * cur_hgt) / DUN_HGT;
		i = (i * cur_wid) / DUN_WID;
		i += 1;

		if (cheat_hear)
		{
			msg_format("Changed monsters base from %d to %d", small_tester, i);
		}
	}

	i += randint(8);

	/* Put some monsters in the dungeon */
	for (i = i + k; i > 0; i--)
	{
		(void)alloc_monster(0, TRUE);
	}


	/* Place some traps in the dungeon */
	alloc_object(ALLOC_SET_ANY, ALLOC_TYP_TRAP, randint(k));

	/* Put some rubble in corridors */
	alloc_object(ALLOC_SET_CORR, ALLOC_TYP_RUBBLE, randint(k));

	/* Put some objects in rooms */
	alloc_object(ALLOC_SET_ROOM, ALLOC_TYP_OBJECT, rand_nor(DUN_AMT_ROOM, 3));

	/* Put some objects/gold in the dungeon */
	alloc_object(ALLOC_SET_ANY, ALLOC_TYP_OBJECT, rand_nor(DUN_AMT_ITEM, 3));
	alloc_object(ALLOC_SET_ANY, ALLOC_TYP_GOLD, rand_nor(DUN_AMT_GOLD, 3));

	/* Put some exploding barrels in the dungeon */
	alloc_object((ALLOC_SET_IN_ROOM | ALLOC_SET_WALL | ALLOC_SET_BARREL),
					 ALLOC_TYP_BARREL, rand_nor((DUN_AMT_BARREL * p_ptr->depth) /
					 									randint(MAX_DEPTH), 4));

	 if ((empty_level) && (randint(DARK_EMPTY)!=1 || (randint(100) > p_ptr->depth)))
		  wiz_lite();

	 return TRUE;

}



/*
 * Builds a store at a given (row, column)
 *
 * Note that the solid perma-walls are at x=0/65 and y=0/21
 *
 * As of 2.7.4 (?) the stores are placed in a more "user friendly"
 * configuration, such that the four "center" buildings always
 * have at least four grids between them, to allow easy running,
 * and the store doors tend to face the middle of town.
 *
 * The stores now lie inside boxes from 3-9 and 12-18 vertically,
 * and from 7-17, 21-31, 35-45, 49-59.  Note that there are thus
 * always at least 2 open grids between any disconnected walls.
 */
static void build_store(int n, int yy, int xx)
{
	int y, x, y0, x0, y1, x1, y2, x2, tmp;

	/* Find the "center" of the store */
	y0 = yy * 9 + 6;
	x0 = xx * 11 + 11;

	/* Determine the store boundaries */
	y1 = y0 - randint((yy == 0) ? 3 : 2);
	y2 = y0 + randint((yy == 1) ? 3 : 2);
	x1 = x0 - randint(4);
	x2 = x0 + randint(4);

	/* Build an invulnerable rectangular building */
	fill_rect_full(y1, x1, y2, x2, FEAT_PERM_EXTRA, (CAVE_GLOW | CAVE_MARK));

	/* Pick a door direction (S,N,E,W) */
	tmp = rand_int(4);

	/* Re-roll "annoying" doors */
	if (((tmp == 0) && (yy == 1)) ||
	    ((tmp == 1) && (yy == 0)) ||
	    ((tmp == 2) && (xx == 4)) ||
	    ((tmp == 3) && (xx == 0)))
	{
		/* Pick a new direction */
		tmp = rand_int(4);
	}

	/* Extract a "door location" */
	switch (tmp)
	{
		/* Bottom side */
		case 0:
		y = y2;
		x = rand_range(x1, x2);
		break;

		/* Top side */
		case 1:
		y = y1;
		x = rand_range(x1, x2);
		break;

		/* Right side */
		case 2:
		y = rand_range(y1, y2);
		x = x2;
		break;

		/* Left side */
		default:
		y = rand_range(y1, y2);
		x = x1;
		break;
	}

	/* Clear previous contents, add a store door */
	draw_grid_feat(y, x, FEAT_SHOP_HEAD + n);
}




/*
 * Generate the "consistent" town features, and place the player
 *
 * Hack -- play with the R.N.G. to always yield the same town
 * layout, including the size and shape of the buildings, the
 * locations of the doorways, and the location of the stairs.
 */
static void town_gen_hack(void)
{
	int y, x, k, n;
	int dummy = 0;

	int rooms[MAX_STORES];

	u32b old_seed = random;

	rand_seed(seed_town);


	/* Prepare an Array of "remaining stores", and count them */
	for (n = 0; n < MAX_STORES; n++) rooms[n] = n;

	/* Place two rows of stores */
	for (y = 0; y < 2; y++)
	{
		/* Place four stores per row */
		for (x = 0; x < 4; x++)
		{
			/* Pick a random unplaced store */
			k = rand_int(n);

			/* Build that store at the proper location */
			build_store(rooms[k], y, x);

			/* Shift the stores down, remove one store */
			rooms[k] = rooms[--n];
		}
	}

	/* Hack -- Build the 9th store */
	build_store(rooms[0], rand_int(2), 4);

	/* Place the stairs */
	while (dummy < SAFE_MAX_ATTEMPTS)
	{
		dummy++;

		/* Pick a location at least "three" from the outer walls */
		y = rand_range(3, cur_hgt - 4);
		x = rand_range(3, cur_wid - 4);

		/* Require a "naked" floor grid */
		if (cave_naked_bold(y, x)) break;
	}

	if (dummy >= SAFE_MAX_ATTEMPTS)
	{
		if (cheat_room)
		{
			msg_print("Warning! Could not place stairs!");
		}
	}


	/* Clear previous contents, add down stairs */
	place_down_stairs(y, x);

	/* Memorize the stairs */
	cave_info[y][x] |= CAVE_MARK;

	/* Hack -- the player starts on the stairs */
	player_place(y, x);

	rand_seed(old_seed);
}




/*
 * Town logic flow for generation of new town
 *
 * We start with a fully wiped cave of normal floors.
 *
 * Note that town_gen_hack() plays games with the R.N.G.
 *
 * This function does NOT do anything about the owners of the stores,
 * nor the contents thereof.  It only handles the physical layout.
 *
 * We place the player on the stairs at the same time we make them.
 *
 * Hack -- since the player always leaves the dungeon by the stairs,
 * he is always placed on the stairs, even if he left the dungeon via
 * word of recall or teleport level.
 */
static void town_gen(void)
{
	int i;

	bool day = ((turn % (10L * TOWN_DAWN)) < ((10L * TOWN_DAWN) / 2));
	byte info = day ? (CAVE_GLOW | (view_perma_grids ? CAVE_MARK : 0)) : 0;

	/* Build the town */
	draw_rect_room(0, 0, cur_hgt - 1, cur_wid - 1, FEAT_FLOOR, FEAT_PERM_SOLID, info);

	/* Note the outer walls */
	draw_rect_info(0, 0, cur_hgt - 1, cur_wid - 1, CAVE_MARK);

	/* Hack -- Build the buildings/stairs (from memory) */
	town_gen_hack();


	/* Day Light */
	if (day)
	{
		/* Make some day-time residents */
		for (i = 0; i < MIN_M_ALLOC_TD; i++) (void)alloc_monster(3, TRUE);
	}

	/* Night Time */
	else
	{
		/* Make some night-time residents */
		for (i = 0; i < MIN_M_ALLOC_TN; i++) (void)alloc_monster(3, TRUE);
	}
}


/*
 * Generates a random dungeon level			-RAK-
 *
 * Hack -- regenerate any "overflow" levels
 *
 * Hack -- allow auto-scumming via a gameplay option.
 */
void generate_cave(void)
{
	int num;

	/* The dungeon is not ready */
	character_dungeon = FALSE;

	/* Generate */
	for (num = 0; TRUE; num++)
	{
		bool okay = TRUE;

		cptr why = NULL;

		/* Reset */
		o_max = 1;
		m_max = 1;

		/* No flags */
		C_WIPE(cave_info, MAX_HGT, byte_256);

		/* No features */
		C_WIPE(cave_feat, MAX_HGT, byte_256);

		/* No objects */
		C_WIPE(cave_o_idx, MAX_HGT, s16b_wid);

		/* No monsters */
		C_WIPE(cave_m_idx, MAX_HGT, s16b_wid);

#ifdef MONSTER_FLOW
		/* No flow */
		C_WIPE(cave_cost, MAX_HGT, byte_wid);
		C_WIPE(cave_when, MAX_HGT, byte_wid);
#endif /* MONSTER_FLOW */

		/* No crushers yet */
		crusher_n = 0;

		/* Mega-Hack -- no player yet */
		p_ptr->px = p_ptr->py = 0;


		/* Mega-Hack -- no panel yet */
		panel_row_min = 0;
		panel_row_max = 0;
		panel_col_min = 0;
		panel_col_max = 0;


		/* Reset the monster generation level */
		monster_level = p_ptr->depth;

		/* Reset the object generation level */
		object_level = p_ptr->depth;

		/* Nothing special here yet */
		good_item_flag = FALSE;

		/* Nothing good here yet */
		rating = 0;


		/* Build the town */
		if (!p_ptr->depth)
		{
			/* Small town */
			cur_hgt = SCREEN_HGT;
			cur_wid = SCREEN_WID;

			/* Determine number of panels */
			max_panel_rows = cur_hgt * 2 / SCREEN_HGT - 2;
			max_panel_cols = cur_wid * 2 / SCREEN_WID - 2;

			/* Assume illegal panel */
			panel_row = max_panel_rows;
			panel_col = max_panel_cols;

			/* Make a town */
			town_gen();
		}

		/* Build a real level */
		else
		{
			if (small_levels && !rand_int(SMALL_LEVEL))
			{
				cur_hgt = randint(MAX_HGT / SCREEN_HGT) * SCREEN_HGT;
				cur_wid = randint(MAX_WID / SCREEN_WID) * SCREEN_WID;

				/* Determine number of panels */
				max_panel_rows = cur_hgt * 2 / SCREEN_HGT - 2;
				max_panel_cols = cur_wid * 2 / SCREEN_WID - 2;

				/* Assume illegal panel */
				panel_row = max_panel_rows;
				panel_col = max_panel_cols;

				if (cheat_room)
				{
					msg_format("A 'resized' level - X:%d, Y:%d.",
								  max_panel_cols, max_panel_rows);
				}
			}
			else
			{
				/* Big dungeon */
				cur_hgt = DUN_HGT;
				cur_wid = DUN_WID;

				/* Determine number of panels */
				max_panel_rows = cur_hgt * 2 / SCREEN_HGT - 2;
				max_panel_cols = cur_wid * 2 / SCREEN_WID - 2;

				/* Assume illegal panel */
				panel_row = max_panel_rows;
				panel_col = max_panel_cols;
			}

			/* Make a dungeon */
			if (!cave_gen())
			{
				why = "could not place player";
				okay = FALSE;
			}
		}


		/* Extract the feeling */
		if (!p_ptr->depth) feeling = 0;
		else if ((good_item_flag && !p_ptr->preserve) || (rating > 120)) feeling = 1;
		else if (rating > 100) feeling = 2;
		else if (rating > 80) feeling = 3;
		else if (rating > 60) feeling = 4;
		else if (rating > 40) feeling = 5;
		else if (rating > 30) feeling = 6;
		else if (rating > 20) feeling = 7;
		else if (rating > 10) feeling = 8;
		else if (rating > 0) feeling = 9;
		else feeling = 10;

		/* Prevent object over-flow */
		if (o_max >= MAX_O_IDX)
		{
			/* Message */
			why = "too many objects";

			/* Message */
			okay = FALSE;
		}

		/* Prevent monster over-flow */
		if (m_max >= MAX_M_IDX)
		{
			/* Message */
			why = "too many monsters";

			/* Message */
			okay = FALSE;
		}

		/* Mega-Hack -- "auto-scum" */
		if (auto_scum && (num < 100))
		{
			/* Require "goodness" */
			if ((feeling > 9) ||
			    ((p_ptr->depth >= 5) && (feeling > 8)) ||
			    ((p_ptr->depth >= 10) && (feeling > 7)) ||
			    ((p_ptr->depth >= 20) && (feeling > 6)) ||
			    ((p_ptr->depth >= 40) && (feeling > 5)))
			{
				/* Give message to cheaters */
				if (cheat_room || cheat_hear ||
				    cheat_peek || cheat_xtra)
				{
					/* Message */
					why = "boring level";
				}

				/* Try again */
				okay = FALSE;
			}
		}

		/* Accept */
		if (okay) break;


		/* Message */
		if (why) msg_format("Generation restarted (%s)", why);

		/* Wipe the objects */
		wipe_o_list();

		/* Wipe the monsters */
		wipe_m_list();
	}


	/* The dungeon is ready */
	character_dungeon = TRUE;

	/* Remember when this level was "created" */
	old_turn = turn;
}


#ifdef ALLOW_WIZARD

# if 0

/*
 * Mega-hack -- provide a hook into dungeon generation!
 */
void do_cmd_generate(void)
{
	char cmd;

	/* Get a "generation command" */
	(void)(get_com("Generation Command: ", &cmd));

	/* Analyze the command */
	switch (cmd)
	{
		/* Nothing */
		case ESCAPE:
		case ' ':
		case '\n':
		case '\r':
		{
			break;
		}

		/* Hack -- Help */
		case '?':
		{
			do_cmd_help("help.hlp");
			break;
		}

		/* Not a generation command */
		default:
		{
			msg_print("That is not a valid generation command.");
			break;
		}
	}
}

# endif

#endif


