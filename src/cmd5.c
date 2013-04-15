/* File: cmd5.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#include "animeband.h"



/*
 * Allow user to choose a spell/prayer from the given book.
 *
 * If a valid spell is chosen, saves it in '*sn' and returns TRUE
 * If the user hits escape, returns FALSE, and set '*sn' to -1
 * If there are no legal choices, returns FALSE, and sets '*sn' to -2
 *
 * The "prompt" should be "cast", "recite", or "study"
 * The "known" should be TRUE for cast/pray, FALSE for study
 */
static int get_spell(int *sn, cptr prompt, int sval, bool known)
{
	int i;

	int spell;
	int num = 0;

	byte spells[PY_MAX_SPELLS];

	bool verify;

	bool flag, redraw, okay;
	char choice;

	const magic_type *s_ptr;

	char out_val[160];

	cptr p = ((cp_ptr->spell_book == TV_MAGIC_BOOK) ? "spell" : "prayer");

#ifdef ALLOW_REPEAT

	/* Get the spell, if available */
	if (repeat_pull(sn))
	{
		/* Verify the spell */
		if (spell_okay(*sn, known))
		{
			/* Success */
			return (TRUE);
		}
	}

#endif /* ALLOW_REPEAT */

	/* Extract spells */
	for (spell = 0; spell < PY_MAX_SPELLS; spell++)
	{
		/* Check for this spell */
		if ((spell < 32) ?
		    (spell_flags[cp_ptr->spell_type][sval][0] & (1L << spell)) :
		    (spell_flags[cp_ptr->spell_type][sval][1] & (1L << (spell - 32))))
		{
			/* Collect this spell */
			spells[num++] = spell;
		}
	}


	/* Assume no usable spells */
	okay = FALSE;

	/* Assume no spells available */
	(*sn) = -2;

	/* Check for "okay" spells */
	for (i = 0; i < num; i++)
	{
		/* Look for "okay" spells */
		if (spell_okay(spells[i], known)) okay = TRUE;
	}

	/* No "okay" spells */
	if (!okay) return (FALSE);


	/* Assume cancelled */
	*sn = (-1);

	/* Nothing chosen yet */
	flag = FALSE;

	/* No redraw yet */
	redraw = FALSE;

#if 0
	/* Show the list */
	if (redraw)
	{
		/* Save screen */
		screen_save();

		/* Display a list of spells */
		print_spells(spells, num, 1, 20);
	}

#endif


	/* Build a prompt (accept all spells) */
	strnfmt(out_val, 78, "(%^ss %c-%c, *=List, ESC=exit) %^s which %s? ",
	        p, I2A(0), I2A(num - 1), prompt, p);

	/* Get a spell from the user */
	while (!flag && get_com(out_val, &choice))
	{
		/* Request redraw */
		if ((choice == ' ') || (choice == '*') || (choice == '?'))
		{
			/* Hide the list */
			if (redraw)
			{
				/* Load screen */
				screen_load();

				/* Hide list */
				redraw = FALSE;
			}

			/* Show the list */
			else
			{
				/* Show list */
				redraw = TRUE;

				/* Save screen */
				screen_save();

				/* Display a list of spells */
				print_spells(spells, num, 1, 20);
			}

			/* Ask again */
			continue;
		}


		/* Note verify */
		verify = (isupper(choice) ? TRUE : FALSE);

		/* Lowercase */
		choice = tolower(choice);

		/* Extract request */
		i = (islower(choice) ? A2I(choice) : -1);

		/* Totally Illegal */
		if ((i < 0) || (i >= num))
		{
			bell("Illegal spell choice!");
			continue;
		}

		/* Save the spell index */
		spell = spells[i];

		/* Require "okay" spells */
		if (!spell_okay(spell, known))
		{
			bell("Illegal spell choice!");
			msg_format("You may not %s that %s.", prompt, p);
			continue;
		}

		/* Verify it */
		if (verify)
		{
			char tmp_val[160];

			/* Get the spell */
			s_ptr = &mp_ptr->info[spell];

			/* Prompt */
			strnfmt(tmp_val, 78, "%^s %s (%d mana, %d%% fail)? ",
			        prompt, spell_names[cp_ptr->spell_type][spell],
			        s_ptr->smana, spell_chance(spell));

			/* Belay that order */
			if (!get_check(tmp_val)) continue;
		}

		/* Stop the loop */
		flag = TRUE;
	}


	/* Restore the screen */
	if (redraw)
	{
		/* Load screen */
		screen_load();

		/* Hack -- forget redraw */
		/* redraw = FALSE; */
	}


	/* Abort if needed */
	if (!flag) return (FALSE);

	/* Save the choice */
	(*sn) = spell;

#ifdef ALLOW_REPEAT

	repeat_push(*sn);

#endif /* ALLOW_REPEAT */

	/* Success */
	return (TRUE);
}




/*
 * Peruse the spells/prayers in a Book
 *
 * Note that *all* spells in the book are listed
 *
 * Note that browsing is allowed while confused or blind,
 * and in the dark, primarily to allow browsing in stores.
 */
void do_cmd_browse(void)
{
	int item, sval;

	int spell;
	int num = 0;

	byte spells[PY_MAX_SPELLS];

	object_type *o_ptr;

	cptr q, s;


	/* Warriors are illiterate */
	if (!cp_ptr->spell_book)
	{
		msg_print("You cannot read books!");
		return;
	}

#if 0

	/* No lite */
	if (p_ptr->blind || no_lite())
	{
		msg_print("You cannot see!");
		return;
	}

	/* Confused */
	if (p_ptr->confused)
	{
		msg_print("You are too confused!");
		return;
	}

#endif

	/* Restrict choices to "useful" books */
	item_tester_tval = cp_ptr->spell_book;

	/* Get an item */
	q = "Browse which book? ";
	s = "You have no books that you can read.";
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

	/* Get the item's sval */
	sval = o_ptr->sval;


	/* Track the object kind */
	object_kind_track(o_ptr->k_idx);

	/* Hack -- Handle stuff */
	handle_stuff();


	/* Extract spells */
	for (spell = 0; spell < PY_MAX_SPELLS; spell++)
	{
		/* Check for this spell */
		if ((spell < 32) ?
		    (spell_flags[cp_ptr->spell_type][sval][0] & (1L << spell)) :
		    (spell_flags[cp_ptr->spell_type][sval][1] & (1L << (spell - 32))))
		{
			/* Collect this spell */
			spells[num++] = spell;
		}
	}


	/* Save screen */
	screen_save();

	/* Display the spells */
	print_spells(spells, num, 1, 20);

	/* Prompt for a command */
	put_str("(Browsing) Command: ", 0, 0);

	/* Hack -- Get a new command */
	p_ptr->command_new = inkey();

	/* Load screen */
	screen_load();


	/* Hack -- Process "Escape" */
	if (p_ptr->command_new == ESCAPE)
	{
		/* Reset stuff */
		p_ptr->command_new = 0;
	}
}




/*
 * Study a book to gain a new spell/prayer
 */
void do_cmd_study(void)
{
	int i, item, sval;

	int spell = -1;

	cptr p = ((cp_ptr->spell_book == TV_MAGIC_BOOK) ? "spell" : "prayer");

	cptr q, s;

	object_type *o_ptr;


	if (!cp_ptr->spell_book)
	{
		msg_print("You cannot read books!");
		return;
	}

	if (p_ptr->blind || no_lite())
	{
		msg_print("You cannot see!");
		return;
	}

	if (p_ptr->confused)
	{
		msg_print("You are too confused!");
		return;
	}

	if (!(p_ptr->new_spells))
	{
		msg_format("You cannot learn any new %ss!", p);
		return;
	}


	/* Restrict choices to "useful" books */
	item_tester_tval = cp_ptr->spell_book;

	/* Get an item */
	q = "Study which book? ";
	s = "You have no books that you can read.";
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

	/* Get the item's sval */
	sval = o_ptr->sval;


	/* Track the object kind */
	object_kind_track(o_ptr->k_idx);

	/* Hack -- Handle stuff */
	handle_stuff();


	/* Mage -- Learn a selected spell */
	if (cp_ptr->flags & CF_CHOOSE_SPELLS)
	{
		/* Ask for a spell, allow cancel */
		if (!get_spell(&spell, "study", sval, FALSE) && (spell == -1)) return;
	}
	else
	{
		int k = 0;

		int gift = -1;

		/* Extract spells */
		for (spell = 0; spell < PY_MAX_SPELLS; spell++)
		{
			/* Check spells in the book */
			if ((spell < 32) ?
			    (spell_flags[cp_ptr->spell_type][sval][0] & (1L << spell)) :
			    (spell_flags[cp_ptr->spell_type][sval][1] & (1L << (spell - 32))))
			{
				/* Skip non "okay" prayers */
				if (!spell_okay(spell, FALSE)) continue;

				/* Apply the randomizer */
				if ((++k > 1) && (rand_int(k) != 0)) continue;

				/* Track it */
				gift = spell;
			}
		}

		/* Accept gift */
		spell = gift;
	}

	/* Nothing to study */
	if (spell < 0)
	{
		/* Message */
		msg_format("You cannot learn any %ss in that book.", p);

		/* Abort */
		return;
	}


	/* Take a turn */
	p_ptr->energy_use = 100;

	/* Learn the spell */
	if (spell < 32)
	{
		p_ptr->spell_learned1 |= (1L << spell);
	}
	else
	{
		p_ptr->spell_learned2 |= (1L << (spell - 32));
	}

	/* Find the next open entry in "spell_order[]" */
	for (i = 0; i < PY_MAX_SPELLS; i++)
	{
		/* Stop at the first empty space */
		if (p_ptr->spell_order[i] == 99) break;
	}

	/* Add the spell to the known list */
	p_ptr->spell_order[i] = spell;

	/* Mention the result */
	message_format(MSG_STUDY, 0, "You have learned the %s of %s.",
	           p, spell_names[cp_ptr->spell_type][spell]);

	/* One less spell available */
	p_ptr->new_spells--;

	/* Message if needed */
	if (p_ptr->new_spells)
	{
		/* Message */
		msg_format("You can learn %d more %s%s.",
		           p_ptr->new_spells, p,
		           (p_ptr->new_spells != 1) ? "s" : "");
	}

	/* Save the new_spells value */
	p_ptr->old_spells = p_ptr->new_spells;

	/* Redraw Study Status */
	p_ptr->redraw |= (PR_STUDY);

	/* Redraw object recall */
	p_ptr->window |= (PW_OBJECT);
}



/*
 * Cast a spell
 */
void do_cmd_cast(void)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	int item, sval, spell, dir;
	int chance, beam;
	int wrath;

	int plev = p_ptr->lev;

	object_type *o_ptr;

	const magic_type *s_ptr;

	cptr q, s;


	/* Require spell ability */
	if (cp_ptr->spell_book != TV_MAGIC_BOOK)
	{
		msg_print("You cannot cast spells!");
		return;
	}

	/* Require lite */
	if (p_ptr->blind || no_lite())
	{
		msg_print("You cannot see!");
		return;
	}

	/* Not when confused */
	if (p_ptr->confused)
	{
		msg_print("You are too confused!");
		return;
	}


	/* Restrict choices to spell books */
	item_tester_tval = cp_ptr->spell_book;

	/* Get an item */
	q = "Use which book? ";
	s = "You have no spell books!";
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

	/* Get the item's sval */
	sval = o_ptr->sval;


	/* Track the object kind */
	object_kind_track(o_ptr->k_idx);

	/* Hack -- Handle stuff */
	handle_stuff();


	/* Ask for a spell */
	if (!get_spell(&spell, "cast", sval, TRUE))
	{
		if (spell == -2) msg_print("You don't know any spells in that book.");
		return;
	}


	/* Get the spell */
	s_ptr = &mp_ptr->info[spell];


	/* Verify "dangerous" spells */
	if (s_ptr->smana > p_ptr->csp)
	{
		/* Warning */
		msg_print("You do not have enough mana to cast this spell.");

		/* Flush input */
		flush();

		/* Verify */
		if (!get_check("Attempt it anyway? ")) return;
	}


	/* Spell failure chance */
	chance = spell_chance(spell);

	/* Failed spell */
	if (rand_int(100) < chance)
	{
		if (flush_failure) flush();
		msg_print("You failed to get the spell off!");
	}

	/* Process spell */
	else
	{
		/* Hack -- chance of "beam" instead of "bolt" */
		beam = ((cp_ptr->flags & CF_BEAM) ? plev : (plev / 2));

		/* Spells. */
		switch (spell)
		{
			case SPELL_MAGIC_MISSILE:
			{
				if (!get_aim_dir(&dir)) return;
				fire_bolt_or_beam(beam-10, GF_MISSILE, dir,
				                  damroll(3 + ((plev - 1) / 5), 4));
				break;
			}

			case SPELL_DETECT_MONSTERS:
			{
				(void)detect_monsters_normal();
				break;
			}

			case SPELL_PHASE_DOOR:
			{
				teleport_player(10);
				break;
			}

			case SPELL_LIGHT_AREA:
			{
				(void)lite_area(damroll(2, (plev / 2)), (plev / 10) + 1);
				break;
			}

			case SPELL_TREASURE_DETECTION:
			{
				(void)detect_treasure();
				(void)detect_objects_gold();
				break;
			}

			case SPELL_CURE_LIGHT_WOUNDS:
			{
				(void)hp_player(damroll(2, 8));
				(void)set_cut(p_ptr->cut - 15);
				break;
			}

			case SPELL_OBJECT_DETECTION:
			{
				(void)detect_objects_normal();
				break;
			}

			case SPELL_FIND_TRAPS_DOORS:
			{
				(void)detect_traps();
				(void)detect_doors();
				(void)detect_stairs();
				break;
			}

			case SPELL_STINKING_CLOUD:
			{
				if (!get_aim_dir(&dir)) return;
				fire_ball(GF_POIS, dir,
				          10 + (plev / 2), 2);
				break;
			}

			case SPELL_CONFUSE_MONSTER:
			{
				if (!get_aim_dir(&dir)) return;
				(void)confuse_monster(dir, plev);
				break;
			}

			case SPELL_LIGHTNING_BOLT:
			{
				if (!get_aim_dir(&dir)) return;
				fire_bolt_or_beam(beam-10, GF_ELEC, dir,
				                  damroll(3+((plev-5)/4), 8));
				break;
			}

			case SPELL_TRAP_DOOR_DESTRUCTION:
			{
				(void)destroy_doors_touch();
				break;
			}

			case SPELL_SLEEP_I:
			{
				if (!get_aim_dir(&dir)) return;
				(void)sleep_monster(dir);
				break;
			}

			case SPELL_CURE_POISON:
			{
				(void)set_poisoned(0);
				break;
			}

			case SPELL_TELEPORT_SELF:
			{
				teleport_player(plev * 5);
				break;
			}

			case SPELL_SPEAR_OF_LIGHT:
			{
				if (!get_aim_dir(&dir)) return;
				msg_print("A line of blue shimmering light appears.");
				lite_line(dir);
				break;
			}

			case SPELL_FROST_BOLT:
			{
				if (!get_aim_dir(&dir)) return;
				fire_bolt_or_beam(beam-10, GF_COLD, dir,
				                  damroll(5+((plev-5)/4), 8));
				break;
			}

			case SPELL_TURN_STONE_TO_MUD:
			{
				if (!get_aim_dir(&dir)) return;
				(void)wall_to_mud(dir);
				break;
			}

			case SPELL_SATISFY_HUNGER:
			{
				(void)set_food(PY_FOOD_MAX - 1);
				break;
			}

			case SPELL_RECHARGE_ITEM_I:
			{
				(void)recharge(5);
				break;
			}

			case SPELL_SLEEP_II:
			{
				(void)sleep_monsters_touch();
				break;
			}

			case SPELL_POLYMORPH_OTHER:
			{
				if (!get_aim_dir(&dir)) return;
				(void)poly_monster(dir);
				break;
			}

			case SPELL_IDENTIFY:
			{
				(void)ident_spell();
				break;
			}

			case SPELL_SLEEP_III:
			{
				(void)sleep_monsters();
				break;
			}

			case SPELL_FIRE_BOLT:
			{
				if (!get_aim_dir(&dir)) return;
				fire_bolt_or_beam(beam, GF_FIRE, dir,
				                  damroll(8+((plev-5)/4), 8));
				break;
			}

			case SPELL_SLOW_MONSTER:
			{
				if (!get_aim_dir(&dir)) return;
				(void)slow_monster(dir);
				break;
			}

			case SPELL_SHADOW_FLARE:
			{
				if (!get_aim_dir(&dir)) return;
				fire_ball(GF_DARK, dir,
				          30 + (plev), 2);
				break;
			}

			case SPELL_RECHARGE_ITEM_II:
			{
				(void)recharge(40);
				break;
			}

			case SPELL_TELEPORT_OTHER:
			{
				if (!get_aim_dir(&dir)) return;
				(void)teleport_monster(dir);
				break;
			}

			case SPELL_HASTE_SELF:
			{
				if (!p_ptr->fast)
				{
					(void)set_fast(randint(20) + plev);
				}
				else
				{
					(void)set_fast(p_ptr->fast + randint(5));
				}
				break;
			}

			case SPELL_STAR_FLARE:
			{
				if (!get_aim_dir(&dir)) return;
				fire_ball(GF_LITE, dir,
				          55 + (plev), 2);
				break;
			}

			case SPELL_WORD_OF_DESTRUCTION:
			{
				destroy_area(py, px, 15, TRUE);
				break;
			}

			case SPELL_SOUTHERN_CROSS:
			{
				fire_beam(GF_METEOR, 2,
				          80 + (plev));
				fire_beam(GF_METEOR, 4,
				          80 + (plev));
				fire_beam(GF_METEOR, 6,
				          80 + (plev));
				fire_beam(GF_METEOR, 8,
				          80 + (plev));
				break;
			}

			case SPELL_DOOR_CREATION:
			{
				(void)door_creation();
				break;
			}

			case SPELL_STAIR_CREATION:
			{
				(void)stair_creation();
				break;
			}

			case SPELL_TELEPORT_LEVEL:
			{
				(void)teleport_player_level();
				break;
			}

			case SPELL_SMOKE_BOMB:
			{
				fire_ball(GF_CONFUSION, 0, 0, 10);
				fire_ball(GF_SOUND, 0, 0, 10);
				break;
			}

			case SPELL_WORD_OF_RECALL:
			{
				set_recall();
				break;
			}

			case SPELL_ACID_BOLT:
			{
				if (!get_aim_dir(&dir)) return;
				fire_bolt_or_beam(beam, GF_ACID, dir,
				                  damroll(6+((plev-5)/4), 8));
				break;
			}

			case SPELL_FIRE_BALL:
			{
				if (!get_aim_dir(&dir)) return;
				fire_ball(GF_FIRE, dir,
				          20 + (plev / 2), 3);
				break;
			}

			case SPELL_ACID_BALL:
			{
				if (!get_aim_dir(&dir)) return;
				fire_ball(GF_ACID, dir,
				          40 + (plev), 2);
				break;
			}

			case SPELL_RAH_TILT:
			{
				if (!get_aim_dir(&dir)) return;
				fire_bolt_or_beam(beam, GF_MANA, dir,
				                  (plev) * 8);
				break;
			}

			case SPELL_DRAGON_SLAVE:
			{
				if (!get_aim_dir(&dir)) return;
				fire_ball(GF_METEOR, dir,
				          (plev) * 10, 3);
				break;
			}

			case SPELL_GIGA_SLAVE:
			{
				if (!get_aim_dir(&dir)) return;
				
				if (rand_int(100) < 10){ /* 10% of nuking the Universe */

				msg_print("You were unable to contain the Giga Slave!");
				take_hit(4500, "invoking a Giga Slave");

				}

				else fire_ball(GF_MANA, dir,
				          4000 + (plev * 10), 12);
				break;
			}

			case SPELL_DETECT_EVIL:
			{
				(void)detect_monsters_evil();
				break;
			}

			case SPELL_DETECT_ENCHANTMENT:
			{
				(void)detect_objects_magic();
				break;
			}

			case SPELL_RECHARGE_ITEM_III:
			{
				recharge(100);
				break;
			}

			case SPELL_GENOCIDE2:
			{
				(void)genocide();
				break;
			}

			case SPELL_MASS_GENOCIDE:
			{
				(void)mass_genocide();
				break;
			}

			case SPELL_FOEHN:
			{
				if (!get_aim_dir(&dir)) return;
				fire_ball(GF_FORCE, dir,
				          (plev * 2), 12);
				break;
			}

			case SPELL_NOAH:
			{
				if (!get_aim_dir(&dir)) return;
				fire_ball(GF_WATER, dir,
				          (plev * 2) + 20, 12);
				break;
			}

			case SPELL_PROTECTION:
			{
				(void)set_shield(p_ptr->shield + randint(20) + 30);
				break;
			}

			case SPELL_EARTHQUAKE:
			{
				earthquake(py, px, 12);
				break;
			}

			case SPELL_ANTI:
			{
				int time = randint(20) + 20;
				(void)set_oppose_acid(p_ptr->oppose_acid + time);
				(void)set_oppose_elec(p_ptr->oppose_elec + time);
				(void)set_oppose_fire(p_ptr->oppose_fire + time);
				(void)set_oppose_cold(p_ptr->oppose_cold + time);
				(void)set_oppose_pois(p_ptr->oppose_pois + time);
				break;
			}

			case SPELL_HEROISM:
			{
				(void)hp_player(10);
				(void)set_hero(p_ptr->hero + randint(25) + 25);
				(void)set_afraid(0);
				break;
			}

			case SPELL_KAMEHAMEHA:
			{
				msg_print("You scream out 'Kamehameha'!");
				if (!get_aim_dir(&dir)) return;
				fire_beam(GF_MANA, dir,
				          (plev * 3));
				break;
			}

			case SPELL_BERSERKER:
			{
				(void)hp_player(30);
				(void)set_shero(p_ptr->shero + randint(25) + 25);
				(void)set_afraid(0);
				break;
			}

			case SPELL_ESSENCE_OF_SPEED:
			{
				if (!p_ptr->fast)
				{
					(void)set_fast(randint(30) + 30 + plev);
				}
				else
				{
					(void)set_fast(p_ptr->fast + randint(10));
				}
				break;
			}

			case SPELL_WRATH:
			{
				if (!get_aim_dir(&dir)) return;
				msg_print("You scream as you rapidly fling ki bolts!");
				for (wrath = 0; wrath < plev; wrath++)
				fire_bolt_or_beam(beam-10, GF_MISSILE, dir,
				                  damroll(3, 8));
				msg_print("You become tired from the effort.");
				(void)set_slow(p_ptr->slow + rand_int(10) + 4);
				/*(void)set_invuln(p_ptr->invuln + randint(8) + 8); Original GOI */
				break;
			}
			

		}

		/* A spell was cast */
		if (!((spell < 32) ?
		      (p_ptr->spell_worked1 & (1L << spell)) :
		      (p_ptr->spell_worked2 & (1L << (spell - 32)))))
		{
			int e = s_ptr->sexp;

			/* The spell worked */
			if (spell < 32)
			{
				p_ptr->spell_worked1 |= (1L << spell);
			}
			else
			{
				p_ptr->spell_worked2 |= (1L << (spell - 32));
			}

			/* Gain experience */
			gain_exp(e * s_ptr->slevel);

			/* Redraw object recall */
			p_ptr->window |= (PW_OBJECT);
		}
	}

	/* Take a turn */
	p_ptr->energy_use = 100;

	/* Sufficient mana */
	if (s_ptr->smana <= p_ptr->csp)
	{
		/* Use some mana */
		p_ptr->csp -= s_ptr->smana;
	}

	/* Over-exert the player */
	else
	{
		int oops = s_ptr->smana - p_ptr->csp;

		/* No mana left */
		p_ptr->csp = 0;
		p_ptr->csp_frac = 0;

		/* Message */
		msg_print("You faint from the effort!");

		/* Hack -- Bypass free action */
		(void)set_paralyzed(p_ptr->paralyzed + randint(5 * oops + 1));

		/* Damage CON (possibly permanently) */
		if (rand_int(100) < 50)
		{
			bool perm = (rand_int(100) < 25);

			/* Message */
			msg_print("You have damaged your health!");

			/* Reduce constitution */
			(void)dec_stat(A_CON, 15 + randint(10), perm);
		}
	}

	/* Redraw mana */
	p_ptr->redraw |= (PR_MANA);

	/* Window stuff */
	p_ptr->window |= (PW_PLAYER_0 | PW_PLAYER_1);
}


/*
 * Brand the current weapon
 */
static void brand_weapon(void)
{
	object_type *o_ptr;

	o_ptr = &inventory[INVEN_WIELD];

	/* you can never modify artifacts / ego-items */
	/* you can never modify broken / cursed items */
	if ((o_ptr->k_idx) &&
	    (!artifact_p(o_ptr)) && (!ego_item_p(o_ptr)) &&
	    (!broken_p(o_ptr)) && (!cursed_p(o_ptr)))
	{
		cptr act;

		char o_name[80];

		if (rand_int(100) < 25)
		{
			act = "is covered in a fiery shield!";
			o_ptr->name2 = EGO_BRAND_FIRE;
		}
		else
		{
			act = "glows deep, icy blue!";
			o_ptr->name2 = EGO_BRAND_COLD;
		}

		object_desc(o_name, o_ptr, FALSE, 0);

		msg_format("Your %s %s", o_name, act);

		enchant(o_ptr, rand_int(3) + 4, ENCH_TOHIT | ENCH_TODAM);
	}

	else
	{
		if (flush_failure) flush();
		msg_print("The Branding failed.");
	}
}

/* Doing a "Super" (Limit Break) */
void do_cmd_super(void)
{
 
int plev = p_ptr->lev;	
int dir;
int rand_limit = rand_int(LB_GENEI_JIN);
int selector;

/*	#define LB_GLOBE					0   For my own benefit
	#define LB_FAIRY_HEAL               1   
	#define LB_SUPER_SAYIAN             2   
	#define LB_HYPER_ARMOR              3   
	#define LB_NUKE                     4   
	#define LB_WHIRLWIND				5   
	#define LB_AHVB						6   
	#define LB_POWER_BLAST				7   
	#define LB_KEKKAI					8
#define LB_AHVB					9   
#define LB_FAIL					10  
#define LB_RAGING_DEAMON			11  
#define LB_RANDOM					12  */

											
	

	/* Must have meter*/
	if (p_ptr->c_meter < p_ptr->m_meter)
	{
		msg_print("You have insufficient meter!");
		return;
	}

	/* Remnant exception */
	if (p_ptr->l_break == LB_RANDOM)
		selector = rand_limit;
		/*selector = LB_GENEI_JIN;*/
	else
		selector = p_ptr->l_break;

	
	/* Determine what kind of limit break used*/
	switch (selector)
	{

	case LB_GLOBE:
		{

		msg_print("You start to laugh maniacally!");
		(void)set_invuln(p_ptr->invuln + plev + 10);
		break;

		}

	case LB_FAIRY_HEAL:
		{

		msg_print("Fairies dance around you!");
		
			(void)restore_level();
			(void)set_poisoned(0);
			(void)set_blind(0);
			(void)set_confused(0);
			(void)set_image(0);
			(void)set_stun(0);
			(void)set_cut(0);
			(void)do_res_stat(A_STR);
			(void)do_res_stat(A_CON);
			(void)do_res_stat(A_DEX);
			(void)do_res_stat(A_WIS);
			(void)do_res_stat(A_INT);
			(void)do_res_stat(A_CHR);

			/* Recalculate max. hitpoints */
			update_stuff();

			(void)hp_player(5000);

		if (p_ptr->csp < p_ptr->msp)
			{
				p_ptr->csp = p_ptr->msp;
				p_ptr->csp_frac = 0;
				p_ptr->redraw |= (PR_MANA);
				p_ptr->window |= (PW_PLAYER_0 | PW_PLAYER_1);
			}
	break;
	}

	case LB_SUPER_SAYIAN:
		{
			(void)set_stun(0);
			(void)hp_player(200);
			(void)set_s_sayian(p_ptr->s_sayian + plev + 10);
			break;
		}

	case LB_HYPER_ARMOR:
		{
			(void)set_shield(p_ptr->shield + plev * 4);
			(void)set_protevil(p_ptr->protevil + plev * 4);
			break;
		}

	case LB_NUKE:
		{
			msg_print("You tiger knee into the air and drop a nuke!");
			fire_ball(GF_FIRE, 0,
				          plev * 50, 13);
			take_hit(100, "Nuclear Explosion");
			break;
			
		}

	case LB_WHIRLWIND:
		{
			msg_print("You unleash a whirlwind of fury swipes!");
			fire_ball(GF_FORCE, 0, plev * 25, 1);
			teleport_player(15);
			break;
		}

	case LB_TENTACLE:
		{
			msg_print("You start molesting all the creatures around you!");
			fire_ball(GF_MISSILE, 0, plev * 75, 1);

			break;
		}

	case LB_POWER_BLAST: /*misnamed*/
		{
		p_ptr->word_recall = 1;
		break;
		}
	case LB_KEKKAI:
		{
			msg_print("You clasp your hands together and form a Kekkai.");
			(void)set_kekkai(p_ptr->kekkai + plev);
			break;
		}

	case LB_AHVB:
		{
			if (!get_aim_dir(&dir)) return;
			msg_print("You tiger knee into the air and unleash a beam!");
			fire_beam(GF_METEOR, dir,
				          plev * 20);
			break;
		}
	case LB_FAIL:
		{
			msg_print("Nothing happens.....");
			break;
		}
	case LB_GENEI_JIN:
		{
			(void)set_geneijin(p_ptr->geneijin + 10 + plev/10);
			break;


		}

	
	default:
		{
			msg_print("Invalid super");
		}
	}

	/* consume all meter */
	p_ptr->c_meter = 0;
	p_ptr->redraw |= (PR_METER);

	/* Take a turn */
	p_ptr->energy_use = 100;

}


/*
 * Pray a prayer
 */
void do_cmd_pray(void)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	int item, sval, spell, dir, chance;

	int plev = p_ptr->lev;

	object_type *o_ptr;

	const magic_type *s_ptr;

	cptr q, s;


	/* Must use prayer books */
	if (cp_ptr->spell_book != TV_PRAYER_BOOK)
	{
		msg_print("Pray hard enough and your prayers may be answered.");
		return;
	}

	/* Must have lite */
	if (p_ptr->blind || no_lite())
	{
		msg_print("You cannot see!");
		return;
	}

	/* Must not be confused */
	if (p_ptr->confused)
	{
		msg_print("You are too confused!");
		return;
	}


	/* Restrict choices */
	item_tester_tval = cp_ptr->spell_book;

	/* Get an item */
	q = "Use which book? ";
	s = "You have no prayer books!";
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

	/* Get the item's sval */
	sval = o_ptr->sval;


	/* Track the object kind */
	object_kind_track(o_ptr->k_idx);

	/* Hack -- Handle stuff */
	handle_stuff();


	/* Choose a spell */
	if (!get_spell(&spell, "recite", sval, TRUE))
	{
		if (spell == -2) msg_print("You don't know any prayers in that book.");
		return;
	}


	/* Get the spell */
	s_ptr = &mp_ptr->info[spell];


	/* Verify "dangerous" prayers */
	if (s_ptr->smana > p_ptr->csp)
	{
		/* Warning */
		msg_print("You do not have enough mana to recite this prayer.");

		/* Flush input */
		flush();

		/* Verify */
		if (!get_check("Attempt it anyway? ")) return;
	}


	/* Spell failure chance */
	chance = spell_chance(spell);

	/* Check for failure */
	if (rand_int(100) < chance)
	{
		if (flush_failure) flush();
		msg_print("You failed to concentrate hard enough!");
	}

	/* Success */
	else
	{
		switch (spell)
		{
			case PRAYER_DETECT_EVIL:
			{
				(void)detect_monsters_evil();
				break;
			}

			case PRAYER_CURE_LIGHT_WOUNDS:
			{
				(void)hp_player(damroll(2, 10));
				(void)set_cut(p_ptr->cut - 10);
				break;
			}

			case PRAYER_BLESS:
			{
				(void)set_blessed(p_ptr->blessed + randint(12) + 12);
				break;
			}

			case PRAYER_REMOVE_FEAR:
			{
				(void)set_afraid(0);
				break;
			}

			case PRAYER_CALL_LIGHT:
			{
				(void)lite_area(damroll(2, (plev / 2)), (plev / 10) + 1);
				break;
			}

			case PRAYER_FIND_TRAPS:
			{
				(void)detect_traps();
				break;
			}

			case PRAYER_DETECT_DOORS_STAIRS:
			{
				(void)detect_doors();
				(void)detect_stairs();
				break;
			}

			case PRAYER_SLOW_POISON:
			{
				(void)set_poisoned(p_ptr->poisoned / 2);
				break;
			}

			case PRAYER_SCARE_MONSTER:
			{
				if (!get_aim_dir(&dir)) return;
				(void)fear_monster(dir, plev);
				break;
			}

			case PRAYER_PORTAL:
			{
				teleport_player(plev * 3);
				break;
			}

			case PRAYER_CURE_SERIOUS_WOUNDS:
			{
				(void)hp_player(damroll(4, 10));
				(void)set_cut((p_ptr->cut / 2) - 20);
				break;
			}

			case PRAYER_CHANT:
			{
				(void)set_blessed(p_ptr->blessed + randint(24) + 24);
				break;
			}

			case PRAYER_SANCTUARY:
			{
				(void)sleep_monsters_touch();
				break;
			}

			case PRAYER_SATISFY_HUNGER:
			{
				(void)set_food(PY_FOOD_MAX - 1);
				break;
			}

			case PRAYER_REMOVE_CURSE:
			{
				remove_curse();
				break;
			}

			case PRAYER_RESIST_HEAT_COLD:
			{
				(void)set_oppose_fire(p_ptr->oppose_fire + randint(10) + 10);
				(void)set_oppose_cold(p_ptr->oppose_cold + randint(10) + 10);
				break;
			}

			case PRAYER_NEUTRALIZE_POISON:
			{
				(void)set_poisoned(0);
				break;
			}

			case PRAYER_ORB_OF_DRAINING:
			{
				if (!get_aim_dir(&dir)) return;
				fire_ball(GF_HOLY_ORB, dir,
				          (damroll(3, 6) + plev +
				           (plev / ((cp_ptr->flags & CF_BLESS_WEAPON) ? 2 : 4))),
				          ((plev < 30) ? 2 : 3));
				break;
			}

			case PRAYER_CURE_CRITICAL_WOUNDS:
			{
				(void)hp_player(damroll(6, 10));
				(void)set_cut(0);
				break;
			}

			case PRAYER_SENSE_INVISIBLE:
			{
				(void)set_tim_invis(p_ptr->tim_invis + randint(24) + 24);
				break;
			}

			case PRAYER_PROTECTION_FROM_EVIL:
			{
				(void)set_protevil(p_ptr->protevil + randint(25) + 3 * p_ptr->lev);
				break;
			}

			case PRAYER_EARTHQUAKE:
			{
				earthquake(py, px, 10);
				break;
			}

			case PRAYER_SENSE_SURROUNDINGS:
			{
				map_area();
				break;
			}

			case PRAYER_CURE_MORTAL_WOUNDS:
			{
				(void)hp_player(damroll(8, 10));
				(void)set_stun(0);
				(void)set_cut(0);
				break;
			}

			case PRAYER_TURN_UNDEAD:
			{
				(void)turn_undead();
				break;
			}

			case PRAYER_PRAYER:
			{
				(void)set_blessed(p_ptr->blessed + randint(48) + 48);
				break;
			}

			case PRAYER_DISPEL_UNDEAD:
			{
				(void)dispel_undead(randint(plev * 3));
				break;
			}

			case PRAYER_HEAL:
			{
				(void)hp_player(300);
				(void)set_stun(0);
				(void)set_cut(0);
				break;
			}

			case PRAYER_DISPEL_EVIL:
			{
				(void)dispel_evil(randint(plev * 3));
				break;
			}

			case PRAYER_GLYPH_OF_WARDING:
			{
				warding_glyph();
				break;
			}

			case PRAYER_HOLY_WORD:
			{
				(void)dispel_evil(randint(plev * 4));
				(void)hp_player(1000);
				(void)set_afraid(0);
				(void)set_poisoned(0);
				(void)set_stun(0);
				(void)set_cut(0);
				break;
			}

			case PRAYER_DETECT_MONSTERS:
			{
				(void)detect_monsters_normal();
				break;
			}

			case PRAYER_DETECTION:
			{
				(void)detect_all();
				break;
			}

			case PRAYER_PERCEPTION:
			{
				(void)ident_spell();
				break;
			}

			case PRAYER_PROBING:
			{
				(void)probing();
				break;
			}

			case PRAYER_CLAIRVOYANCE:
			{
				wiz_lite();
				break;
			}

			case PRAYER_CURE_SERIOUS_WOUNDS2:
			{
				(void)hp_player(damroll(4, 10));
				(void)set_cut(0);
				break;
			}

			case PRAYER_CURE_MORTAL_WOUNDS2:
			{
				(void)hp_player(damroll(8, 10));
				(void)set_stun(0);
				(void)set_cut(0);
				break;
			}

			case PRAYER_HEALING:
			{
				(void)hp_player(2000);
				(void)set_stun(0);
				(void)set_cut(0);
				break;
			}

			case PRAYER_RESTORATION:
			{
				(void)do_res_stat(A_STR);
				(void)do_res_stat(A_INT);
				(void)do_res_stat(A_WIS);
				(void)do_res_stat(A_DEX);
				(void)do_res_stat(A_CON);
				(void)do_res_stat(A_CHR);
				break;
			}

			case PRAYER_REMEMBRANCE:
			{
				(void)restore_level();
				break;
			}

			case PRAYER_DISPEL_UNDEAD2:
			{
				(void)dispel_undead(randint(plev * 4));
				break;
			}

			case PRAYER_DISPEL_EVIL2:
			{
				(void)dispel_evil(randint(plev * 4));
				break;
			}

			case PRAYER_BANISHMENT:
			{
				if (banish_evil(100))
				{
					msg_print("The power of your god banishes evil!");
				}
				break;
			}

			case PRAYER_ARMAGEDDON:
			{
				msg_print("You feel a searing heat!");
				fire_ball(GF_METEOR, 0,
				          plev * 5, 15);
				destroy_area(py, px, 15, TRUE);
				break;
			}

			case PRAYER_NOAH:
				{
				if (!get_aim_dir(&dir)) return;
				fire_ball(GF_WATER, dir,
				          (plev * 2) + 20, 12);
				break;
				}

			case PRAYER_JUDGEMENT_DAY:
			{
				if (!get_aim_dir(&dir)) return;
				drain_life(dir, 400);
				break;
			}

			case PRAYER_UNBARRING_WAYS:
			{
				(void)destroy_doors_touch();
				break;
			}

			case PRAYER_RECHARGING:
			{
				(void)recharge(15);
				break;
			}

			case PRAYER_DISPEL_CURSE:
			{
				(void)remove_all_curse();
				break;
			}

			case PRAYER_ENCHANT_WEAPON:
			{
				(void)enchant_spell(rand_int(4) + 1, rand_int(4) + 1, 0);
				break;
			}

			case PRAYER_ENCHANT_ARMOUR:
			{
				(void)enchant_spell(0, 0, rand_int(3) + 2);
				break;
			}

			case PRAYER_ELEMENTAL_BRAND:
			{
				brand_weapon();
				break;
			}

			case PRAYER_BLINK:
			{
				teleport_player(10);
				break;
			}

			case PRAYER_TELEPORT_SELF:
			{
				teleport_player(plev * 8);
				break;
			}

			case PRAYER_TELEPORT_OTHER:
			{
				if (!get_aim_dir(&dir)) return;
				(void)teleport_monster(dir);
				break;
			}

			case PRAYER_TELEPORT_LEVEL:
			{
				(void)teleport_player_level();
				break;
			}

			case PRAYER_WORD_OF_RECALL:
			{
				set_recall();
				break;
			}

			case PRAYER_ALTER_REALITY:
			{
				msg_print("The world changes!");

				/* Leaving */
				p_ptr->leaving = TRUE;

				break;
			}
		}

		/* A prayer was prayed */
		if (!((spell < 32) ?
		      (p_ptr->spell_worked1 & (1L << spell)) :
		      (p_ptr->spell_worked2 & (1L << (spell - 32)))))
		{
			int e = s_ptr->sexp;

			/* The spell worked */
			if (spell < 32)
			{
				p_ptr->spell_worked1 |= (1L << spell);
			}
			else
			{
				p_ptr->spell_worked2 |= (1L << (spell - 32));
			}

			/* Gain experience */
			gain_exp(e * s_ptr->slevel);

			/* Redraw object recall */
			p_ptr->window |= (PW_OBJECT);
		}
	}

	/* Take a turn */
	p_ptr->energy_use = 100;

	/* Sufficient mana */
	if (s_ptr->smana <= p_ptr->csp)
	{
		/* Use some mana */
		p_ptr->csp -= s_ptr->smana;
	}

	/* Over-exert the player */
	else
	{
		int oops = s_ptr->smana - p_ptr->csp;

		/* No mana left */
		p_ptr->csp = 0;
		p_ptr->csp_frac = 0;

		/* Message */
		msg_print("You faint from the effort!");

		/* Hack -- Bypass free action */
		(void)set_paralyzed(p_ptr->paralyzed + randint(5 * oops + 1));

		/* Damage CON (possibly permanently) */
		if (rand_int(100) < 50)
		{
			bool perm = (rand_int(100) < 25);

			/* Message */
			msg_print("You have damaged your health!");

			/* Reduce constitution */
			(void)dec_stat(A_CON, 15 + randint(10), perm);
		}
	}

	/* Redraw mana */
	p_ptr->redraw |= (PR_MANA);

	/* Window stuff */
	p_ptr->window |= (PW_PLAYER_0 | PW_PLAYER_1);
}

/* Taunt */
void do_cmd_taunt(void)
{

	int counter = randint(4);

	switch (counter)
	{
	case 1: { msg_print("You shout out a challenge."); break; }
	case 2: { msg_print("You yawn."); break; }
	case 3: { msg_print("You laugh out loud."); break; }
	case 4: { msg_print("You make a raspberry while pulling your lower eyelid down."); break; }
	}
	aggravate_monsters(-1);

	
		/* Take a turn */
	p_ptr->energy_use = 100;


}

/* Fire Weapons from Mech */
void do_cmd_mechfire(void)
{
	int dir;
	int counter;
	object_type *o_ptr;
	o_ptr = &inventory[INVEN_MECHA];

	if (!(o_ptr->k_idx)){

		msg_print("You are not riding on a mecha.");
		return;

	}
	if (o_ptr->pval <= 25) {

		msg_print("Your mecha is too damaged.");
		return;

	}

	switch (o_ptr->sval){

	case SV_GUNDAM:
		{
			if (!get_aim_dir(&dir)) return;
			msg_print("You fire your machine gun.");

			for (counter = 0; counter < 4; counter++){
			fire_bolt(GF_METEOR, dir, 15);
			}

			break;

		}
	case SV_EVA:
		{
			if (!get_aim_dir(&dir)) return;
			msg_print("You fire your laser cannon.");
			fire_beam(GF_METEOR, dir, 60);
			break;
		}

	case SV_RAYEARTH:
		{
			if (!get_aim_dir(&dir)) return;
			msg_print("You shoot a Fire Arrow!");
			fire_bolt(GF_FIRE, dir, 60);
			break;
		}

	case SV_SELECE:
		{
			if (!get_aim_dir(&dir)) return;
			msg_print("You shoot a Sapphire Whirlwind!");
			fire_ball(GF_WATER, dir, 60, 5);
			break;
		}

	case SV_WINDAM:
		{
			if (!get_aim_dir(&dir)) return;
			msg_print("You shoot a Emerald Typhoon!");
			fire_ball(GF_SHARD, dir, 60, 10);
			break;
		}
	}

	/* Take a turn */
	p_ptr->energy_use = 100;

	/* Lose some Mecha Life */
	o_ptr->pval -= 25;

	/* Window stuff */
	p_ptr->window |= (PW_PLAYER_0 | PW_PLAYER_1);
}

/* Use a Magic Knight power -- This is just as sad as the student code */
void do_cmd_mknight(void)
{

	int py = p_ptr->py;
	int px = p_ptr->px;
	int i, spell;

	char out_val[160];
	char choice;

	/* Magic Powers */
cptr fire_powers[3] =
{
	"Flame Arrow",
	"Flame Arrow 2",
 	"Ruby Lightning"
};

cptr water_powers[3] =
{
	"Magic Missile",
	"Water Dragon",
	"Sapphire Whirlwind"
 	
};

cptr wind_powers[3] =
{
	"Wind of Protection",
	"Emerald Typhoon",
	"Winds of Healing"
 	
};

cptr metal_powers[3] =
{
	"Electric Bolt",
	"Thunderstorm",
	"Magnetic Tempest"
 	
};

cptr drunk_powers[3] =
{
	"Drunken Flail",
	"Aura of Confusion",
	"Sake Dragon"
 	
};

/* Cost */
int mana_cost[3] = {5, 10, 20};
byte line_attr = TERM_WHITE;

/* Direction */
int dir;

/*	bool verify; */

	bool flag = FALSE; 
	bool redraw = FALSE;
	/* bool okay;*/

	int plev = p_ptr->lev;

/*	const power_type *s_ptr; */

	if (p_ptr->pclass != C_MAGIC_KNIGHT) {
		msg_print("You are not a magic knight!");
		return;

	}

	/* Must not be confused */
	if (p_ptr->confused)
	{
		msg_print("You are too confused!");
		return;
	}

	/* Hack -- Handle stuff */
	handle_stuff();

	/* Save screen */
	screen_save();

	/* Title the list */
	prt("", 1, 20);
	put_str("Name", 1, 25);
	put_str("Mana", 1, 55);

	/* Build a prompt (accept all spells) */
	strnfmt(out_val, 78, "(Powers %c-%c, ESC=exit) Use which Power? ",
	        I2A(0), I2A(2));
	c_prt(line_attr, out_val, 0, 0);
	


	switch (p_ptr->pgroove)
	{
	case G_FIRE:
		{
	
	for (i = 0; i < 3; i++)
	{
		/* Dump the spell --(-- */
		sprintf(out_val, "  %c) %-30s %4d",
		        I2A(i), fire_powers[i], mana_cost[i]);
		c_prt(line_attr, out_val, 1 + i + 1, 20);
	}
	break;

		}

	case G_WATER:
		{
	
	for (i = 0; i < 3; i++)
	{
		/* Dump the spell --(-- */
		sprintf(out_val, "  %c) %-30s %4d",
		        I2A(i), water_powers[i], mana_cost[i]);
		c_prt(line_attr, out_val, 1 + i + 1, 20);
	}
	break;

		}

	case G_WIND:
		{
	
	for (i = 0; i < 3; i++)
	{
		/* Dump the spell --(-- */
		sprintf(out_val, "  %c) %-30s %4d",
		        I2A(i), wind_powers[i], mana_cost[i]);
		c_prt(line_attr, out_val, 1 + i + 1, 20);
	}
	break;

		}

	case G_METAL:
		{
	
	for (i = 0; i < 3; i++)
	{
		/* Dump the spell --(-- */
		sprintf(out_val, "  %c) %-30s %4d",
		        I2A(i), metal_powers[i], mana_cost[i]);
		c_prt(line_attr, out_val, 1 + i + 1, 20);
	}
	break;

		}

	case G_DRUNK:
		{
	
	for (i = 0; i < 3; i++)
	{
		/* Dump the spell --(-- */
		sprintf(out_val, "  %c) %-30s %4d",
		        I2A(i), drunk_powers[i], mana_cost[i]);
		c_prt(line_attr, out_val, 1 + i + 1, 20);
	}
	break;

		}

	}

	while (1)
	{
	

		choice = inkey();
		spell = (islower(choice) ? A2I(choice) : -1);
		if (choice == ESCAPE){
			screen_load();
			return;
		}
		if ((spell >= 0) && (spell < 3)) break;
		else bell("Illegal POWER!");
	}
	
	screen_load();
	/* Verify adequate mana */
	if (mana_cost[spell] > p_ptr->csp)
	{
		/* Warning */
		msg_print("You do not have enough mana to use that power!.");
		
		return;
	}

	if (p_ptr->pgroove == G_FIRE){

		switch (spell){

		case FIRE_FLARE_ARROW:
			{

				if (!get_aim_dir(&dir)) return;
				fire_bolt(GF_FIRE, dir,
				           damroll(1 + (plev  / 2), 6));
					break;
			}
		

		case FIRE_FLARE_ARROW_2:
			{

				if (!get_aim_dir(&dir)) return;
				fire_beam(GF_FIRE, dir,
				           damroll(1 + (plev  / 2), 4));
					break;
			}

		case FIRE_RUBY_LIGHTNING:
			{
				if (!get_aim_dir(&dir)) return;
				fire_ball(GF_FIRE, dir,
				           damroll(1 + (plev  / 2), 6), 6);
					break;
			}
		}
	}

	else if (p_ptr->pgroove == G_WATER){
		switch (spell)
		{

		case WATER_MAGIC_MISSILE:
			{
				if (!get_aim_dir(&dir)) return;
				fire_bolt(GF_WATER, dir,
				           damroll(1 + (plev  / 2), 6));
					break;
			}

		case WATER_WATER_DRAGON:
			{
				if (!get_aim_dir(&dir)) return;
				fire_beam(GF_WATER, dir,
				           damroll(1 + (plev  / 2), 4));
					break;

			}

		case WATER_SAPPHIRE_WHIRLWIND:
			{
				if (!get_aim_dir(&dir)) return;
				fire_ball(GF_WATER, dir,
				           damroll(1 + (plev  / 2), 4),6);
					break;

			}
		}
	}

	else if (p_ptr->pgroove == G_WIND){
		switch (spell)
		{
		
		case WIND_WIND_OF_PROTECTION:
			{
				msg_print("You create a protective aura!");

				(void)set_blessed(p_ptr->blessed + 5 + plev);
					break;

			}

		case WIND_EMERALD_TYPHOON:
			{

					if (!get_aim_dir(&dir)) return;
				fire_ball(GF_FORCE, dir,
				           damroll(1 + (plev  / 2), 6),6);
					break;
			}

		case WIND_WINDS_OF_HEALING:
			{

				(void)hp_player(10 + plev);
				break;


			}
		}
	}

	else if (p_ptr->pgroove == G_METAL)
	{
		switch (spell)
		{

		case METAL_ELECTRIC_BOLT:
			{

				if (!get_aim_dir(&dir)) return;
				fire_bolt(GF_ELEC, dir,
				           damroll(1 + (plev  / 2), 4));
					break;
			}

		case METAL_THUNDERSTORM:
			{
				if (!get_aim_dir(&dir)) return;
				fire_ball(GF_WATER, dir,
				           damroll(1 + (plev  / 2), 4),6);
					break;
			}

		case METAL_MAGNETIC_TEMPEST:
			{
				for (dir = 1; dir < 10; dir++){
				fire_bolt(GF_WATER, dir,
				           damroll(1 + (plev  / 2), 4));
					}
				break;

			}
		}
	}

	else {
		switch (spell)
		{
		case DRUNK_DRUNKEN_FLAIL:
			{
				msg_print("You flail about!");
				fire_ball(GF_MISSILE, 5,
				           damroll(1 + (plev  / 2), 4),1);
					break;
			}
		case DRUNK_AURA_OF_CONFUSION:
		{
			fire_ball(GF_CONFUSION, 5, 0, 10);
			break;


		}
		case DRUNK_SAKE_DRAGON:
			{

				for (dir = 1; dir < 10; dir++){
				fire_beam(GF_CONFUSION, dir,
				           damroll(1 + (plev  / 2), 4));
				
				}

				break;


			}
	}
	}

	/* Take a turn */
	p_ptr->energy_use = 100;

	/* Use mana */
	p_ptr->csp -= mana_cost[spell];

	/* Redraw mana */
	p_ptr->redraw |= (PR_MANA);

	/* Window stuff */
	p_ptr->window |= (PW_PLAYER_0 | PW_PLAYER_1);
	



}



/*
 * Use a magical power  -- believe me, this will definitely be
 * rewritten.
 */
void do_cmd_power(void)
{
	int py = p_ptr->py;
	int px = p_ptr->px;
	int i, spell;

	char out_val[160];
	char choice;

	/* Names of Magical Student Powers */
cptr mag_powers[STUDENT_MAX] =
{
	"Magic Missile",
	"Phase Door",
	"Heroism",
	"Haste",
	"Ouroborous",
	"Terrible Engrish",
	"Teleport",
	"Ki Beam",
	"Ultimate Restoration",
	"Genei Jin",
};

int power_mana[STUDENT_MAX] = {1,5,15,20,25,30,40,50,100,120};
	byte line_attr = TERM_WHITE;

	/* int spell, dir;*/
	int dir;

/*	bool verify; */

	bool flag = FALSE; 
	bool redraw = FALSE;
	/* bool okay;*/

	int plev = p_ptr->lev;

/*	const power_type *s_ptr; */

	


	/* Must be a student */
	if ((cp_ptr->spell_book != TV_STUDENT_BOOK) || (!p_ptr->mag_student))
	{
		msg_print("You are not a magical student!");
		return;
	}

	/* Must not be confused */
	if (p_ptr->confused)
	{
		msg_print("You are too confused!");
		return;
	}

	
	/* Hack -- Handle stuff */
	handle_stuff();

	/* Save screen */
	screen_save();

	/* Title the list */
	prt("", 1, 20);
	put_str("Name", 1, 25);
	put_str("Mana", 1, 55);

	/* Build a prompt (accept all spells) */
	strnfmt(out_val, 78, "(Powers %c-%c, ESC=exit) Use which Power? ",
	        I2A(0), I2A(STUDENT_MAX - 1));
	c_prt(line_attr, out_val, 0, 0);
	/* Dump the spells */
	for (i = 0; i < STUDENT_MAX; i++)
	{
		/* Dump the spell --(-- */
		sprintf(out_val, "  %c) %-30s %4d",
		        I2A(i), mag_powers[i], power_mana[i]);
		c_prt(line_attr, out_val, 1 + i + 1, 20);
	}

	/* Clear the bottom line */
	prt("", 1 + i + 1, 20);
		
	
	/* Choose */
	while (1)
	{
	

		choice = inkey();
		spell = (islower(choice) ? A2I(choice) : -1);
		if (choice == ESCAPE){
			screen_load();
			return;
		}
		if ((spell >= 0) && (spell < STUDENT_MAX)) break;
		else bell("Illegal POWER!");
	}
	
	
	screen_load();
	/* Verify adequate mana */
	if (power_mana[spell] > p_ptr->csp)
	{
		/* Warning */
		msg_print("You do not have enough mana to use that power!.");
		
		return;
	}

	switch (spell)
	{
	case STUDENT_MAGIC_MISSILE: {
		if (!get_aim_dir(&dir)) return;
				fire_bolt(GF_MISSILE, dir,
				           damroll(3 + ((plev - 1) / 5), 4));
			break;
		}

	case STUDENT_PHASE_DOOR:	{
		teleport_player(10);
				break;
			}

	case STUDENT_HASTE:			{

		if (!p_ptr->fast)
				{
					(void)set_fast(randint(20) + plev);
				}

				else
				{
					(void)set_fast(p_ptr->fast + randint(5));
				}

				break;
		
			}

	case STUDENT_HEROISM:		{

				(void)hp_player(10);
				(void)set_hero(p_ptr->hero + randint(25) + 25);
				(void)set_afraid(0);
				break;
			}

	case STUDENT_OUROBOROUS:	{
		(void)set_ouroborous(p_ptr->ouroborous + 10 + plev/5);
			break;
		}

	case STUDENT_POOR_DUBBING:	  {
		msg_print("You start to speak in terrible Engrish!");
		for (dir = 1; dir < 10; dir++){
		(void)fear_monster(dir, plev);
		}
			break;
		}

	case STUDENT_TELEPORT:		{

		teleport_player(plev * 5);
				break;

			}
	case STUDENT_KI_BEAM:		{

			if (!get_aim_dir(&dir)) return;
				fire_beam(GF_MANA, dir,
				          (plev * 3));
					break;
				}

	case STUDENT_RESTORATION:	{
				(void)hp_player(2000);
				(void)set_stun(0);
				(void)set_cut(0);
				(void)do_res_stat(A_STR);
				(void)do_res_stat(A_INT);
				(void)do_res_stat(A_WIS);
				(void)do_res_stat(A_DEX);
				(void)do_res_stat(A_CON);
				(void)do_res_stat(A_CHR);

						break;

					}

				case STUDENT_GENEI_JIN:				{

				(void)set_geneijin(p_ptr->geneijin + 15 + plev/10);
							break;
						}
	}


	

	/* Take a turn */
	p_ptr->energy_use = 100;

	/* Use mana */
	p_ptr->csp -= power_mana[spell];

	/* Redraw mana */
	p_ptr->redraw |= (PR_MANA);

	/* Window stuff */
	p_ptr->window |= (PW_PLAYER_0 | PW_PLAYER_1);
}


