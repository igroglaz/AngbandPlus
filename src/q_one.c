#undef cquest
#define cquest (quest[QUEST_ONE])

bool quest_one_move_hook(char *fmt)
{
	s32b y, x;
	cave_type *c_ptr;

	y = get_next_arg(fmt);
	x = get_next_arg(fmt);
	c_ptr = &cave[y][x];

	if (cquest.status == QUEST_STATUS_UNTAKEN)
	{
		if (quest[QUEST_NECRO].status < QUEST_STATUS_FINISHED) return (FALSE);

		/* The mirror of Galadriel */
		if ((c_ptr->feat != FEAT_SHOP) || (c_ptr->special != 23)) return (FALSE);

		cmsg_print(TERM_YELLOW, "You meet Galadriel, she seems worried.");
		cmsg_print(TERM_YELLOW, "'So it was Sauron that lurked in Dol Guldur...'");
		cmsg_print(TERM_YELLOW, "'The Enemy is growing in power, Morgoth will be unreachable as long'");
		cmsg_print(TERM_YELLOW, "'as his most powerful servant, Sauron, lives. But the power of Sauron'");
		cmsg_print(TERM_YELLOW, "'lies in the One Ring, our only hope is that you find it'");
		cmsg_print(TERM_YELLOW, "'and destroy it. I know it will tempt you, but *NEVER* use it'");
		cmsg_print(TERM_YELLOW, "'or it will corrupt you forever.'");
		cmsg_print(TERM_YELLOW, "'Without the destruction of the ring, Sauron's death can only be temporary'");
		cmsg_print(TERM_YELLOW, "'When you have it bring it to Mount Doom, in Mordor,'");
		cmsg_print(TERM_YELLOW, "'to destroy it in the Great Fire where it was forged.'");
		cmsg_print(TERM_YELLOW, "'I do not know where to find it, seek it through Middle-earth. Maybe there'");
		cmsg_print(TERM_YELLOW, "'are other people that might know'");
		cmsg_print(TERM_YELLOW, "'Do not forget, the Ring must be cast back into the fires or Mount Doom!'");

		/* Continue the plot */
		cquest.status = QUEST_STATUS_TAKEN;
		cquest.init(QUEST_ONE);

		return TRUE;
	}

	return FALSE;
}
bool quest_one_drop_hook(char *fmt)
{
	s32b o_idx;
	object_type *o_ptr;

	o_idx = get_next_arg(fmt);
	o_ptr = &inventory[o_idx];

	if (cquest.status != QUEST_STATUS_TAKEN) return FALSE;

	if (o_ptr->name1 != ART_POWER) return FALSE;
	if (cave[py][px].feat != FEAT_GREAT_FIRE) return FALSE;

	cmsg_print(TERM_YELLOW, "You throw the One Ring in the #RGreat Fire#y, it is rapidly consumed");
	cmsg_print(TERM_YELLOW, "by the searing flames.");
	cmsg_print(TERM_YELLOW, "You feel the powers of evil weakening.");
	cmsg_print(TERM_YELLOW, "Now you can go onto the hunt for Sauron!");

	inven_item_increase(o_idx, -99);
	inven_item_optimize(o_idx);

        abandon_god(GOD_MELKOR);

	/* Continue the plot */
	cquest.status = QUEST_STATUS_FINISHED;
	*(quest[QUEST_ONE].plot) = QUEST_SAURON;
	quest[*(quest[QUEST_ONE].plot)].status = QUEST_STATUS_TAKEN;
	quest[*(quest[QUEST_ONE].plot)].init(*(quest[QUEST_ONE].plot));

	return TRUE;
}
bool quest_one_wield_hook(char *fmt)
{
	s32b o_idx;
	object_type *o_ptr;

	o_idx = get_next_arg(fmt);
	o_ptr = &inventory[o_idx];

	if (cquest.status != QUEST_STATUS_TAKEN) return FALSE;

	if (o_ptr->name1 != ART_POWER) return FALSE;

	/* Flush input */
	flush();

	if (!get_check("You were warned not to wear it, are you sure?")) return TRUE;
	/* Flush input */
	flush();

	if (!get_check("You were warned not to wear it, are you *REALLY* sure?")) return TRUE;

	/* Flush input */
	flush();

	if (!get_check("You were *WARNED* not to wear it, are you *R*E*A*L*L*Y* sure?")) return TRUE;

	cmsg_print(TERM_YELLOW, "As you put it on your finger you feel #Ddark powers #ysapping your soul.");
	cmsg_print(TERM_YELLOW, "The ring firmly binds to your finger!");

        abandon_god(GOD_ERU);
        abandon_god(GOD_MANWE);
        abandon_god(GOD_TULKAS);
        abandon_god(GOD_YAVANNA);

        /*
         * Ok now we are evil, right ?
         * Towns aren't, right ?
         * So let's destroy them !
         */
        town_info[1].destroyed = TRUE;
        town_info[2].destroyed = TRUE;
        town_info[3].destroyed = TRUE;
        town_info[4].destroyed = TRUE;
        town_info[5].destroyed = TRUE;

	/* Continue the plot */
	cquest.status = QUEST_STATUS_FAILED_DONE;
	*(quest[QUEST_ONE].plot) = QUEST_SAURON;
	quest[*(quest[QUEST_ONE].plot)].status = QUEST_STATUS_TAKEN;
	quest[*(quest[QUEST_ONE].plot)].init(*(quest[QUEST_ONE].plot));

	return FALSE;
}
bool quest_one_identify_hook(char *fmt)
{
	s32b item;

	item = get_next_arg(fmt);

	if (cquest.status == QUEST_STATUS_TAKEN)
	{
		object_type *o_ptr;

		/* Inventory */
		if (item >= 0)
		{
			o_ptr = &inventory[item];
		}

		/* Floor */
		else
		{
			o_ptr = &o_list[0 - item];
		}

		if ((o_ptr->name1 == ART_POWER) && (!object_known_p(o_ptr)))
		{
			cmsg_print(TERM_YELLOW, "You finally found the One Ring, source of Sauron power, and key to");
			cmsg_print(TERM_YELLOW, "its destruction. Remember, bring it to Mount Doom and destroy it.");
			cmsg_print(TERM_YELLOW, "And *NEVER* use it.");
		}
	}

	return (FALSE);
}
bool quest_one_death_hook(char *fmt)
{
	s32b r_idx, m_idx;
	bool ok = FALSE;

	m_idx = get_next_arg(fmt);
	r_idx = m_list[m_idx].r_idx;

	if (a_info[ART_POWER].cur_num) return FALSE;

	/* Paranoia */
	if (cquest.status != QUEST_STATUS_TAKEN) return (FALSE);

	if (magik(30) && (r_idx == test_monster_name("Sauron, the Sorcerer")))
	{
		ok = TRUE;
	}
	else if (magik(10) && (r_idx == test_monster_name("Ar-Pharazon the Golden")))
	{
		ok = TRUE;
	}
	else if (magik(10) && (r_idx == test_monster_name("Shelob, Spider of Darkness")))
	{
		ok = TRUE;
	}
	else if (magik(10) && (r_idx == test_monster_name("The Watcher in the Water")))
	{
		ok = TRUE;
	}
	else if (magik(10) && (r_idx == test_monster_name("Glaurung, Father of the Dragons")))
	{
		ok = TRUE;
	}
	else if (magik(10) && (r_idx == test_monster_name("Feagwath, the Undead Sorcerer")))
	{
		ok = TRUE;
	}

	if (ok)
	{
                int i;

		/* Get local object */
		object_type forge, *q_ptr = &forge;

		/* Mega-Hack -- Prepare to make "Grond" */
		object_prep(q_ptr, lookup_kind(TV_RING, SV_RING_POWER));

		/* Mega-Hack -- Mark this item as "the one ring" */
		q_ptr->name1 = ART_POWER;

		/* Mega-Hack -- Actually create "the one ring" */
		apply_magic(q_ptr, -1, TRUE, TRUE, TRUE);

                /* Find a space */
                for (i = 0; i < INVEN_PACK; i++)
		{
			/* Skip non-objects */
                        if (!inventory[i].k_idx) break;
                }
                /* Arg, no space ! */
                if (i == INVEN_PACK)
                {
                        char o_name[200];

                        object_desc(o_name, &inventory[INVEN_PACK - 1], FALSE, 0);

                        /* Drop the item */
                        inven_drop(INVEN_PACK - 1, 99, py, px, FALSE);

                        cmsg_format(TERM_VIOLET, "You feel the urge to drop your %s to make room in your inventory.", o_name);
                }

                /* Carry it */
                cmsg_format(TERM_VIOLET, "You feel the urge to pick up that plain gold ring you see.");
                inven_carry(q_ptr, FALSE);
	}

	return (FALSE);
}
bool quest_one_dump_hook(char *fmt)
{
	if (cquest.status == QUEST_STATUS_FINISHED)
	{
		fprintf(hook_file, "\n You destroyed the One Ring, thus weakening Sauron.");
	}
	if (cquest.status == QUEST_STATUS_FAILED_DONE)
	{
		fprintf(hook_file, "\n You felt under the evil influence of the One Ring and decided to wear it.");
	}
	return (FALSE);
}
bool quest_one_gen_hook(char *fmt)
{
	s32b x, y, try = 10000;

	/* Paranoia */
	if (cquest.status != QUEST_STATUS_TAKEN) return (FALSE);
	if ((dungeon_type != DUNGEON_ANGBAND) || (dun_level != 99)) return (FALSE);

	/* Find a good position */
	while (try)
	{
		/* Get a random spot */
		y = randint(cur_hgt - 4) + 2;
		x = randint(cur_wid - 4) + 2;

		/* Is it a good spot ? */
		if (cave_empty_bold(y, x)) break;

		/* One less try */
		try--;
	}

	if (try) place_monster_one(y, x, test_monster_name("Sauron, the Sorcerer"), 0, FALSE, MSTATUS_ENEMY);

	return (FALSE);
}
bool quest_one_init_hook(int q_idx)
{
	if ((cquest.status >= QUEST_STATUS_TAKEN) && (cquest.status < QUEST_STATUS_FINISHED))
	{
		add_hook(HOOK_LEVEL_END_GEN, quest_one_gen_hook, "one_gen");
		add_hook(HOOK_MONSTER_DEATH, quest_one_death_hook, "one_death");
		add_hook(HOOK_DROP, quest_one_drop_hook, "one_drop");
		add_hook(HOOK_WIELD, quest_one_wield_hook, "one_wield");
		add_hook(HOOK_IDENTIFY, quest_one_identify_hook, "one_id");
	}
	if (cquest.status == QUEST_STATUS_UNTAKEN)
	{
		add_hook(HOOK_MOVE, quest_one_move_hook, "one_move");
	}
	add_hook(HOOK_CHAR_DUMP, quest_one_dump_hook, "one_dump");
	return (FALSE);
}
