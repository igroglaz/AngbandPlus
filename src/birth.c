/* Purpose: create a player character */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies. Other copyrights may also apply.
 */

#include "angband.h"
#include "z-doc.h"


cptr realm_jouhou[VALID_REALM] =
{
"Life magic is very good for healing; it relies mostly on healing, protection and detection spells. Also life magic have a few attack spells as well. It said that some high level spell of life magic can disintegrate Undead monsters into ash.",
"Sorcery is a `meta` realm, including enchantment and general spells. It provides superb protection spells, spells to enhance your odds in combat and, most importantly, a vast selection of spells for gathering information. However, Sorcery has one weakness: it has no spells to deal direct damage to your enemies.",
"Nature magic makes you master of elements; it provides protection, detection, curing and attack spells. Nature also has a spell of Herbal Healing, which is the only powerful healing spell outside the realm of Life magic.",
"There are few types of magic more unpredictable and difficult to control than Chaos magic. Chaos is the very element of unmaking, and the Chaos spells are the most terrible weapons of destruction imaginable. The caster can also call on the primal forces of Chaos to induce mutations in his/her opponents and even him/herself.",
"There is no fouler nor more evil category of spells than the necromantic spells of Death Magic. These spells are relatively hard to learn, but at higher levels the spells give the caster power over living and the (un)dead, but the most powerful spells need his / her own blood as the focus, often hurting the caster in the process of casting.",
"Trump magic has, indeed, an admirable selection of teleportation spells. Since the Trump gateways can also be used to summon other creatures, Trump magic has an equally impressive selection of summoning spells. However, not all monsters appreciate being drawn to another place by Trump user.",
"Arcane magic is a general purpose realm of magic. It attempts to encompass all 'useful' spells from all realms. This is the downside of Arcane magic: while Arcane does have all the necessary 'tool' spells for a dungeon delver, it has no ultra-powerful high level spells. As a consequence, all Arcane spellbooks can be bought in town. It should also be noted that the 'specialized' realms usually offer the same spell at a lower level and cost. ",
"Craft magic can strengthen the caster or the equipments. These spells can greatly improve the caster's fighting ability. Using them against opponents directly is not possible.",
"Demon is a very evil realm, same as Death. It provides various attack spells and devilish detection spells. at higher levels, Demon magic provides ability to dominate demons, and to polymorph yourself into a demon.",
"Crusade is a magic of 'Justice'. It includes damage spells, which are greatly effective against foul and evil monsters, but have poor effects against good monsters.",
"Necromancy allows communication with and ultimately control over the deceased. All direct damage afforded by this realm requires the caster to touch his or her opponent. Any weapons or gloves will obstruct this macabre contact.",
"Armageddon is the most deadly offensive realm. You won't be lacking for firepower here. "
    "However, every spell is an offensive spell, so this realm suffers from a lack of any "
    "utility spells.",
"Music magic shows various effects as sing song. There is two type of song; the one which shows effects instantly and the other one shows effect continuously until SP runs out. But the latter type has a limit; only one song can be sing at the same time.",
"The books of Kendo describe about various combat techniques. When learning new techniques, you are required to carry the books, but once you memorizes them, you don't have to carry them. When using a technique, wielding a weapon is required.",
"Hex is a very terrible realm. Spells gives continual effects when they are spelled continually like songs. Spells may obstract monsters' actions, may deal damages in sight, may revenge against enemies.",
"The books of Rage describe various techniques. To learn a new technique, you must perform a ritual of rage, destroying the book in the process."
    " Once learned, you may use techniques without requiring the corresponding Rage book, but you will need to find many copies of each book in order"
    " to learn all of the techniques.",
"Burglary is the preferred realm of rogues, allowing them to specialize in what they do best: Stealing! "
    "This realm offers good detection and escapes, offers talents for picking pockets and setting traps, "
    "and even allows for direct assassination of sleeping monsters. The books for this realm are only "
    "available in the Black Market (or in the dungeon).",
};



#define AUTOROLLER_STEP 50
#define AUTOROLLER_MAX 50 * 1000
#define AUTOROLLER_DELAY
#define MAX_TRIES 100

void get_max_stats(void)
{
    int        i, j, roll;
    int        dice[6];

    /* Roll and verify some stats */
    while (TRUE)
    {
        /* Roll some dice */
        for (j = i = 0; i < 6; i++)
        {
            /* Roll the dice */
            roll = randint1(7);

            dice[i] = roll;

            /* Collect the maximum */
            j += dice[i];
        }

        /* Verify totals */
        if (j == 24) break;
    }

    /* Acquire the stats */
    for (i = 0; i < 6; i++)
    {
        j = 18 + 60 + dice[i]*10;

        /* Save that value */
        p_ptr->stat_max_max[i] = j;
        if (p_ptr->stat_max[i] > j)
            p_ptr->stat_max[i] = j;
        if (p_ptr->stat_cur[i] > j)
            p_ptr->stat_cur[i] = j;
    }
    p_ptr->knowledge &= ~(KNOW_STAT);

    /* Redisplay the stats later */
    p_ptr->redraw |= (PR_STATS);
}

int _race_exp_factor(void)
{
    if (p_ptr->prace == RACE_DOPPELGANGER)
        return get_race()->exp;
    return get_true_race()->exp;
}
int calc_exp_factor(void)
{
    int exp;
    int r_exp = _race_exp_factor();
    int c_exp = get_class()->exp;
    int a_exp = get_personality()->exp;

    if (p_ptr->prace == RACE_ANDROID)
        return r_exp;

    exp = r_exp * c_exp / 100;
    exp = exp * a_exp / 100;

    if (p_ptr->prace == RACE_MON_DRAGON)
    {
        dragon_realm_ptr realm = dragon_get_realm(p_ptr->dragon_realm);
        exp = exp * realm->exp / 100;
    }

    return exp;
}

static void e_info_reset(void)
{
}

static void k_info_reset(void)
{
    int i;

    /* Reset the "objects" */
    for (i = 1; i < max_k_idx; i++)
    {
        object_kind *k_ptr = &k_info[i];

        k_ptr->tried = FALSE;
        k_ptr->aware = FALSE;
    }
}

/*
 * Clear all the global "character" data
 * XXX Only used by player_birth() 
 */
static void player_wipe(void)
{
    int i;

    plr_wipe();

    /* Start with no artifacts made yet */
    for (i = 0; i < max_a_idx; i++)
    {
        artifact_type *a_ptr = &a_info[i];
        a_ptr->generated = FALSE;
        a_ptr->found = FALSE;
    }

    /* Reset the objects */
    k_info_reset();
    e_info_reset();
    stats_reset();

    /* Reset the "monsters" */
    for (i = 1; i < max_r_idx; i++)
    {
        monster_race *r_ptr = &r_info[i];

        /* Hack -- Reset the counter */
        r_ptr->cur_num = 0;

        /* Hack -- Reset the max counter */
        r_ptr->max_num = 30000;
        r_ptr->flagsx = 0;

        /* Hack -- Reset the max counter */
        if (r_ptr->flags1 & RF1_UNIQUE) r_ptr->max_num = 1;

        /* Hack -- Non-unique Nazguls are semi-unique */
        else if (r_ptr->flags7 & RF7_NAZGUL) r_ptr->max_num = MAX_NAZGUL_NUM;
        else if (i == MON_CAMELOT_KNIGHT) r_ptr->max_num = MAX_CAMELOT_KNIGHT_NUM;

        /* Clear visible kills in this life */
        r_ptr->r_pkills = 0;

        /* Clear all kills in this life */
        r_ptr->r_akills = 0;
        r_ptr->r_skills = 0;
        r_ptr->stolen_ct = 0;

        /* Wipe out pact alliances from previous character
           Currently, flagsr is only set to make the memory field
           work, but perhaps it would be better to set this once
           and for all when a pact is made?  This would break
           my savefiles though ...*/
        r_ptr->flagsr &= ~(RFR_PACT_MONSTER);
        r_ptr->r_flagsr &= ~(RFR_PACT_MONSTER);
    }

    /* Clean the mutation count */
    mutant_regenerate_mod = 100;

    world_player = FALSE;
}

/*
 * Reset turn
 */
/*
 * Hook function for human corpses
 */
bool monster_hook_human(mon_race_ptr r_ptr)
{
    if (r_ptr->flags1 & (RF1_UNIQUE)) return FALSE;
    if (my_strchr("pht", r_ptr->d_char)) return TRUE;
    return FALSE;
}

/*
 * Create a new character.
 *
 * Note that we may be called with "junk" leftover in the various
 * fields, so we must be sure to clear them first.
 */
bool birth_hack = FALSE;
void player_birth(void)
{
    birth_hack = TRUE;
    playtime = 0;

    player_wipe();

    /* Create a new character */
    if (plr_birth() != UI_OK)
        quit(NULL);

    birth_hack = FALSE;
}

