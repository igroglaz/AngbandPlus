/* option.c */

#include "option.h"

#ifdef __cplusplus
#include <cstddef>

#define TRUE true
#define FALSE false
#else
#include <stddef.h>
#include <stdbool.h>
#endif

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This work is free software; you can redistribute it and/or modify it
 * under the terms of either:
 *
 * a) the GNU General Public License as published by the Free Software
 *    Foundation, version 2, or
 *
 * b) the "Angband licence":
 *    This software may be copied and distributed for educational, research,
 *    and not for profit purposes provided that this copyright and statement
 *    are included in all such copies.  Other copyrights may also apply.
 */

const option_def options[OPT_MAX] =
{	
	{ "rogue_like_commands","Rogue-like commands", FALSE},
	{ "quick_messages", "Activate quick messages", TRUE},
	{ "floor_query_flag", "Prompt for floor item selection", FALSE},
	{ "carry_query_flag", "Prompt before picking things up", TRUE},
	{ "use_old_target", "Use old target by default", FALSE},
	{ "always_pickup", "Pick things up by default", TRUE},
	{ "always_repeat", "Repeat obvious commands", FALSE},
	{ "depth_in_feet", "Show dungeon level in feet", FALSE},
	{ "stack_force_notes", "Merge inscriptions when stacking", FALSE},
	{ NULL, NULL, FALSE},												/* XXX OPT_stack_force_costs XXX */
	{ "show_labels", "Show labels in equipment listings", TRUE},
	{ "show_weights", "Show weights in all object listings", TRUE},
	{ "show_choices", "Show choices in inven/equip windows", TRUE},
	{ "show_details", "Show details in monster descriptions", TRUE},
	{ "ring_bell", "Audible bell (on errors, etc)", FALSE},
	{ "show_flavors", "Show flavors in object descriptions", TRUE},

	{ "run_ignore_stairs", "When running, ignore stairs", FALSE},
	{ "run_ignore_doors", "When running, ignore doors", FALSE},
	{ "run_cut_corners", "When running, cut corners", TRUE},
	{ "run_use_corners", "When running, use corners", TRUE},
	{ "disturb_move", "Disturb whenever any monster moves", TRUE},
	{ "disturb_near", "Disturb whenever viewable monster moves", TRUE},
	{ "disturb_panel", "Disturb whenever map panel changes", TRUE},
	{ "disturb_state", "Disturb whenever player state changes", TRUE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ "verify_destroy", "Verify destruction of objects", TRUE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},

	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ "expand_look", "Expand the power of the look command", TRUE},
	{ NULL, NULL, FALSE},
	{ "view_perma_grids", "Map remembers all perma-lit grids", TRUE},
	{ "view_torch_grids", "Map remembers all torch-lit grids", FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},

	{ "view_reduce_lite", "Reduce lite-radius when running", FALSE},
	{ "hidden_player", "Hide player symbol when running", FALSE},
	{ "avoid_abort", "Avoid checking for user abort", FALSE},
	{ "avoid_other", "Avoid processing special colors", FALSE},
	{ "flush_failure", "Flush input on various failures", TRUE},
	{ "flush_disturb", "Flush input whenever disturbed", FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ "compress_savefile", "Compress messages in savefiles", TRUE},
	{ "hilite_player", "Hilite the player with the cursor", FALSE},
	{ "view_yellow_lite", "Use special colors for torch lite", TRUE},
	{ "view_bright_lite", "Use special colors for field of view", TRUE},
	{ "view_granite_lite", "Use special colors for wall grids", TRUE},
	{ "view_special_lite", "Use special colors for floor grids", TRUE},

	{ "easy_open", "Open/Disarm/Close without direction", FALSE},
	{ "easy_alter", "Open/Disarm doors/traps on movement", FALSE},
	{ "easy_floor", "Display floor stacks in a list", FALSE},
	{ "show_piles", "Show stacks using special attr/char", FALSE},
	{ "center_player", "Center map continuously (very slow)", FALSE},
	{ "run_avoid_center", "Avoid centering while running", FALSE},
	{ NULL, NULL, FALSE},
	{ "auto_more", "Automatically clear '-more-' prompts", FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},

	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},

	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},

	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},

	{ "birth_point_based", "Birth: Allow purchase of stats using points", FALSE},
	{ "birth_auto_roller", "Birth: Allow specification of minimal stats", FALSE},
	{ "birth_maximize", "Birth: Maximize effect of race/class bonuses", TRUE},
	{ "birth_preserve", "Birth: Preserve artifacts when leaving level", TRUE},
	{ "birth_ironman", "Birth: Restrict the use of stairs/recall", FALSE},
	{ "birth_no_stores", "Birth: Restrict the use of stores/home", FALSE},
	{ "birth_no_artifacts", "Birth: Restrict creation of artifacts", FALSE},
	{ "birth_rand_artifacts", "Birth: Randomize some of the artifacts (beta)", FALSE},
	{ "birth_no_stacking", "Birth: Don't stack objects on the floor", FALSE},
	{ "birth_auto_scum", "Birth: Auto-scum for good levels", FALSE},
	{ "birth_dungeon_align", "Birth: Generate dungeons with aligned rooms", TRUE},
	{ "birth_dungeon_stair", "Birth: Generate dungeons with connected stairs", TRUE},
	{ "birth_flow_by_sound", "Birth: Monsters chase current location", TRUE},
	{ "birth_flow_by_smell", "Birth: Monsters chase recent locations", TRUE},
	{ "birth_smart_monsters", "Birth: Monsters behave more intelligently", TRUE},
	{ "birth_smart_packs", "Birth: Monsters act smarter in groups", TRUE},

	{ "birth_smart_learn", "Birth: Monsters learn from their mistakes", FALSE},
	{ "birth_smart_cheat", "Birth: Monsters exploit players weaknesses", FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},

	{ "cheat_peek", "Cheat: Peek into object creation", FALSE},
	{ "cheat_hear", "Cheat: Peek into monster creation", FALSE},
	{ "cheat_room", "Cheat: Peek into dungeon creation", FALSE},
	{ "cheat_xtra", "Cheat: Peek into something else", FALSE},
	{ "cheat_know", "Cheat: Know complete monster info", FALSE},
	{ "cheat_live", "Cheat: Allow player to avoid death", FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},

	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},

	{ "adult_point_based", "Adult: Allow purchase of stats using points", FALSE},
	{ "adult_auto_roller", "Adult: Allow specification of minimal stats", FALSE},
	{ "adult_maximize", "Adult: Maximize effect of race/class bonuses", TRUE},
	{ "adult_preserve", "Adult: Preserve artifacts when leaving level", TRUE},
	{ "adult_ironman", "Adult: Restrict the use of stairs/recall", FALSE},
	{ "adult_no_stores", "Adult: Restrict the use of stores/home", FALSE},
	{ "adult_no_artifacts", "Adult: Restrict creation of artifacts", FALSE},
	{ "adult_rand_artifacts", "Adult: Randomize some of the artifacts (beta)", FALSE},
	{ "adult_no_stacking", "Adult: Don't stack objects on the floor", FALSE},
	{ "adult_auto_scum", "Adult: Auto-scum for good levels", FALSE},
	{ "adult_dungeon_align", "Adult: Generate dungeons with aligned rooms", TRUE},
	{ "adult_dungeon_stair", "Adult: Generate dungeons with connected stairs", TRUE},
	{ "adult_flow_by_sound", "Adult: Monsters chase current location", TRUE},
	{ "adult_flow_by_smell", "Adult: Monsters chase recent locations", TRUE},
	{ "adult_smart_monsters", "Adult: Monsters behave more intelligently", TRUE},
	{ "adult_smart_packs", "Adult: Monsters act smarter in groups", TRUE},

	{ "adult_smart_learn", "Adult: Monsters learn from their mistakes", FALSE},
	{ "adult_smart_cheat", "Adult: Monsters exploit players weaknesses", FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},

	{ "score_peek", "Score: Peek into object creation", FALSE},
	{ "score_hear", "Score: Peek into monster creation", FALSE},
	{ "score_room", "Score: Peek into dungeon creation", FALSE},
	{ "score_xtra", "Score: Peek into something else", FALSE},
	{ "score_know", "Score: Know complete monster info", FALSE},
	{ "score_live", "Score: Allow player to avoid death", FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},

	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE},
	{ NULL, NULL, FALSE}
};

/*
 * Option screen interface
 */
const unsigned char option_page[OPT_PAGE_MAX][OPT_PAGE_PER] =
{
	/*** User-Interface ***/

	{
		OPT_rogue_like_commands,
		OPT_quick_messages,
		OPT_floor_query_flag,
		OPT_carry_query_flag,
		OPT_use_old_target,
		OPT_always_pickup,
		OPT_always_repeat,
		OPT_stack_force_notes,
		OPT_ring_bell,
		OPT_easy_open,
		OPT_easy_alter,
		OPT_easy_floor,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE
	},

	/*** Disturbance ***/

	{
		OPT_run_ignore_stairs,
		OPT_run_ignore_doors,
		OPT_run_cut_corners,
		OPT_run_use_corners,
		OPT_disturb_move,
		OPT_disturb_near,
		OPT_disturb_panel,
		OPT_disturb_state,
		OPT_verify_destroy,
		OPT_auto_more,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE
	},

	/*** Game-Play ***/

	{
		OPT_expand_look,
		OPT_view_perma_grids,
		OPT_view_torch_grids,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE
	},

	/*** Efficiency ***/

	{
		OPT_view_reduce_lite,
		OPT_hidden_player,
		OPT_avoid_abort,
		OPT_avoid_other,
		OPT_flush_failure,
		OPT_flush_disturb,
		OPT_compress_savefile,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
	},

	/*** Display ***/

	{
		OPT_depth_in_feet,
		OPT_show_labels,
		OPT_show_weights,
		OPT_show_choices,
		OPT_show_details,
		OPT_show_flavors,
		OPT_hilite_player,
		OPT_view_yellow_lite,
		OPT_view_bright_lite,
		OPT_view_granite_lite,
		OPT_view_special_lite,
 		OPT_center_player,
 		OPT_run_avoid_center,
		OPT_show_piles,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE
	},

	/*** Birth ***/

	{
		OPT_birth_point_based,
		OPT_birth_auto_roller,
		OPT_birth_maximize,
		OPT_birth_preserve,
		OPT_birth_ironman,
		OPT_birth_no_stores,
		OPT_birth_no_artifacts,
		OPT_birth_rand_artifacts,
		OPT_birth_no_stacking,
		OPT_birth_auto_scum,
		OPT_birth_dungeon_align,
		OPT_birth_dungeon_stair,
		OPT_birth_flow_by_sound,
		OPT_birth_flow_by_smell,
		OPT_birth_smart_monsters,
		OPT_birth_smart_packs,
		OPT_birth_smart_learn,
		OPT_birth_smart_cheat,
		OPT_NONE,
		OPT_NONE
	},

	/*** Cheat ***/

	{
		OPT_cheat_peek,
		OPT_cheat_hear,
		OPT_cheat_room,
		OPT_cheat_xtra,
		OPT_cheat_know,
		OPT_cheat_live,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE
	}
};

