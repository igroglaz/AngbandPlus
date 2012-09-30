#undef cquest
#define cquest (quest[QUEST_NARSIL])

bool quest_narsil_move_hook(int loc)
{
        int y = loc >> 8, x = loc & 0xFF;
        cave_type *c_ptr = &cave[y][x];
        int i;
        object_type *o_ptr;

        if (cquest.status != QUEST_STATUS_TAKEN) return FALSE;

        /* The castle of Aragorn */
        if ((c_ptr->feat != FEAT_SHOP) || (c_ptr->special != 14)) return FALSE;

        /* Look out for Narsil */
        for (i = 0; i < INVEN_TOTAL; i++)
        {
                o_ptr = &inventory[i];

                if (!o_ptr->k_idx) continue;

                if (o_ptr->name1 == ART_NARSIL) break;
        }

        if (i == INVEN_TOTAL) return FALSE;

        cmsg_print(TERM_YELLOW, "I heard that the broken sword had been found!");
        cmsg_print(TERM_YELLOW, "I thought it was only a rumor... until now.");
        cmsg_print(TERM_YELLOW, "What you have is really the sword that was broken.");
        cmsg_print(TERM_YELLOW, "I will get it reforged...");
        cmsg_print(TERM_L_BLUE, "Aragorn leaves for a few hours then comes back...");
        cmsg_print(TERM_YELLOW, "Here it is, Anduril the sword that was forged and is");
        cmsg_print(TERM_YELLOW, "reforged again, take it, you sure need it for you quest.");

        o_ptr->name1 = ART_ANDURIL;
        apply_magic(o_ptr, -1, TRUE, TRUE, TRUE);
        inven_item_describe(i);
        inven_item_optimize(i);

	/* Window stuff */
        p_ptr->window |= (PW_EQUIP | PW_PLAYER | PW_INVEN);

        /* Continue the plot */
        cquest.status = QUEST_STATUS_FINISHED;

        del_hook(HOOK_MOVE, quest_narsil_move_hook);
        process_hooks_restart = TRUE;

        return TRUE;
}
bool quest_narsil_dump_hook(int q_idx)
{
        if (cquest.status >= QUEST_STATUS_COMPLETED)
        {
                fprintf(hook_file, "\n The sword that was broken is now reforged.");
        }
        return (FALSE);
}
bool quest_narsil_identify_hook(int item)
{
        if (cquest.status == QUEST_STATUS_UNTAKEN)
        {
                int i;
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

                if (o_ptr->name1 == ART_NARSIL)
                {
                        cquest.status = QUEST_STATUS_TAKEN;

                        for (i = 0; i < 5; i++)
                        {
                                if (quest[QUEST_NARSIL].desc[i] != NULL)
                                {
                                        cmsg_print(TERM_YELLOW, quest[QUEST_NARSIL].desc[i]);
                                }
                        }

                        add_hook(HOOK_MOVE, quest_narsil_move_hook, "narsil_move");
                        del_hook(HOOK_IDENTIFY, quest_narsil_identify_hook);
                        process_hooks_restart = TRUE;
                }
        }

        return (FALSE);
}
bool quest_narsil_init_hook(int q_idx)
{
        if ((cquest.status >= QUEST_STATUS_TAKEN) && (cquest.status < QUEST_STATUS_FINISHED))
        {
                add_hook(HOOK_MOVE, quest_narsil_move_hook, "narsil_move");
        }
        if (cquest.status == QUEST_STATUS_UNTAKEN) add_hook(HOOK_IDENTIFY, quest_narsil_identify_hook, "narsil_id");
        add_hook(HOOK_CHAR_DUMP, quest_narsil_dump_hook, "narsil_dump");
        return (FALSE);
}
