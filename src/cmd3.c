/* File: cmd3.c */

/* Purpose: Inventory commands */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies. Other copyrights may also apply.
 */

#include "angband.h"
#include "int_map.h"
#include "z-doc.h"

#include <assert.h>


/*
 * Drop an item
 */
void do_cmd_drop(void)
{
    obj_prompt_t prompt = {0};

    if (p_ptr->special_defense & KATA_MUSOU)
        set_action(ACTION_NONE);

    prompt.prompt = "Drop which item?";
    prompt.error = "You have nothing to drop.";
    prompt.where[0] = INV_PACK;
    prompt.where[1] = INV_EQUIP;
    prompt.where[2] = INV_QUIVER;

    obj_prompt(&prompt);
    if (!prompt.obj) return;

    energy_use = 50;

    switch (prompt.obj->loc.where)
    {
    case INV_PACK:
        pack_drop(prompt.obj);
        break;
    case INV_EQUIP:
        equip_drop(prompt.obj);
        break;
    case INV_QUIVER:
        quiver_drop(prompt.obj);
        break;
    }
}

bool high_level_book(object_type *o_ptr)
{
    if ((o_ptr->tval == TV_LIFE_BOOK) ||
        (o_ptr->tval == TV_SORCERY_BOOK) ||
        (o_ptr->tval == TV_NATURE_BOOK) ||
        (o_ptr->tval == TV_CHAOS_BOOK) ||
        (o_ptr->tval == TV_DEATH_BOOK) ||
        (o_ptr->tval == TV_TRUMP_BOOK) ||
        (o_ptr->tval == TV_CRAFT_BOOK) ||
        (o_ptr->tval == TV_DAEMON_BOOK) ||
        (o_ptr->tval == TV_CRUSADE_BOOK) ||
        (o_ptr->tval == TV_NECROMANCY_BOOK) ||
        (o_ptr->tval == TV_ARMAGEDDON_BOOK) ||
        (o_ptr->tval == TV_MUSIC_BOOK) ||
        (o_ptr->tval == TV_HEX_BOOK))
    {
        if (o_ptr->sval > 1)
            return TRUE;
        else
            return FALSE;
    }

    return FALSE;
}

/*
 * An "item_tester_hook" for refilling lanterns
 */
static bool item_tester_refill_lantern(object_type *o_ptr)
{
    /* Flasks of oil are okay */
    if (o_ptr->tval == TV_FLASK) return (TRUE);

    /* Laterns are okay */
    if ((o_ptr->tval == TV_LITE) &&
        (o_ptr->sval == SV_LITE_LANTERN)) return (TRUE);

    /* Assume not okay */
    return (FALSE);
}

static bool _lite_is_darkness(object_type *lite)
{
    if (lite->name2 == EGO_LITE_DARKNESS || have_flag(lite->flags, OF_DARKNESS))
        return TRUE;
    return FALSE;
}

/*
 * Refill the players lamp (from the pack or floor)
 */
static void do_cmd_refill_lamp(object_type *lantern)
{
    obj_prompt_t prompt = {0};

    prompt.prompt = "Refill with which flask?";
    prompt.error = "You have no flasks of oil.";
    prompt.filter = item_tester_refill_lantern;
    prompt.where[0] = INV_PACK;
    prompt.where[1] = INV_FLOOR;

    obj_prompt(&prompt);
    if (!prompt.obj) return;

    energy_use = 50;
    lantern->xtra4 += prompt.obj->xtra4;
    msg_print("You fuel your lamp.");
    if (_lite_is_darkness(prompt.obj) && lantern->xtra4 > 0)
    {
        lantern->xtra4 = 0;
        msg_print("Your lamp has gone out!");
    }
    else if (_lite_is_darkness(prompt.obj) || _lite_is_darkness(lantern))
    {
        lantern->xtra4 = 0;
        msg_print("Curiously, your lamp doesn't light.");
    }
    else if (lantern->xtra4 >= FUEL_LAMP)
    {
        lantern->xtra4 = FUEL_LAMP;
        msg_print("Your lamp is full.");
    }

    prompt.obj->number--;
    obj_release(prompt.obj, 0);
    p_ptr->update |= PU_TORCH;
}


/*
 * Refuel the players torch (from the pack or floor)
 */
static bool _is_torch(object_type *o_ptr) {
    return object_is_(o_ptr, TV_LITE, SV_LITE_TORCH);
}
static void do_cmd_refill_torch(object_type *torch)
{
    obj_prompt_t prompt = {0};

    prompt.prompt = "Refuel with which torch?";
    prompt.error = "You have no extra torches.";
    prompt.filter = _is_torch;
    prompt.where[0] = INV_PACK;
    prompt.where[1] = INV_FLOOR;

    obj_prompt(&prompt);
    if (!prompt.obj) return;

    energy_use = 50;
    torch->xtra4 += prompt.obj->xtra4 + 5;

    msg_print("You combine the torches.");
    if (_lite_is_darkness(prompt.obj) && torch->xtra4 > 0)
    {
        torch->xtra4 = 0;
        msg_print("Your torch has gone out!");
    }
    else if (_lite_is_darkness(prompt.obj) || _lite_is_darkness(torch))
    {
        torch->xtra4 = 0;
        msg_print("Curiously, your torch does not light.");
    }
    else if (torch->xtra4 >= FUEL_TORCH)
    {
        torch->xtra4 = FUEL_TORCH;
        msg_print("Your torch is fully fueled.");
    }
    else
        msg_print("Your torch glows more brightly.");

    prompt.obj->number--;
    obj_release(prompt.obj, 0);
    p_ptr->update |= PU_TORCH;
}

/*
 * Refill the players lamp, or restock his torches
 */
void do_cmd_refill(void)
{
    int slot = equip_find_obj(TV_LITE, SV_ANY);

    if (slot)
    {
        object_type *o_ptr = equip_obj(slot);

        if (p_ptr->special_defense & KATA_MUSOU)
            set_action(ACTION_NONE);

        switch (o_ptr->sval)
        {
        case SV_LITE_LANTERN:
            do_cmd_refill_lamp(o_ptr);
            break;
        case SV_LITE_TORCH:
            do_cmd_refill_torch(o_ptr);
            break;
        default:
            msg_print("Your light cannot be refilled.");
        }
    }
    else
        msg_print("You are not wielding a light.");
}

/*
 * Target command
 */
void do_cmd_target(void)
{
    /* Target set */
    if (target_set(TARGET_KILL))
    {
        if (target_who > 0)
        {
            monster_type *m_ptr = dun_mon(cave, target_who);
            char          m_name[MAX_NLEN];

            monster_desc(m_name, m_ptr, MD_INDEF_VISIBLE);
            msg_format("Targetting %s.", m_name);
        }
        else
        {
            msg_format("Targetting Position %d, %d.", target_row, target_col);
        }
    }

    /* Target aborted */
    else
    {
        msg_print("Target Aborted.");

    }
}



/*
 * Look command
 */
void do_cmd_look(void)
{
    /* Look around */
    if (target_set(TARGET_LOOK))
    {
        msg_print("Target Selected.");

    }
}



/*
 * Allow the player to examine other sectors on the map
 */
void do_cmd_locate(void)
{
    int     dir, y1, x1, y2, x2;
    char    tmp_val[80];
    char    out_val[160];
    rect_t  map_rect = ui_map_rect();

    /* Start at current panel */
    y2 = y1 = viewport_origin.y;
    x2 = x1 = viewport_origin.x;

    /* Show panels until done */
    while (1)
    {
        point_t pos = point_subtract(point_create(x2, y2), rect_top_left(cave->rect));

        /* Describe the location */
        if (y2 == y1 && x2 == x1)
        {
            tmp_val[0] = '\0';
        }
        else
        {
            sprintf(tmp_val, "%s%s of",
                ((y2 < y1) ? " North" : (y2 > y1) ? " South" : ""),
                ((x2 < x1) ? " West" : (x2 > x1) ? " East" : ""));

        }

        /* Prepare to ask which way to look */
        sprintf(out_val,
            "Map sector [%d(%02d),%d(%02d)], which is%s your sector. Direction?",

            pos.y / (map_rect.cy / 2), pos.y % (map_rect.cy / 2),
            pos.x / (map_rect.cx / 2), pos.x % (map_rect.cx / 2), tmp_val);

        /* Assume no direction */
        dir = 0;

        /* Get a direction */
        while (!dir)
        {
            char command;

            /* Get a command (or Cancel) */
            if (!get_com(out_val, &command, TRUE)) break;
            if (command == '5') break;

            /* Extract the action (if any) */
            dir = get_keymap_dir(command);

            /* Error */
            if (!dir) bell();
        }

        /* No direction */
        if (!dir) break;

        /* Apply the motion */
        if (viewport_scroll(ddy[dir], ddx[dir]))
        {
            y2 = viewport_origin.y;
            x2 = viewport_origin.x;
        }
    }

    viewport_verify();

    p_ptr->update |= PU_MONSTERS;
    p_ptr->redraw |= PR_MAP;
    p_ptr->window |= PW_OVERHEAD | PW_DUNGEON;

    redraw_hack = TRUE;
    handle_stuff();
    redraw_hack = FALSE;
}



/*
 * Sorting hook -- Comp function -- see below
 *
 * We use "u" to point to array of monster indexes,
 * and "v" to select the type of sorting to perform on "u".
 */
bool ang_sort_comp_hook(vptr u, vptr v, int a, int b)
{
    u16b *who = (u16b*)(u);

    u16b *why = (u16b*)(v);

    int w1 = who[a];
    int w2 = who[b];

    int z1, z2;

    /* Sort by player kills */
    if (*why >= 4)
    {
        /* Extract player kills */
        z1 = r_info[w1].r_pkills;
        z2 = r_info[w2].r_pkills;

        /* Compare player kills */
        if (z1 < z2) return (TRUE);
        if (z1 > z2) return (FALSE);
    }


    /* Sort by total kills */
    if (*why >= 3)
    {
        /* Extract total kills */
        z1 = r_info[w1].r_tkills;
        z2 = r_info[w2].r_tkills;

        /* Compare total kills */
        if (z1 < z2) return (TRUE);
        if (z1 > z2) return (FALSE);
    }


    /* Sort by monster level */
    if (*why >= 2)
    {
        /* Extract levels */
        z1 = r_info[w1].level;
        z2 = r_info[w2].level;

        /* Compare levels */
        if (z1 < z2) return (TRUE);
        if (z1 > z2) return (FALSE);
    }


    /* Sort by monster experience */
    if (*why >= 1)
    {
        /* Extract experience */
        z1 = r_info[w1].mexp;
        z2 = r_info[w2].mexp;

        /* Compare experience */
        if (z1 < z2) return (TRUE);
        if (z1 > z2) return (FALSE);
    }


    /* Compare indexes */
    return (w1 <= w2);
}


/*
 * Sorting hook -- Swap function -- see below
 *
 * We use "u" to point to array of monster indexes,
 * and "v" to select the type of sorting to perform.
 */
void ang_sort_swap_hook(vptr u, vptr v, int a, int b)
{
    u16b *who = (u16b*)(u);

    u16b holder;

    /* Unused */
    (void)v;

    /* Swap */
    holder = who[a];
    who[a] = who[b];
    who[b] = holder;
}



/*
 * Identify a character, allow recall of monsters
 *
 * Several "special" responses recall "multiple" monsters:
 *   ^A (all monsters)
 *   ^U (all unique monsters)
 *   ^N (all non-unique monsters)
 *
 * The responses may be sorted in several ways, see below.
 *
 * Note that the player ghosts are ignored. XXX XXX XXX
 */
void do_cmd_query_symbol(void)
{
    int        i, n, r_idx;
    char    sym, query;
    char    buf[128];

    bool    all = FALSE;
    bool    uniq = FALSE;
    bool    norm = FALSE;
    bool    ride = FALSE;
    char    temp[80] = "";

    bool    recall = FALSE;
    doc_ptr doc = NULL;

    u16b    why = 0;
    u16b    *who;

    /* Get a character, or abort */
    if (!get_com("Enter character to be identified(^A:All,^U:Uniqs,^N:Non uniqs,^M:Name): ", &sym, FALSE)) return;

    /* Find that character info, and describe it */
    for (i = 0; ident_info[i]; ++i)
    {
        if (sym == ident_info[i][0]) break;
    }

    /* Describe */
    if (sym == KTRL('A'))
    {
        all = TRUE;
        strcpy(buf, "Full monster list.");
    }
    else if (sym == KTRL('U'))
    {
        all = uniq = TRUE;
        strcpy(buf, "Unique monster list.");
    }
    else if (sym == KTRL('N'))
    {
        all = norm = TRUE;
        strcpy(buf, "Non-unique monster list.");
    }
    else if (sym == KTRL('R'))
    {
        all = ride = TRUE;
        strcpy(buf, "Ridable monster list.");
    }
    /* XTRA HACK WHATSEARCH */
    else if (sym == KTRL('M'))
    {
        all = TRUE;
        if (!get_string("Enter name:",temp, 70))
        {
            temp[0]=0;
            return;
        }
        sprintf(buf, "Monsters with a name \"%s\"",temp);
    }
    else if (ident_info[i])
    {
        sprintf(buf, "%c - %s.", sym, ident_info[i] + 2);
    }
    else
    {
        sprintf(buf, "%c - %s.", sym, "Unknown Symbol");
    }

    /* Display the result */
    prt(buf, 0, 0);

    /* Allocate the "who" array */
    C_MAKE(who, max_r_idx, u16b);

    /* Collect matching monsters */
    for (n = 0, i = 1; i < max_r_idx; i++)
    {
        monster_race *r_ptr = &r_info[i];

        /* Nothing to recall */
        if (!p_ptr->wizard && !r_ptr->r_sights) continue;
        if (r_ptr->flagsx & RFX_SUPPRESS) continue;

        /* Require non-unique monsters if needed */
        if (norm && (r_ptr->flags1 & (RF1_UNIQUE))) continue;

        /* Require unique monsters if needed */
        if (uniq && !(r_ptr->flags1 & (RF1_UNIQUE))) continue;

        /* Require ridable monsters if needed */
        if (ride && !(r_ptr->flags7 & (RF7_RIDING))) continue;

        /* XTRA HACK WHATSEARCH */
        if (temp[0])
        {
          int xx;
          char temp2[80];

          for (xx=0; temp[xx] && xx<80; xx++)
          {
            if (isupper(temp[xx])) temp[xx]=tolower(temp[xx]);
          }

          strcpy(temp2, r_name+r_ptr->name);
          for (xx=0; temp2[xx] && xx<80; xx++)
            if (isupper(temp2[xx])) temp2[xx]=tolower(temp2[xx]);

          if (my_strstr(temp2, temp))
              who[n++]=i;
        }

        /* Collect "appropriate" monsters */
        else if (all || (r_ptr->d_char == sym)) who[n++] = i;
    }

    /* Nothing to recall */
    if (!n)
    {
        /* Free the "who" array */
        C_KILL(who, max_r_idx, u16b);

        return;
    }


    /* Prompt XXX XXX XXX */
    put_str("Recall details? (k/y/n): ", 0, 40);


    /* Query */
    query = inkey();

    /* Restore */
    prt(buf, 0, 0);

    why = 2;

    /* Select the sort method */
    ang_sort_comp = ang_sort_comp_hook;
    ang_sort_swap = ang_sort_swap_hook;

    /* Sort the array */
    ang_sort(who, &why, n);

    /* Sort by kills (and level) */
    if (query == 'k')
    {
        why = 4;
        query = 'y';
    }

    /* Catch "escape" */
    if (query != 'y')
    {
        /* Free the "who" array */
        C_KILL(who, max_r_idx, u16b);

        return;
    }

    /* Sort if needed */
    if (why == 4)
    {
        /* Select the sort method */
        ang_sort_comp = ang_sort_comp_hook;
        ang_sort_swap = ang_sort_swap_hook;

        /* Sort the array */
        ang_sort(who, &why, n);
    }

    doc = doc_alloc(72);

    /* Start at the end */
    i = n - 1;

    /* Scan the monster memory */
    while (1)
    {
        /* Extract a race */
        r_idx = who[i];

        /* Hack -- Auto-recall */
        monster_race_track(r_idx);

        /* Hack -- Handle stuff */
        handle_stuff();

        /* Interact */
        while (1)
        {
            /* Recall */
            if (recall)
            {
                /* Save the screen */
                screen_save();

                /* Recall on screen
                screen_roff(who[i], 0);*/
                doc_clear(doc);
                mon_display_doc(mon_race_lookup(who[i]), doc);
                doc_sync_term(doc, doc_range_all(doc), doc_pos_create(0, 1));
            }

            /* Hack -- Begin the prompt */
            roff_top(r_idx);

            /* Hack -- Complete the prompt */
            Term_addstr(-1, TERM_WHITE, " [(r)ecall, ESC]");

            /* Command */
            query = inkey();

            /* Unrecall */
            if (recall)
            {
                /* Restore*/
                screen_load();
            }

            /* Normal commands */
            if (query != 'r') break;

            /* Toggle recall */
            recall = !recall;
        }

        /* Stop scanning */
        if (query == ESCAPE) break;

        /* Move to "prev" monster */
        if (query == '-')
        {
            if (++i == n)
            {
                i = 0;
                if (!expand_list) break;
            }
        }

        /* Move to "next" monster */
        else
        {
            if (i-- == 0)
            {
                i = n - 1;
                if (!expand_list) break;
            }
        }
    }

    /* Free the "who" array */
    C_KILL(who, max_r_idx, u16b);
    doc_free(doc);

    /* Re-display the identity */
    prt(buf, 0, 0);
}

/* Display a List of Nearby Monsters
   Idea from Vanilla 3.5, but recoded from scratch and enhanced.
   The list is scrollable and displays the current target.
   You can invoke monster recall.
   You can change the current target.
   You can travel to a given monster.
   You can rename your pets.
   You can view info when probing.

   Apologies: MON_LIST_PROBING is a giant, ugly hack, but the old
   probing code was a carpal tunnel death trap in a crowded room.
   
   Apologies: Fuzzy detection complicates the code a bit.*/
struct _mon_list_info_s
{
    int group;
    int subgroup;
    int r_idx;
    int level;
    int m_idx;   /* Allows Targetting for Groups of One */
    int ct_total;
    int ct_awake;
    int ct_los;
    int dx, dy;
    int dis;
    bool target; /* Current target is in this group, in which case, m_idx is the target.
                    Otherwise, m_idx is the monster in this group that is closest to
                    the player. */
    bool fuzzy;
};

typedef struct _mon_list_info_s _mon_list_info_t, *_mon_list_info_ptr;

static _mon_list_info_ptr _mon_list_info_alloc(void)
{
    _mon_list_info_ptr result = malloc(sizeof(_mon_list_info_t));
    memset(result, 0, sizeof(_mon_list_info_t));
    return result;
}

struct _mon_list_s
{
    vec_ptr list;
    int     ct_total;
    int     ct_los;
    int     ct_awake;
    int     ct_level;
};

typedef struct _mon_list_s _mon_list_t, *_mon_list_ptr;

static _mon_list_ptr _mon_list_alloc(void)
{
    _mon_list_ptr result = malloc(sizeof(_mon_list_t));
    result->list = vec_alloc(free);
    result->ct_total = 0;
    result->ct_los = 0;
    result->ct_awake = 0;
    result->ct_level = 0;
    return result;
}

static void _mon_list_free(_mon_list_ptr list)
{
    vec_free(list->list);
    list->list = NULL;
    free(list);
}

#define _GROUP_LOS   1
#define _GROUP_AWARE 2

#define _SUBGROUP_HEADER 1
#define _SUBGROUP_DATA   2
#define _SUBGROUP_FOOTER 3

static int _mon_list_comp(_mon_list_info_ptr left, _mon_list_info_ptr right)
{
    if (left->group < right->group)
        return -1;
    if (left->group > right->group)
        return 1;

    if (left->subgroup < right->subgroup)
        return -1;
    if (left->subgroup > right->subgroup)
        return 1;

    if (left->level > right->level)
        return -1;
    else if (left->level < right->level)
        return 1;

    if (left->dis < right->dis)
        return -1;
    else if (left->dis > right->dis)
        return 1;

    return 0;
}

static _mon_list_ptr _create_monster_list(int mode)
{
    _mon_list_ptr    result = _mon_list_alloc();
    int_map_ptr      map = int_map_alloc(NULL);
    int_map_ptr      los_map = int_map_alloc(NULL);
    int_map_ptr      fuzzy_map = int_map_alloc(NULL);
    int_map_iter_ptr iter;
    int              ct_los_awake = 0, ct_other_awake = 0;

    /* Group all aware monsters by kind. */
    for (iter = int_map_iter_alloc(cave->mon);
            int_map_iter_is_valid(iter);
            int_map_iter_next(iter))
    {
        mon_ptr             mon = int_map_iter_current(iter);
        mon_race_ptr        race = mon_apparent_race(mon);
        int                 key;
        bool                los, fuzzy;
        _mon_list_info_ptr  info_ptr;
        int_map_ptr         which_map = map;

        if (!mon->ml) continue;
        if (plr_tim_find(T_HALLUCINATE))
        {
            /* TODO: Zany stuff in the list?? */
            continue;
        }

        los = point_project(p_ptr->pos, mon->pos);
        fuzzy = BOOL(mon->mflag2 & MFLAG2_FUZZY);

        if (mode == MON_LIST_PROBING) /* No grouping and only display los monsters */
        {
            if (!los) continue;
            fuzzy = FALSE; /* side effect: probing reveals hidden los monsters */
            if (!is_original_ap(mon))
            {
                if (mon->mflag2 & MFLAG2_KAGE)
                    mon->mflag2 &= ~(MFLAG2_KAGE);

                mon->ap_r_idx = mon->r_idx;
                race = mon_race(mon);
                lite_pos(mon->pos);
            }
            lore_do_probe(mon->r_idx);
            key = mon->id;
        }
        else if (fuzzy)
            key = race->d_char;
        else
            key = race->id;

        if (fuzzy) which_map = fuzzy_map;
        else if (los) which_map = los_map;

        info_ptr = int_map_find(which_map, key);
        if (!info_ptr)
        {
            info_ptr = _mon_list_info_alloc();

            info_ptr->group = los ? _GROUP_LOS : _GROUP_AWARE;
            info_ptr->subgroup = _SUBGROUP_DATA;
            info_ptr->r_idx = race->id;
            info_ptr->m_idx = mon->id;
            info_ptr->level = race->level;
            info_ptr->dy = mon->pos.y - p_ptr->pos.y;
            info_ptr->dx = mon->pos.x - p_ptr->pos.x;
            info_ptr->dis = mon->cdis;
            info_ptr->fuzzy = fuzzy;

            int_map_add(which_map, key, info_ptr);
        }

        assert(info_ptr);
        info_ptr->ct_total++;
        result->ct_total++;
        result->ct_level += race->level;

        if (target_who == mon->id)
        {
            info_ptr->m_idx = mon->id;
            info_ptr->dy = mon->pos.y - p_ptr->pos.y;
            info_ptr->dx = mon->pos.x - p_ptr->pos.x;
            info_ptr->dis = mon->cdis;
            info_ptr->target = TRUE;
        }
        else if (mon->cdis < info_ptr->dis && !info_ptr->target)
        {
            info_ptr->m_idx = mon->id;
            info_ptr->dy = mon->pos.y - p_ptr->pos.y;
            info_ptr->dx = mon->pos.x - p_ptr->pos.x;
            info_ptr->dis = mon->cdis;
        }

        if (!mon_tim_find(mon, MT_SLEEP))
        {
            info_ptr->ct_awake++;
            result->ct_awake++;
            if (los)
                ct_los_awake++;
            else
                ct_other_awake++;
        }
        if (los)
        {
            info_ptr->ct_los++;
            result->ct_los++;
        }
    }
    int_map_iter_free(iter);

    /* Insert Dummy Groups if Needed and Sort */
    if (result->ct_los)
    {
        _mon_list_info_ptr info_ptr = _mon_list_info_alloc();
        info_ptr->group = _GROUP_LOS;
        info_ptr->subgroup = _SUBGROUP_HEADER;
        info_ptr->ct_total = result->ct_los;
        info_ptr->ct_los = result->ct_los;
        info_ptr->ct_awake = ct_los_awake;
        vec_add(result->list, info_ptr);

        info_ptr = _mon_list_info_alloc();
        info_ptr->group = _GROUP_LOS;
        info_ptr->subgroup = _SUBGROUP_FOOTER;
        vec_add(result->list, info_ptr);
    }
    if (result->ct_total - result->ct_los)
    {
        _mon_list_info_ptr info_ptr = _mon_list_info_alloc();
        info_ptr->group = _GROUP_AWARE;
        info_ptr->subgroup = _SUBGROUP_HEADER;
        info_ptr->ct_total = result->ct_total - result->ct_los;
        info_ptr->ct_awake = ct_other_awake;
        vec_add(result->list, info_ptr);
    }

    for (iter = int_map_iter_alloc(map);
            int_map_iter_is_valid(iter);
            int_map_iter_next(iter) )
    {
        vec_add(result->list, int_map_iter_current(iter));
    }
    int_map_iter_free(iter);
    for (iter = int_map_iter_alloc(los_map);
            int_map_iter_is_valid(iter);
            int_map_iter_next(iter) )
    {
        vec_add(result->list, int_map_iter_current(iter));
    }
    int_map_iter_free(iter);
    for (iter = int_map_iter_alloc(fuzzy_map);
            int_map_iter_is_valid(iter);
            int_map_iter_next(iter) )
    {
        vec_add(result->list, int_map_iter_current(iter));
    }
    int_map_iter_free(iter);

    vec_sort(result->list, (vec_cmp_f)_mon_list_comp);

    /* Hack: Auto track the first monster on the list
     * if the old track is either missing, or stale. */
    if ( !p_ptr->monster_race_idx
      || !int_map_find(los_map, p_ptr->monster_race_idx) )
    {
        /* If there is a valid target, it should already be tracking,
         * but let's double check */
        if ( target_who > 0
          && int_map_find(los_map, dun_mon(cave, target_who)->r_idx) )
        {
            monster_race_track(dun_mon(cave, target_who)->r_idx);
        }
        else
        {
            int i;
            for (i = 0; i < vec_length(result->list); i++)
            {
                _mon_list_info_ptr info_ptr = vec_get(result->list, i);
                if (info_ptr->subgroup == _SUBGROUP_DATA && !info_ptr->fuzzy)
                {
                    monster_race_track(info_ptr->r_idx);
                    break;
                }
            }
        }
    }

    int_map_free(map);
    int_map_free(los_map);
    int_map_free(fuzzy_map);
    return result;
}

static int _draw_monster_list(_mon_list_ptr list, int top, rect_t rect, int mode)
{
    int     i;
    int     cx_monster;
    char    mon_fmt[50];
    char    mon_fmt_ex[50];

    cx_monster = rect.cx - 14;
    if (cx_monster < 10)
        cx_monster = 10;
    sprintf(mon_fmt, "%%-%d.%ds", cx_monster, cx_monster);
    sprintf(mon_fmt_ex, "%%-%d.%ds ", cx_monster + 10, cx_monster + 10);

    for (i = 0; i < rect.cy; i++)
    {
        int                 idx = top + i;
        _mon_list_info_ptr  info_ptr;

        if (i >= vec_length(list->list)) break;

        Term_erase(rect.x, rect.y + i, rect.cx);

        info_ptr = vec_get(list->list, idx);
        assert(info_ptr);

        if (info_ptr->subgroup == _SUBGROUP_HEADER)
        {
            if (info_ptr->group == _GROUP_LOS)
            {
                c_put_str(TERM_WHITE,
                      format("You %s %d monster%s, %d %s awake:",
                             mode == MON_LIST_PROBING ? "probe" : "see",
                             info_ptr->ct_los,
                             info_ptr->ct_los != 1 ? "s" : "",
                             info_ptr->ct_awake,
                             info_ptr->ct_awake == 1 ? "is" : "are"),
                      rect.y + i, rect.x);
            }
            else if (info_ptr->group == _GROUP_AWARE)
            {
                char buf[200];

                sprintf(buf, "You are aware of %d %smonster%s, %d %s awake:",
                    info_ptr->ct_total,
                    list->ct_los ? "other " : "",
                    info_ptr->ct_total != 1 ? "s" : "",
                    info_ptr->ct_awake,
                    info_ptr->ct_awake == 1 ? "is" : "are"
                );
                if (p_ptr->wizard && list->ct_total)
                {
                    sprintf(buf + strlen(buf), " %d.%02d",
                        list->ct_level/list->ct_total,
                        (list->ct_level*100/list->ct_total)%100
                    );
                }
                c_put_str(TERM_WHITE, buf, rect.y + i, rect.x);
            }
        }
        else if (info_ptr->subgroup == _SUBGROUP_FOOTER)
        {
        }
        else
        {
            const monster_race *r_ptr;
            byte                attr = TERM_WHITE;
            char                buf[100];
            char                loc[100];
            cptr                name;

            assert(info_ptr->r_idx > 0);
            r_ptr = mon_race_lookup(info_ptr->r_idx);

            if (info_ptr->fuzzy)
                attr = TERM_WHITE;
            else if (r_ptr->flags1 & RF1_UNIQUE)
                attr = TERM_VIOLET;
            else if (!info_ptr->ct_awake)
                attr = TERM_L_DARK;

            if (info_ptr->fuzzy)
            {
                name = info_ptr->ct_total > 1 ? mon_race_describe_plural(r_ptr->d_char)
                        : mon_race_describe_singular(r_ptr->d_char);
            }
            else
                name = r_name + r_ptr->name;

            if (info_ptr->ct_total == 1)
            {
                sprintf(buf, "%s", name);
                if ((r_ptr->flags1 & RF1_UNIQUE) && !info_ptr->ct_awake)
                    strcat(buf, " (asleep)");
                if (info_ptr->group == _GROUP_LOS && display_distance)
                {
                    sprintf(loc, "Rng %2d", info_ptr->dis);
                }
                else
                {
                    sprintf(loc, "%c%3d %c%3d",
                            (info_ptr->dy > 0) ? 'S' : 'N', abs(info_ptr->dy),
                            (info_ptr->dx > 0) ? 'E' : 'W', abs(info_ptr->dx));
                }
            }
            else if (!info_ptr->ct_awake)
                sprintf(buf, "%s (%d asleep)", name, info_ptr->ct_total);
            else if (info_ptr->ct_awake == info_ptr->ct_total)
                sprintf(buf, "%s (%d awake)", name, info_ptr->ct_total);
            else
            {
                sprintf(buf, "%s (%d awake, %d asleep)", name,
                    info_ptr->ct_awake, info_ptr->ct_total - info_ptr->ct_awake);
            }

            if (info_ptr->fuzzy)
                Term_queue_char(rect.x + 1, rect.y + i, TERM_WHITE, r_ptr->d_char, 0, 0);
            else
                Term_queue_bigchar(rect.x + 1, rect.y + i, r_ptr->x_attr, r_ptr->x_char, 0, 0);
            if (info_ptr->ct_total == 1)
            {
                c_put_str(attr, format(mon_fmt, buf), rect.y + i, rect.x + 3);
                c_put_str(TERM_WHITE, format("%-9.9s ", loc), rect.y + i, rect.x + 3 + cx_monster + 1);
            }
            else
                c_put_str(attr, format(mon_fmt_ex, buf), rect.y + i, rect.x + 3);

            if (info_ptr->target)
                Term_putch(rect.x, rect.y + i, TERM_L_RED, '*');
            else
                Term_putch(rect.x, rect.y + i, TERM_WHITE, ' ');
        }
    }
    return i;
}

static byte _mon_health_color(monster_type *m_ptr)
{
    int pct = 100 * m_ptr->hp / m_ptr->maxhp;
    if (pct >= 100) return 'G';
    else if (pct >= 60) return 'y';
    else if (pct >= 25) return 'o';
    else if (pct >= 10) return 'R';
    return 'r';
}

static byte _mon_exp_color(monster_type *m_ptr)
{
    return 'y';
}

static void _mon_display_probe(doc_ptr doc, int m_idx)
{
    monster_type *m_ptr = dun_mon(cave, m_idx);
    monster_race *r_ptr = mon_race(m_ptr);
    int           speed;

    speed = m_ptr->mspeed - 110;
    if (m_ptr->nickname)
        doc_printf(doc, "Name : <color:R>%13s</color>\n", quark_str(m_ptr->nickname));
    doc_printf(doc, "Speed: <color:G>%+13d</color>\n", speed);
    doc_printf(doc, "HP   : <color:%c>%6d</color>/<color:G>%6d</color>\n",
        _mon_health_color(m_ptr),
        m_ptr->hp,
        m_ptr->maxhp
    );
    doc_printf(doc, "AC   : <color:G>%13d</color>\n", mon_ac(m_ptr));
    if (m_ptr->mpower != 1000)
    {
        doc_printf(doc, "Power: <color:%c>%10d.%d%%</color>\n",
            m_ptr->mpower > 1000 ? 'r' : 'u', m_ptr->mpower/10, m_ptr->mpower%10);
    }
    if ((r_ptr->flags3 & (RF3_EVIL | RF3_GOOD)) == (RF3_EVIL | RF3_GOOD))
        doc_printf(doc, "Align: <color:B>%13.13s</color>\n", "Good&Evil");
    else if (r_ptr->flags3 & RF3_EVIL)
        doc_printf(doc, "Align: <color:r>%13.13s</color>\n", "Evil");
    else if (r_ptr->flags3 & RF3_GOOD)
        doc_printf(doc, "Align: <color:g>%13.13s</color>\n", "Good");
    else if ((m_ptr->sub_align & (SUB_ALIGN_EVIL | SUB_ALIGN_GOOD)) == (SUB_ALIGN_EVIL | SUB_ALIGN_GOOD))
        doc_printf(doc, "Align: <color:g>%s</color>\n", "neutral(good&evil)");
    else if (m_ptr->sub_align & SUB_ALIGN_EVIL)
        doc_printf(doc, "Align: <color:o>%13.13s</color>\n", "Neutral Evil");
    else if (m_ptr->sub_align & SUB_ALIGN_GOOD)
        doc_printf(doc, "Align: <color:G>%13.13s</color>\n", "Neutral Good");
    else
        doc_printf(doc, "Align: <color:w>%13.13s</color>\n", "Neutral");

    if (r_ptr->next_r_idx)
    {
        doc_printf(doc, "Exp  : <color:%c>%6d</color>/<color:G>%6d</color>\n",
            _mon_exp_color(m_ptr),
            m_ptr->exp,
            r_ptr->next_exp
        );
    }
    if (p_ptr->riding == m_idx)
        doc_printf(doc, "       <color:G>%13.13s</color>\n", "Riding");

    if (is_pet(m_ptr))
        doc_printf(doc, "       <color:G>%13.13s</color>\n", "Pet");
    else if (is_friendly(m_ptr))
        doc_printf(doc, "       <color:G>%13.13s</color>\n", "Friendly");

    doc_insert(doc, "       <indent>");
    mon_tim_probe(m_ptr, doc);
    doc_insert(doc, "</indent>");
}

static void _list_monsters_aux(_mon_list_ptr list, rect_t display_rect, int mode)
{
    int  top = 0, page_size, pos = 1;
    int  ct_types = vec_length(list->list);
    bool done = FALSE;
    bool redraw = TRUE;
    int  cmd_queue[10]; /* A worthy hack!! */
    int  q_pos = 0;
    int  q_ct = 0;

    page_size = display_rect.cy;
    if (page_size > ct_types)
        page_size = ct_types;

    msg_line_clear();
    screen_save();

    while (!done)
    {
        int  cmd;
        bool handled = FALSE;

        if (redraw)
        {
            int ct;
            ct = _draw_monster_list(list, top, display_rect, mode);
            Term_erase(display_rect.x, display_rect.y + ct, display_rect.cx);
            if (mode == MON_LIST_PROBING)
            {
                c_put_str(TERM_L_BLUE, "['P' for Probing; ESC to Exit; ? for Help]",
                        display_rect.y + ct, display_rect.x + 3);
            }
            else
            {
                c_put_str(TERM_L_BLUE, "[Press ESC to exit. Press ? for help]",
                        display_rect.y + ct, display_rect.x + 3);
            }
            redraw = FALSE;
        }
        Term_gotoxy(display_rect.x, display_rect.y + pos);
        {
            int idx = top + pos;
            if (0 <= idx && idx < ct_types)
            {
                _mon_list_info_ptr info_ptr = vec_get(list->list, idx);
                assert(info_ptr);
                if (info_ptr->r_idx && info_ptr->r_idx != p_ptr->monster_race_idx)
                {
                    monster_race_track(info_ptr->r_idx);
                    window_stuff();
                }
            }
        }
        if (q_pos < q_ct)
            cmd = cmd_queue[q_pos++];
        else
            cmd = inkey_special(TRUE);

        switch (cmd)
        {
        /* Monster Recall */
        case '/': case 'R':
        {
            int idx = top + pos;
            if (0 <= idx && idx < ct_types)
            {
                _mon_list_info_ptr info_ptr = vec_get(list->list, idx);
                assert(info_ptr);
                if (info_ptr->r_idx && !info_ptr->fuzzy)
                {
                    mon_race_ptr race = mon_race_lookup(info_ptr->r_idx);
                    mon_display(race);
                    screen_load();
                    screen_save();
                    redraw = TRUE;
                    handled = TRUE;
                }
            }
            break;
        }
        /* Probe Monster Info */
        case 'P':
        {
            if (mode == MON_LIST_PROBING)
            {
                int idx = top + pos;
                if (0 <= idx && idx < ct_types)
                {
                    _mon_list_info_ptr info_ptr = vec_get(list->list, idx);
                    assert(info_ptr);
                    if (info_ptr->m_idx)
                    {
                        doc_ptr   doc = doc_alloc(display_rect.cx);
                        doc_pos_t loc = doc_pos_create(display_rect.x, display_rect.y + pos + 1);

                        doc_insert(doc, "    <indent>");
                        _mon_display_probe(doc, info_ptr->m_idx);
                        doc_insert(doc, "</indent>\n<color:b>[</color><color:B>Up for Prev; Down for Next; Any Key to Exit</color><color:b>]</color>\n");

                        if (doc_cursor(doc).y + loc.y >= Term->hgt)
                            loc.y = Term->hgt - doc_cursor(doc).y;

                        doc_sync_term(doc, doc_range_all(doc), loc);

                        /* Get a key, but don't waste it! Use arrows to travel up
                           and down and display probe info for next/prev monster */
                        cmd = inkey_special(TRUE);
                        if (cmd == '2' || cmd == SKEY_DOWN)
                        {
                            cmd_queue[0] = '2';
                            cmd_queue[1] = 'p';
                            q_ct = 2;
                            q_pos = 0;
                        }
                        else if (cmd == '8' || cmd == SKEY_UP)
                        {
                            cmd_queue[0] = '8';
                            cmd_queue[1] = 'p';
                            q_ct = 2;
                            q_pos = 0;
                        }
                        else if (cmd == 'n' || cmd == 'N')
                        {
                            cmd_queue[0] = 'n';
                            cmd_queue[1] = 'p';
                            q_ct = 2;
                            q_pos = 0;
                        }

                        doc_free(doc);
                        screen_load();
                        screen_save();
                        redraw = TRUE;
                    }
                }
                handled = TRUE;
            }
            break;
        }
        /* Rename Pet */
        case 'N':
        {
            int idx = top + pos;
            if (0 <= idx && idx < ct_types)
            {
                _mon_list_info_ptr info_ptr = vec_get(list->list, idx);
                assert(info_ptr);
                if (info_ptr->m_idx && !info_ptr->fuzzy)
                {
                    monster_type *m_ptr = dun_mon(cave, info_ptr->m_idx);
                    if (is_pet(m_ptr))
                    {
                        char out_val[20];

                        if (m_ptr->nickname)
                            strcpy(out_val, quark_str(m_ptr->nickname));
                        else
                            strcpy(out_val, "");

                        prt("Name : ", display_rect.y + pos + 1, display_rect.x);
                        if (askfor(out_val, 15))
                        {
                            if (out_val[0])
                                m_ptr->nickname = quark_add(out_val);
                            else
                                m_ptr->nickname = 0;
                        }
                        screen_load();
                        screen_save();
                        redraw = TRUE;
                        handled = TRUE;
                    }
                }
            }
            break;
        }
        /* Set Current Target */
        case '*': case '5': case 'T':
        {
            int idx = top + pos;
            if (0 <= idx && idx < ct_types)
            {
                _mon_list_info_ptr info_ptr = vec_get(list->list, idx);
                assert(info_ptr);
                if ( info_ptr->r_idx
                  && !info_ptr->target
                  && target_able(info_ptr->m_idx) )
                {
                    health_track(info_ptr->m_idx);
                    target_who = info_ptr->m_idx;
                    target_row = dun_mon(cave, info_ptr->m_idx)->pos.y;
                    target_col = dun_mon(cave, info_ptr->m_idx)->pos.x;
                    p_ptr->redraw |= PR_HEALTH_BARS;
                    p_ptr->window |= PW_MONSTER_LIST;
                    done = TRUE; /* Building a better target command :) */
                    handled = TRUE;
                }
            }
            break;
        }
        /* Travel to Location */
        case '`':
        {
            int idx = top + pos;
            if (0 <= idx && idx < ct_types)
            {
                _mon_list_info_ptr info_ptr = vec_get(list->list, idx);
                assert(info_ptr);
                if (info_ptr->m_idx)
                {
                    travel_begin(TRAVEL_MODE_NORMAL, dun_mon(cave, info_ptr->m_idx)->pos);
                    done = TRUE;
                    handled = TRUE;
                }
            }
            break;
        }
        /* Navigate the List */
        case '7': case SKEY_TOP:
            top = 0;
            pos = 0;
            redraw = TRUE;
            handled = TRUE;
            break;
        case '1': case SKEY_BOTTOM:
            top = MAX(0, ct_types - page_size);
            pos = 0;
            redraw = TRUE;
            handled = TRUE;
            break;
        case '9': case SKEY_PGUP:
            top -= page_size;
            if (top < 0)
            {
                top = 0;
                pos = 0;
            }
            redraw = TRUE;
            handled = TRUE;
            break;
        case '3': case SKEY_PGDOWN:
            top += page_size;
            if (top > ct_types - page_size)
            {
                top = MAX(0, ct_types - page_size);
                pos = 0;
            }
            redraw = TRUE;
            handled = TRUE;
            break;
        case '2': case SKEY_DOWN:
            if (top + pos < ct_types - 1)
                pos++;

            if (pos == page_size)
            {
                pos--;
                top++;
                redraw = TRUE;
            }
            handled = TRUE;
            break;
        case '8': case SKEY_UP:
            if (pos > 0)
                pos--;

            if (pos == 0 && top > 0)
            {
                top--;
                redraw = TRUE;
            }
            handled = TRUE;
            break;
        /* Help */
        case '?':
            doc_display_help("context_monster_list.txt", NULL);
            screen_load();
            screen_save();
            redraw = TRUE;
            handled = TRUE;
            break;
        /* Exit */
        case ESCAPE:
        case 'Q':
        case '\n':
        case '\r':
        case '[':
            done = TRUE;
            handled = TRUE;
            break;
        }

        /* Goto next text match */
        if (!handled && islower(cmd))
        {
            int  search = cmd;
            int  i;

            for (i = pos + 1; i != pos; )
            {
                int idx = top + i;
                _mon_list_info_ptr info_ptr = NULL;

                if (idx >= ct_types)
                {
                    i = 0;
                    continue;
                }

                info_ptr = vec_get(list->list, idx);
                assert(info_ptr);
                if (info_ptr->m_idx)
                {
                    monster_type *m_ptr = dun_mon(cave, info_ptr->m_idx);
                    monster_race *r_ptr = mon_race(m_ptr);
                    cptr          name = r_name + r_ptr->name;
                    int           c;

                    if (strstr(name, "The ") == name)
                        name += 4;

                    c = name[0];
                    if (isalpha(c))
                        c = tolower(c);

                    if (c == search)
                    {
                        pos = i;
                        handled = TRUE;
                        break;
                    }
                }
                i++;
                if (i >= page_size)
                    i = 0;
            }
        }

        /* Exit on unkown key? */
        if (!handled && quick_messages && mode != MON_LIST_PROBING) /* Hey, it cost mana to get here! */
            done = TRUE;
    }
    screen_load();
}

void do_cmd_list_monsters(int mode)
{
    _mon_list_ptr list = _create_monster_list(mode);
    rect_t        display_rect = ui_menu_rect();

    if (display_rect.cx > 50)
        display_rect.cx = 50;

    if (list->ct_total)
        _list_monsters_aux(list, display_rect, mode);
    else
        msg_print("You see no visible monsters.");

    _mon_list_free(list);
}

void _fix_monster_list_aux(void)
{
    _mon_list_ptr list = _create_monster_list(MON_LIST_NORMAL);
    rect_t        display_rect = {0};
    int           ct = 0, i;

    Term_get_size(&display_rect.cx, &display_rect.cy);

    if (list->ct_total)
        ct = _draw_monster_list(list, 0, display_rect, MON_LIST_NORMAL);

    for (i = ct; i < display_rect.cy; i++)
        Term_erase(display_rect.x, display_rect.y + i, display_rect.cx);

    _mon_list_free(list);
}

void fix_monster_list(void)
{
    int j;
    for (j = 0; j < 8; j++)
    {
        term *old = Term;

        if (!angband_term[j]) continue;
        if (!(window_flag[j] & PW_MONSTER_LIST)) continue;

        Term_activate(angband_term[j]);

        _fix_monster_list_aux();

        Term_fresh();
        Term_activate(old);
    }
}

/* Display a List of Nearby Objects
   Idea from Vanilla 3.5, but recoded from scratch ... */

struct _obj_list_info_s
{
    int group;
    int subgroup;
    int idx;
    int x,  y;
    int dx, dy;
    int score;
    int count;
};
typedef struct _obj_list_info_s _obj_list_info_t, *_obj_list_info_ptr;

static _obj_list_info_ptr _obj_list_info_alloc(void)
{
    _obj_list_info_ptr result = malloc(sizeof(_obj_list_info_t));
    memset(result, 0, sizeof(_obj_list_info_t));
    return result;
}

struct _obj_list_s
{
    vec_ptr list;
    int     ct_autopick;
    int     ct_total;
    int     ct_feature;
};

typedef struct _obj_list_s _obj_list_t, *_obj_list_ptr;

static _obj_list_ptr _obj_list_alloc(void)
{
    _obj_list_ptr result = malloc(sizeof(_obj_list_t));
    result->list = vec_alloc(free);
    result->ct_autopick = 0;
    result->ct_total = 0;
    result->ct_feature = 0;
    return result;
}

static void _obj_list_free(_obj_list_ptr list)
{
    vec_free(list->list);
    list->list = NULL;
    free(list);
}

#define _GROUP_FEATURE  1
#define _GROUP_AUTOPICK 2
#define _GROUP_OTHER    3

static int _obj_list_comp(_obj_list_info_ptr left, _obj_list_info_ptr right)
{
    if (left->group < right->group)
        return -1;
    if (left->group > right->group)
        return 1;

    if (left->subgroup < right->subgroup)
        return -1;
    if (left->subgroup > right->subgroup)
        return 1;

    if (left->score > right->score)
        return -1;
    if (left->score < right->score)
        return 1;

    if (left->subgroup == _SUBGROUP_DATA) /* Assert: Left and Right are same group/subgroup */
    {
        if (left->group == _GROUP_FEATURE)
        {
            if (left->idx < right->idx)
                return -1;
            if (left->idx > right->idx)
                return 1;
        }
        else
        {
            object_type *left_obj = dun_obj(cave, left->idx);
            object_type *right_obj = dun_obj(cave, right->idx);

            if (left_obj->tval < right_obj->tval)
                return -1;
            if (left_obj->tval > right_obj->tval)
                return 1;
        }
    }

    return 0;
}

static _obj_list_ptr _obj_list = NULL;
static bool _interesting(int feat_id)
{
    feature_type *f_ptr = &f_info[feat_id];
    if (have_flag(f_ptr->flags, FF_RECALL)) return TRUE;
    if (have_flag(f_ptr->flags, FF_STORE)) return TRUE;
    if (have_flag(f_ptr->flags, FF_TRAVEL)) return TRUE;
    if (cave->dun_type_id == D_SURFACE)
    {
        if (have_flag(f_ptr->flags, FF_STAIRS))
            return TRUE;
    }
    return FALSE;
}
static void _obj_list_feature(point_t pos, dun_grid_ptr grid)
{
    if (!(grid->info & CAVE_MARK)) return;
    if (_interesting(grid->feat))
    {
        _obj_list_info_ptr info = _obj_list_info_alloc();
        info->group = _GROUP_FEATURE;
        info->subgroup = _SUBGROUP_DATA;
        info->idx = grid->feat;
        info->x = pos.x;
        info->y = pos.y;
        info->dy = info->y - p_ptr->pos.y;
        info->dx = info->x - p_ptr->pos.x;

        vec_add(_obj_list->list, info);
        _obj_list->ct_feature++;
    }
}
static void _obj_list_obj(point_t pos, obj_ptr obj)
{
    _obj_list_info_ptr info;
    int                auto_pick_idx;

    if (!obj->k_idx) return;
    if (!(obj->marked & OM_FOUND)) return;
    if (obj->tval == TV_GOLD) return;

    info = _obj_list_info_alloc();
    info->subgroup = _SUBGROUP_DATA;
    info->idx = obj->loc.v.floor.obj_id;
    info->x = pos.x;
    info->y = pos.y;
    info->dy = info->y - p_ptr->pos.y;
    info->dx = info->x - p_ptr->pos.x;
    info->score = obj_value(obj);
    info->count = obj->number;

    auto_pick_idx = is_autopick(obj);
    if ( auto_pick_idx >= 0
      && (autopick_list[auto_pick_idx].action & (DO_AUTOPICK | DO_QUERY_AUTOPICK)) )
    {
        info->group = _GROUP_AUTOPICK;
        _obj_list->ct_autopick += obj->number;
    }
    else
        info->group = _GROUP_OTHER;

    vec_add(_obj_list->list, info);
    _obj_list->ct_total += obj->number;
}
static void _obj_list_pile(point_t pos, obj_ptr pile)
{
    obj_ptr obj;
    for (obj = pile; obj; obj = obj->next)
        _obj_list_obj(pos, obj);
}
static _obj_list_ptr _create_obj_list(void)
{
    _obj_list = _obj_list_alloc();;
    dun_iter_interior(cave, _obj_list_feature);
    dun_iter_floor_obj(cave, _obj_list_pile);

    /* Add Headings and Sort */
    if (_obj_list->ct_autopick)
    {
        _obj_list_info_ptr info = _obj_list_info_alloc();
        info->group = _GROUP_AUTOPICK;
        info->subgroup = _SUBGROUP_HEADER;
        info->count = _obj_list->ct_autopick;
        vec_add(_obj_list->list, info);

        info = _obj_list_info_alloc();
        info->group = _GROUP_AUTOPICK;
        info->subgroup = _SUBGROUP_FOOTER;
        vec_add(_obj_list->list, info);
    }
    if (_obj_list->ct_feature)
    {
        _obj_list_info_ptr info = _obj_list_info_alloc();
        info->group = _GROUP_FEATURE;
        info->subgroup = _SUBGROUP_FOOTER;
        vec_add(_obj_list->list, info);
    }
    if (_obj_list->ct_total - _obj_list->ct_autopick)
    {
        _obj_list_info_ptr info = _obj_list_info_alloc();
        info->group = _GROUP_OTHER;
        info->subgroup = _SUBGROUP_HEADER;
        info->count = _obj_list->ct_total - _obj_list->ct_autopick;
        vec_add(_obj_list->list, info);
    }
    if (_obj_list->ct_feature)
    {
        _obj_list_info_ptr info = _obj_list_info_alloc();
        info->group = _GROUP_FEATURE;
        info->subgroup = _SUBGROUP_HEADER;
        info->count = _obj_list->ct_feature;
        vec_add(_obj_list->list, info);
    }
    vec_sort(_obj_list->list, (vec_cmp_f)_obj_list_comp);

    return _obj_list;
}

static int _draw_obj_list(_obj_list_ptr list, int top, rect_t rect)
{
    int     i;
    int     cx_obj;
    char    obj_fmt[50];

    cx_obj = rect.cx - 14;
    if (cx_obj < 10)
        cx_obj = 10;
    sprintf(obj_fmt, "%%-%d.%ds", cx_obj, cx_obj);

    for (i = 0; i < rect.cy; i++)
    {
        int                 idx = top + i;
        _obj_list_info_ptr  info_ptr;

        if (i >= vec_length(list->list)) break;

        Term_erase(rect.x, rect.y + i, rect.cx);

        info_ptr = vec_get(list->list, idx);
        assert(info_ptr);

        if (info_ptr->subgroup == _SUBGROUP_HEADER)
        {
            if (info_ptr->group == _GROUP_FEATURE)
            {
                c_put_str(TERM_WHITE,
                      format("There %s %d interesting feature%s:",
                             info_ptr->count != 1 ? "are" : "is",
                             info_ptr->count,
                             info_ptr->count != 1 ? "s" : ""),
                      rect.y + i, rect.x);
            }
            else if (info_ptr->group == _GROUP_AUTOPICK)
            {
                c_put_str(TERM_WHITE,
                      format("There %s %d wanted object%s:",
                             info_ptr->count != 1 ? "are" : "is",
                             info_ptr->count,
                             info_ptr->count != 1 ? "s" : ""),
                      rect.y + i, rect.x);
            }
            else
            {
                c_put_str(TERM_WHITE,
                      format("There %s %d %sobject%s:",
                             info_ptr->count != 1 ? "are" : "is",
                             info_ptr->count,
                             list->ct_autopick ? "other " : "",
                             info_ptr->count != 1 ? "s" : ""),
                      rect.y + i, rect.x);
            }
        }
        else if (info_ptr->subgroup == _SUBGROUP_FOOTER)
        {
        }
        else if (info_ptr->group == _GROUP_FEATURE)
        {
            feature_type *f_ptr = &f_info[info_ptr->idx];
            char          loc[100];
            char          name[255];

            sprintf(loc, "%c%3d %c%3d",
                    (info_ptr->dy > 0) ? 'S' : 'N', abs(info_ptr->dy),
                    (info_ptr->dx > 0) ? 'E' : 'W', abs(info_ptr->dx));

            sprintf(name, "%s", f_name + f_ptr->name);
            if (have_flag(f_ptr->flags, FF_QUEST_ENTER))
            {
                int quest_id = cave_at_xy(info_ptr->x, info_ptr->y)->special;
                sprintf(name + strlen(name), ": %s", quests_get_name(quest_id));
            }
            Term_queue_bigchar(rect.x + 1, rect.y + i, f_ptr->x_attr[F_LIT_STANDARD], f_ptr->x_char[F_LIT_STANDARD], 0, 0);
            c_put_str(use_graphics ? f_ptr->d_attr[F_LIT_STANDARD] : f_ptr->x_attr[F_LIT_STANDARD],
                format(obj_fmt, name), rect.y + i, rect.x + 3);
            c_put_str(TERM_WHITE, format("%-9.9s ", loc), rect.y + i, rect.x + 3 + cx_obj + 1);
        }
        else
        {
            object_type *o_ptr = dun_obj(cave, info_ptr->idx);
            char         o_name[MAX_NLEN];
            char         loc[100];
            byte         attr = tv_color(o_ptr->tval);
            byte         a = object_attr(o_ptr);
            char         c = object_char(o_ptr);

            object_desc(o_name, o_ptr, 0);
            sprintf(loc, "%c%3d %c%3d",
                    (info_ptr->dy > 0) ? 'S' : 'N', abs(info_ptr->dy),
                    (info_ptr->dx > 0) ? 'E' : 'W', abs(info_ptr->dx));
            Term_queue_bigchar(rect.x + 1, rect.y + i, a, c, 0, 0);
            c_put_str(attr, format(obj_fmt, o_name), rect.y + i, rect.x + 3);
            if (p_ptr->wizard)
                c_put_str(TERM_WHITE, format("%6d %6d ", info_ptr->score, obj_value_real(o_ptr)), rect.y + i, rect.x + 3 + cx_obj + 1);
            else
                c_put_str(TERM_WHITE, format("%-9.9s ", loc), rect.y + i, rect.x + 3 + cx_obj + 1);
        }
    }
    return i;
}

void do_cmd_list_objects(void)
{
    _obj_list_ptr list = _create_obj_list();
    rect_t        display_rect = ui_menu_rect();

    if (display_rect.cx > 50)
        display_rect.cx = 50;

    if (list->ct_total + list->ct_feature)
    {
        int  top = 0, page_size, pos = 1;
        int  ct_types = vec_length(list->list);
        bool done = FALSE;
        bool redraw = TRUE;

        page_size = display_rect.cy;
        if (page_size > ct_types)
            page_size = ct_types;

        msg_line_clear();
        screen_save();

        while (!done)
        {
            int cmd;

            if (redraw)
            {
                int ct;
                ct = _draw_obj_list(list, top, display_rect);
                Term_erase(display_rect.x, display_rect.y + ct, display_rect.cx);
                c_put_str(TERM_L_BLUE, "[Press ESC to exit. Press ? for help]",
                        display_rect.y + ct, display_rect.x + 3);
                redraw = FALSE;
            }
            Term_gotoxy(display_rect.x, display_rect.y + pos);

            cmd = inkey_special(TRUE);
            switch (cmd)
            {
            case '?':
                doc_display_help("context_object_list.txt", NULL);
                screen_load();
                screen_save();
                redraw = TRUE;
                break;
            case ESCAPE:
            case 'Q':
            case '\n':
            case '\r':
            case ']':
                done = TRUE;
                break;
            case '/':
            case 'I':
            {
                int idx = top + pos;
                if (0 <= idx && idx < ct_types)
                {
                    _obj_list_info_ptr info_ptr = vec_get(list->list, idx);
                    assert(info_ptr);
                    if (info_ptr->idx && info_ptr->group != _GROUP_FEATURE)
                    {
                        object_type *o_ptr = dun_obj(cave, info_ptr->idx);
                        if (object_is_weapon_armour_ammo(o_ptr) || obj_is_known(o_ptr))
                        {
                            obj_display(o_ptr);
                            screen_load();
                            screen_save();
                            redraw = TRUE;
                        }
                    }
                }
                break;
            }
            case '`':
            {
                int idx = top + pos;
                if (0 <= idx && idx < ct_types)
                {
                    _obj_list_info_ptr info_ptr = vec_get(list->list, idx);
                    assert(info_ptr);
                    if (info_ptr->idx)
                    {
                        travel_begin(TRAVEL_MODE_NORMAL, point_create(info_ptr->x, info_ptr->y));
                        done = TRUE;
                    }
                }
                break;
            }
            case SKEY_TOP:
            case '7':
                top = 0;
                pos = 0;
                redraw = TRUE;
                break;
            case SKEY_BOTTOM:
            case '1':
                top = MAX(0, ct_types - page_size);
                pos = 0;
                redraw = TRUE;
                break;
            case SKEY_PGUP:
            case '9':
                top -= page_size;
                if (top < 0)
                {
                    top = 0;
                    pos = 0;
                }
                redraw = TRUE;
                break;
            case SKEY_PGDOWN:
            case '3':
                top += page_size;
                if (top > ct_types - page_size)
                {
                    top = MAX(0, ct_types - page_size);
                    pos = 0;
                }
                redraw = TRUE;
                break;
            case SKEY_DOWN:
            case '2':
                if (top + pos < ct_types - 1)
                    pos++;

                if (pos == page_size)
                {
                    pos--;
                    top++;
                    redraw = TRUE;
                }
                break;
            case SKEY_UP:
            case '8':
                if (pos > 0)
                    pos--;

                if (pos == 0 && top > 0)
                {
                    top--;
                    redraw = TRUE;
                }
                break;

            default: /* Attempt to locate next element in list beginning with pressed key */
            {
                bool found = FALSE;
                if (islower(cmd))
                {
                    int search = cmd;
                    int i = pos + 1;
                    for (i = pos + 1; i != pos; )
                    {
                        int idx = top + i;
                        _obj_list_info_ptr info_ptr = NULL;

                        if (idx >= ct_types)
                        {
                            i = 0;
                            continue;
                        }

                        info_ptr = vec_get(list->list, idx);
                        assert(info_ptr);
                        if (info_ptr->idx)
                        {
                            if (info_ptr->group == _GROUP_FEATURE)
                            {
                                feature_type *f_ptr = &f_info[info_ptr->idx];
                                cptr          name = f_name + f_ptr->name;
                                int           c = name[0];

                                if (isalpha(c))
                                    c = tolower(c);

                                if (c == search)
                                {
                                    pos = i;
                                    found = TRUE;
                                    break;
                                }
                            }
                            else
                            {
                                object_type *o_ptr = dun_obj(cave, info_ptr->idx);
                                char         name[MAX_NLEN];
                                char         c;

                                object_desc(name, o_ptr, OD_NAME_ONLY | OD_OMIT_PREFIX | OD_OMIT_INSCRIPTION | OD_NO_FLAVOR | OD_NO_PLURAL);
                                c = name[0];
                                if (isalpha(c))
                                    c = tolower(c);

                                if (c == search)
                                {
                                    pos = i;
                                    found = TRUE;
                                    break;
                                }
                            }
                        }
                        i++;
                        if (i >= page_size)
                            i = 0;
                    }
                }

                if (!found && quick_messages)
                    done = TRUE;
            }}
        }
        screen_load();
    }
    else
        msg_print("You see no objects.");

    _obj_list_free(list);
}

void _fix_object_list_aux(void)
{
    _obj_list_ptr list = _create_obj_list();
    rect_t        display_rect = {0};
    int           ct = 0, i;

    Term_get_size(&display_rect.cx, &display_rect.cy);

    if (list->ct_total + list->ct_feature)
        ct = _draw_obj_list(list, 0, display_rect);

    for (i = ct; i < display_rect.cy; i++)
        Term_erase(display_rect.x, display_rect.y + i, display_rect.cx);

    _obj_list_free(list);
}

void fix_object_list(void)
{
    int j;
    for (j = 0; j < 8; j++)
    {
        term *old = Term;

        if (!angband_term[j]) continue;
        if (!(window_flag[j] & PW_OBJECT_LIST)) continue;

        Term_activate(angband_term[j]);

        _fix_object_list_aux();

        Term_fresh();
        Term_activate(old);
    }
}
