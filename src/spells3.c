/* File: spells3.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"


/*
 * Brand some ammunition.  Used by Cubragol and a mage spell.  The spell was
 * moved here from cmd6.c where it used to be for Cubragol only.  I've also
 * expanded it to do either frost, fire or venom, at random. -GJW	-KMW-
 */
void brand_ammo (int brand_type, int bolts_only)
{
	int a;
	int allowable;

	if (bolts_only)
		allowable = TV_BOLT;
	else
		allowable = TV_BOLT | TV_ARROW | TV_SHOT;

	for (a = 0; a < INVEN_PACK; a++)
	{
		object_type *o_ptr = &inventory[a];

		if ((bolts_only) && (o_ptr->tval != TV_BOLT)) continue;
		if ((!bolts_only) && (o_ptr->tval != TV_BOLT) &&
		    (o_ptr->tval != TV_ARROW) && (o_ptr->tval != TV_SHOT))
		    	continue;
		if ((!artifact_p(o_ptr)) && (!ego_item_p(o_ptr)) &&
		   (!cursed_p(o_ptr) && !broken_p(o_ptr)))
		   	break;
	}

	/* Enchant the ammo (or fail) */
	if ((a < INVEN_PACK) && (rand_int(100) < 50))
	{
		object_type *o_ptr = &inventory[a];
		char *ammo_name, *aura_name, msg[48];
		int aura_type, r;

		if (brand_type == 1) r = 0;		/* fire only */
		else if (brand_type == 2) r = 99;	/* poison only */
		else r = rand_int (100);

		if (r < 33)
		{
			aura_name = "fiery";
			aura_type = EGO_FLAME;
		}
		else if (r < 67)
		{
			aura_name = "frosty";
			aura_type = EGO_FROST;
		}
		else
		{
			aura_name = "sickly";
			aura_type = EGO_VENOM;
		}

		if (o_ptr->tval == TV_BOLT)
			ammo_name = "bolts";
		else if (o_ptr->tval == TV_ARROW)
			ammo_name = "arrows";
		else
			ammo_name = "shots";

		sprintf (msg, "Your %s are covered in a %s aura!",
			ammo_name, aura_name);
		msg_print (msg);
		o_ptr->name2 = aura_type;
		enchant (o_ptr, rand_int(3) + 4, ENCH_TOHIT | ENCH_TODAM);
	}
	else
	{
		if (flush_failure) flush();
		msg_print ("The enchantment failed.");
	}
}


/* Fetch an item (teleport it right underneath the caster) */
void fetch_item(int dir, int wgt)
{
	int ty, tx, i;
	bool flag;
	object_type *o_ptr;
	int py, px;

	py = p_ptr->py; px = p_ptr->px;

	/* Check to see if an object is already there */
	if(cave_o_idx[py][px])
	{
		msg_print("You can't fetch when you're already standing on something.");
		return;
	}

	/* Use a target */
	if(dir==5 && target_okay())
	{
		tx = p_ptr->target_col;
		ty = p_ptr->target_row;

		if(distance(py, px, ty, tx)>MAX_RANGE)
		{
			msg_print("You can't fetch something that far away!");
			return;
		}
	}
	else
	{
		/* Use a direction */
		ty = py; /* Where to drop the item */
		tx = px;
		flag = FALSE;
		do
		{
			ty += ddy[dir];
			tx += ddx[dir];
			if ((distance(py, px, ty, tx)> MAX_RANGE)
				|| !cave_floor_bold(ty, tx)) return;
		} while(!cave_o_idx[ty][tx]);
	}
	o_ptr = &o_list[cave_o_idx[ty][tx]];

	/* Too heavy to 'fetch' */
	if (o_ptr->weight > wgt)
	{
		msg_print("The object is too heavy.");
		return;
	}

	i = cave_o_idx[ty][tx];
	cave_o_idx[ty][tx] = 0;
	cave_o_idx[py][px] = i; /* 'move' it */
	o_ptr->iy = py;
	o_ptr->ix = px;

	p_ptr->redraw |= PR_MAP;
}


/* incremental sleep spell */
/* -KMW- */
void do_sleep_monster(void)
{
	int dir;

	if (p_ptr->lev < 15)
	{
		if (!get_aim_dir(&dir)) return;
		sleep_monster(dir);
	}
	else if (p_ptr->lev < 30)
	{
		sleep_monsters_touch();
	}
	else
	{
		sleep_monsters();
	}
}

/* incremental fear spell */
/* -KMW- */
void do_fear_monster(void)
{
	int dir;

	if (p_ptr->lev < 15)
	{
		if (!get_aim_dir(&dir)) return;
		fear_monster(dir,p_ptr->lev);
	}
	else if (p_ptr->lev < 30)
	{
		fear_monsters_touch();
	}
	else
	{
		fear_monsters();
	}
}

/* incremental cure wounds spell */
/* -KMW- */
void do_cure_wounds(void)
{
	if (p_ptr->lev < 15)
	{
		(void)hp_player(damroll(4, 10));
	}
	else if (p_ptr->lev < 30)
	{
		(void)hp_player(damroll(6, 10));
		(void)set_cut(0);
	}
	else
	{
		(void)hp_player(damroll(8, 10));
		(void)set_stun(0);
		(void)set_cut(0);
	}
}


/* flood -KMW- */
void flood(int cy, int cx, int r, int typ)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	int i, t, y, x, yy, xx, dy, dx;

	int sn = 0, sy = 0, sx = 0;

	bool map[32][32];
	bool floor;

	/* Paranoia -- Enforce maximum range */
	if (r > 12) r = 12;

	/* Clear the "maximal blast" area */
	for (y = 0; y < 32; y++)
	{
		for (x = 0; x < 32; x++)
		{
			map[y][x] = FALSE;
		}
	}

	/* Check around the epicenter */
	for (dy = -r; dy <= r; dy++)
	{
		for (dx = -r; dx <= r; dx++)
		{
			/* Extract the location */
			yy = cy + dy;
			xx = cx + dx;
			floor = cave_floor_bold(yy, xx);

			/* Skip illegal grids */
			if (!in_bounds_fully(yy, xx)) continue;

			/* Skip distant grids */
			if (distance(cy, cx, yy, xx) > r) continue;

			/* Skip the epicenter */
			/* if (!dx && !dy) continue; */

			/* Skip some grids for forest */
			if ((rand_int(100) < 50) &&
			   (typ == 2)) continue;

			if ((typ == 2) && !floor) continue;

			/* Damage this grid */
			map[16+yy-cy][16+xx-cx] = TRUE;

		}
	}


	/* Examine the quaked region */
	for (dy = -r; dy <= r; dy++)
	{
		for (dx = -r; dx <= r; dx++)
		{
			/* Extract the location */
			yy = cy + dy;
			xx = cx + dx;

			/* Skip unaffected grids */
			if (!map[16+yy-cy][16+xx-cx]) continue;

			/* Process monsters */
			if (cave_m_idx[yy][xx] > 0)
			{
				monster_type *m_ptr = &m_list[cave_m_idx[yy][xx]];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];

				/* Most monsters cannot co-exist with rock */
				if (!(r_ptr->flags2 & (RF2_SWIM)) &&
				    !(r_ptr->flags2 & (RF2_FLY)) &&
				    !(r_ptr->flags2 & (RF2_PASS_WALL)) &&
				    (typ == 1))
				{
					char m_name[80];

					/* Assume not safe */
					sn = 0;

					/* Monster can move to escape the wall */
					if (!(r_ptr->flags1 & (RF1_NEVER_MOVE)))
					{
						/* Look for safety */
						for (i = 0; i < 8; i++)
						{
							/* Get the grid */
							y = yy + ddy[i];
							x = xx + ddx[i];

							/* Skip non-empty grids */
							if (!cave_empty_bold(y, x)) continue;

							/* Hack -- no safety on glyph of warding */
							if (cave_feat[y][x] == FEAT_GLYPH) continue;

							/* Important -- Skip "quake" grids */
							if (map[16+y-cy][16+x-cx]) continue;

							/* Count "safe" grids */
							sn++;

							/* Randomize choice */
							if (rand_int(sn) > 0) continue;

							/* Save the safe grid */
							sy = y;
							sx = x;
						}
					}

					/* Describe the monster */
					monster_desc(m_name, m_ptr, 0);

					/* Monster is certainly awake */
					m_ptr->csleep = 0;

					msg_format("%^s sinks below the surface!", m_name);

					/* Delete the monster */
					delete_monster(yy, xx);

					/* No longer safe */
					sn = 0;

					/* Hack -- Escape from the flood */
					if (sn)
					{
						/* Move the monster */
						monster_swap(yy, xx, sy, sx);
					}
				}
			}
		}
	}


	/* XXX XXX XXX */

	/* New location */
	py = p_ptr->py;
	px = p_ptr->px;

	/* Important -- no wall on player */
	map[16+py-cy][16+px-cx] = FALSE;


	/* Examine the quaked region */
	for (dy = -r; dy <= r; dy++)
	{
		for (dx = -r; dx <= r; dx++)
		{
			/* Extract the location */
			yy = cy + dy;
			xx = cx + dx;

			/* Skip unaffected grids */
			if (!map[16+yy-cy][16+xx-cx]) continue;

			/* Paranoia -- never affect player */
			if ((yy == py) && (xx == px)) continue;

			/* Destroy location (if valid) */
			if (cave_valid_bold(yy, xx))
			{
				bool floor = cave_floor_bold(yy, xx);

				/* Delete objects */
				delete_object(yy, xx);

				/* Wall (or floor) type */
				t = (floor ? rand_int(100) : 200);

				if (typ == 1) {
					if (t < 100)
						cave_set_feat(yy, xx, FEAT_DEEP_WATER);
					else
						cave_set_feat(yy, xx, FEAT_SHAL_WATER);
				} else if (typ == 2)
						cave_set_feat(yy, xx, FEAT_TREES);
			}
		}
	}


	/* Fully update the visuals */
	p_ptr->update |= (PU_FORGET_VIEW | PU_UPDATE_VIEW | PU_MONSTERS);

	/* Fully update the flow */
	p_ptr->update |= (PU_FORGET_FLOW | PU_UPDATE_FLOW);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD);
}



/*
 * Places "streamers" of rock through dungeon
 *
 * Note that their are actually six different terrain features used
 * to represent streamers.  Three each of magma and quartz, one for
 * basic vein, one with hidden gold, and one with known gold.  The
 * hidden gold types are currently unused.
 */
void fissure(int feat)
{
	int i, tx, ty;
	int y, x, dir;


	/* Hack -- Choose starting point */
	y = p_ptr->py;
	x = p_ptr->px;

	/* Choose a random compass direction */
	dir = ddd[rand_int(8)];

	/* Place streamer into dungeon */
	while (TRUE)
	{
		/* One grid per density */
		for (i = 0; i < 7; i++)
		{
			int d = 1;

			/* Pick a nearby grid */
			while (1)
			{
				ty = rand_spread(y, d);
				tx = rand_spread(x, d);
				if (!in_bounds(ty, tx)) continue;
				break;
			}

			/* Only convert non-permanent features */
			if ((cave_feat[ty][tx] >= FEAT_PERM_EXTRA) &&
			   (cave_feat[ty][tx] <= FEAT_PERM_SOLID)) continue;
			if (cave_feat[ty][tx] == FEAT_LESS) continue;
			if (cave_feat[ty][tx] == FEAT_MORE) continue;

			/* Clear previous contents, add proper vein type */
			cave_set_feat(ty, tx, feat);

		}

		/* Advance the streamer */
		y += ddy[dir];
		x += ddx[dir];

		/* Stop at dungeon edge */
		if (!in_bounds(y, x)) break;
	}

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);
}


void alter_terrain(int cy, int cx, int r, int typ)
{
	int y, x, yy, xx, dy, dx;

	bool map[32][32];
	bool floor;

	/* Paranoia -- Enforce maximum range */
	if (r > 12) r = 12;

	/* Clear the "maximal blast" area */
	for (y = 0; y < 32; y++)
	{
		for (x = 0; x < 32; x++)
		{
			map[y][x] = FALSE;
		}
	}

	/* Check around the epicenter */
	for (dy = -r; dy <= r; dy++)
	{
		for (dx = -r; dx <= r; dx++)
		{
			/* Extract the location */
			yy = cy + dy;
			xx = cx + dx;
			floor = cave_floor_bold(yy, xx);

			/* Skip illegal grids */
			if (!in_bounds_fully(yy, xx)) continue;

			/* Skip distant grids */
			if (distance(cy, cx, yy, xx) > r) continue;

			/* Skip the epicenter */
			/* if (!dx && !dy) continue; */

			/* Damage this grid */
			map[16+yy-cy][16+xx-cx] = TRUE;

		}
	}

	/* Examine the quaked region */
	for (dy = -r; dy <= r; dy++)
	{
		for (dx = -r; dx <= r; dx++)
		{
			/* Extract the location */
			yy = cy + dy;
			xx = cx + dx;

			/* Skip unaffected grids */
			if (!map[16+yy-cy][16+xx-cx]) continue;

			/* Skip staircases */
			if (cave_feat[yy][xx] == FEAT_LESS || cave_feat[yy][xx] == FEAT_MORE)
			    continue;
			
			if (typ == 1) {
				if (cave_feat[yy][xx] == FEAT_DEEP_LAVA)
					cave_set_feat(yy, xx, FEAT_SHAL_LAVA);
				else if (cave_feat[yy][xx] == FEAT_SHAL_LAVA)
					cave_set_feat(yy, xx, FEAT_FLOOR);
			} else if (typ == 2) {
				if (cave_feat[yy][xx] == FEAT_DEEP_WATER)
					cave_set_feat(yy, xx, FEAT_SHAL_WATER);
				else if (cave_feat[yy][xx] == FEAT_SHAL_WATER)
					cave_set_feat(yy, xx, FEAT_FLOOR);
				else if (cave_feat[yy][xx] == FEAT_FOG)
					cave_set_feat(yy, xx, FEAT_FLOOR);
			} else if (typ == 3) {
				if ((cave_feat[yy][xx] != FEAT_MOUNTAIN) &&
				    (cave_feat[yy][xx] != FEAT_PERM_EXTRA) &&
				    (cave_feat[yy][xx] != FEAT_PERM_INNER) &&
				    (cave_feat[yy][xx] != FEAT_PERM_OUTER) &&
				    (cave_feat[yy][xx] != FEAT_PERM_SOLID))
					cave_set_feat(yy, xx, FEAT_FLOOR);
			} else if (typ == 4) {
				if ((cave_feat[yy][xx] == FEAT_FLOOR) ||
				    (cave_feat[yy][xx] == FEAT_DEEP_WATER) ||
				    (cave_feat[yy][xx] == FEAT_SHAL_WATER))
					cave_set_feat(yy, xx, FEAT_TREES);
			} else if (typ == 5) {
				if ((cave_feat[yy][xx] != FEAT_MOUNTAIN) &&
				    (cave_feat[yy][xx] != FEAT_PERM_EXTRA) &&
				    (cave_feat[yy][xx] != FEAT_PERM_INNER) &&
				    (cave_feat[yy][xx] != FEAT_PERM_OUTER) &&
				    (cave_feat[yy][xx] != FEAT_PERM_SOLID))
					cave_set_feat(yy, xx, FEAT_DEEP_LAVA);
			}
		}
	}


	/* Fully update the visuals */
	p_ptr->update |= (PU_FORGET_VIEW | PU_UPDATE_VIEW | PU_MONSTERS);

	/* Fully update the flow */
	p_ptr->update |= (PU_FORGET_FLOW | PU_UPDATE_FLOW);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD);
}


/*
 * Imprision
 * Allow "target" mode to pass over monsters
 * Affect grids, objects, and monsters
 */
bool imprision(int typ, int dir, int rad)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	int x,y,ty, tx;

	int flg = PROJECT_GRID | PROJECT_ITEM;

	/* Use the given direction */
	ty = py + 99 * ddy[dir];
	tx = px + 99 * ddx[dir];

	/* Hack -- Use an actual "target" */
	if ((dir == 5) && target_okay())
	{
		flg &= ~(PROJECT_STOP);
		tx = p_ptr->target_col;
		ty = p_ptr->target_row;
	}

	for (x = tx - rad;x <= tx + rad; x++)
		for (y = ty - rad; y <= ty + rad; y++)
			if (cave_empty_bold(y,x))  {
				cave_set_feat(y, x, typ);
				cave_info[y][x] |= (CAVE_GLOW | CAVE_MARK);
			}
/*	Term_fresh(); */
	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	return(TRUE);
}


/* From Zangband by Topi Ylinen */
void rustproof(void)
{
	int item;
	object_type *o_ptr;
	char o_name[80];
	cptr q,s;
	object_kind *k_ptr;
	item_tester_hook = item_tester_hook_armour;

	/* Get an item */
	q = "Rustproof which item? ";
	s = "You have no items to rustproof.";
	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	object_desc(o_name, o_ptr, FALSE, 0);
	k_ptr = &k_info[o_ptr->k_idx];
	k_ptr->flags3 |= TR3_IGNORE_ACID;

	if ((o_ptr->to_a < 0) && !(o_ptr->ident & IDENT_CURSED)) {
		msg_format("%s %s look%s as good as new!", ((item >= 0) ? "Your" : "The"), o_name, ((o_ptr->number > 1) ? "" : "s"));
		o_ptr->to_a = 0;
	}

	msg_format("%s %s %s now protected against corrosion.",
	    ((item >= 0) ? "Your" : "The"), o_name, ((o_ptr->number > 1) ? "are" : "is"));
}


/* From Kamband by Ivan Tkatchev */
void summon_monster(int sumtype)
{
	p_ptr->energy_use = 100;

	if (p_ptr->inside_special == 1) {
		msg_print("This place seems devoid of life.");
		msg_print(NULL);
		return;
	}

	if (summon_specific(p_ptr->py, p_ptr->px, p_ptr->depth+randint(5), sumtype, TRUE))
		msg_print("You summon some help.");
	else
		msg_print("You called, but no help came.");
}

