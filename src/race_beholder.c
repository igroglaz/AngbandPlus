#include "angband.h"

static void _birth(void)
{
    p_ptr->current_r_idx = MON_GAZER;
    equip_on_change_race();
    skills_innate_init("Gaze", WEAPON_EXP_BEGINNER, WEAPON_EXP_MASTER);

    plr_birth_food();
    plr_birth_light();
}

static int _rank(void)
{
    int rank = 0;
    if (p_ptr->lev >= 10) rank++;
    if (p_ptr->lev >= 25) rank++;
    if (p_ptr->lev >= 35) rank++;
    if (p_ptr->lev >= 45) rank++;
    return rank;
}

/**********************************************************************
 * Innate Attacks
 **********************************************************************/
static void _calc_innate_bonuses(mon_blow_ptr blow)
{
    int pow = p_ptr->lev + adj_dex_blow[p_ptr->stat_ind[A_INT]];
    if (blow->method != RBM_GAZE) return;
    blow->blows = 100 + MIN(300, 300 * pow / 60);
}
static void _calc_innate_attacks(void)
{
    mon_blow_ptr blow = mon_blow_alloc(RBM_GAZE);
    int l = p_ptr->lev;
    int r = _rank();
    int dd = 2 + r;
    int ds = 3 + r;

    /* Beholder melee is unusual as the attacks are not physical. So, Str and Dex
       do not affect blows, accuracy or damage (cf calc_bonuses in xtra1.c). Rings
       of Combat still work, though, as does Weaponmastery (if you can find it!) */
    blow->power = p_ptr->lev*BTH_PLUS_ADJ;
    mon_blow_push_effect(blow, GF_MISSILE, dice_create(dd, ds, p_ptr->lev/2));
    mon_blow_push_effect(blow, GF_DRAIN_MANA, dice_create(dd, ds, 0))->pct = 50 + l;
    mon_blow_push_effect(blow, GF_OLD_CONF, dice_create(0, 0, l*2))->pct = 40 + l;

    if (p_ptr->lev >= 45)
        mon_blow_push_effect(blow, GF_STASIS, dice_create(dd, ds, 0))->pct = 15 + (l - 45);
    else
        mon_blow_push_effect(blow, GF_OLD_SLEEP, dice_create(0, 0, l*2))->pct = 30 + l;

    mon_blow_push_effect(blow, GF_TURN_ALL, dice_create(0, 0, l))->pct = 15 + l/2;

    if (p_ptr->lev >= 35)
    {
        mon_blow_push_effect(blow, GF_STUN, dice_create(dd/2, ds, 0))->pct = 15 + (l - 35)/3;
        /* XXX no longer works in new spell system
         * mon_blow_push_effect(blow, GF_AMNESIA, dice_create(dd, ds, 0))->pct = 15 + l/2;*/
    }

    _calc_innate_bonuses(blow);
    vec_add(p_ptr->innate_blows, blow);
}

static void _gaze_spell(int cmd, var_ptr res)
{
    switch (cmd)
    {
    case SPELL_NAME:
        var_set_string(res, "Gaze");
        break;
    case SPELL_DESC:
        var_set_string(res, "Gaze at a nearby monster for various effects.");
        break;
    case SPELL_INFO:
        var_set_string(res, info_range(6 + _rank()));
        break;
    case SPELL_COST_EXTRA:
    {
        int costs[5] = {1, 5, 15, 25, 35};
        var_set_int(res, costs[_rank()]);
        break;
    }
    case SPELL_CAST:
    case SPELL_ON_BROWSE: {
        plr_attack_t context = {0};
        int rng = 6 + _rank();
        context.attack_desc = "gaze at";
        if (cmd == SPELL_CAST)
            var_set_bool(res, plr_attack_special_aux(&context, rng));
        else
        {
            plr_attack_display_aux(&context);
            var_set_bool(res, TRUE);
        }
        break; }
    default:
        default_spell(cmd, res);
        break;
    }
}

static void _vision_spell(int cmd, var_ptr res)
{
    switch (cmd)
    {
    case SPELL_NAME:
        var_set_string(res, "Vision");
        break;
    case SPELL_DESC:
        var_set_string(res, "Maps your nearby location.");
        break;
    case SPELL_CAST:
        map_area(DETECT_RAD_MAP);
        var_set_bool(res, TRUE);
        break;
    default:
        default_spell(cmd, res);
        break;
    }
}

/***********************************************************************************
 *                 10           25          35                 45
 * Beholder: Gazer -> Spectator -> Beholder -> Undead Beholder -> Ultimate Beholder
 ***********************************************************************************/
static spell_info _beholder_spells[] = {
    {  1,  1, 30, detect_monsters_spell},
    {  1,  0,  0, _gaze_spell},
    { 15,  7, 30, _vision_spell},
    { 25,  7, 30, drain_mana_spell},
    { -1, -1, -1, NULL}
};
static spell_info _undead_beholder_spells[] = {
    {  1,  1, 30, detect_monsters_spell},
    {  1,  0,  0, _gaze_spell},
    { 15,  7, 30, _vision_spell},
    { 25,  7, 30, drain_mana_spell},
    { 35, 10, 30, animate_dead_spell},
    { 35, 15, 50, mana_bolt_I_spell},
    { 35, 20, 50, brain_smash_spell},
    { 35, 30, 50, summon_undead_spell},
    { -1, -1, -1, NULL}
};
static spell_info _ultimate_beholder_spells[] = {
    {  1,  1, 30, detect_monsters_spell},
    {  1,  0,  0, _gaze_spell},
    { 15,  7, 30, _vision_spell},
    { 25,  7, 30, drain_mana_spell},
    { 35, 20, 50, brain_smash_spell},
    { 45, 30, 50, mana_bolt_II_spell},
    { 45, 40, 60, summon_kin_spell},
    { 45, 40, 60, dispel_magic_spell},
    { 45, 50, 80, darkness_storm_II_spell},
    { -1, -1, -1, NULL}
};
static int _get_spells(spell_info* spells, int max) {
    if (plr_tim_find(T_BLIND))
    {
        msg_print("You can't see!");
        return 0;
    }

    if (p_ptr->lev >= 45)
        return get_spells_aux(spells, max, _ultimate_beholder_spells);
    else if (p_ptr->lev >= 35)
        return get_spells_aux(spells, max, _undead_beholder_spells);
    else
        return get_spells_aux(spells, max, _beholder_spells);
}
static int _get_powers(spell_info* spells, int max)
{
    int ct = 0;

    spell_info* spell = &spells[ct++];
    spell->level = 25;
    spell->cost = 1;
    spell->fail = calculate_fail_rate(spell->level, 90, p_ptr->stat_ind[A_INT]);
    spell->fn = eat_magic_spell;

    return ct;
}
static void _calc_bonuses(void) {
    int l = p_ptr->lev;
    int ac = l/2;

    p_ptr->to_a += ac;
    p_ptr->dis_to_a += ac;

    p_ptr->levitation = TRUE;
    if (p_ptr->lev >= 45)
    {
        p_ptr->to_a += 40;
        p_ptr->dis_to_a += 40;

        p_ptr->telepathy = TRUE;
        res_add(RES_TELEPORT);
        res_add(RES_POIS);
        res_add(RES_CONF);
        p_ptr->free_act++;
        p_ptr->pspeed += 6;
    }
    else if (p_ptr->lev >= 35)
    {
        p_ptr->to_a += 30;
        p_ptr->dis_to_a += 30;

        p_ptr->telepathy = TRUE;
        res_add(RES_TELEPORT);
        res_add(RES_ACID);
        res_add(RES_FIRE);
        res_add(RES_COLD);
        res_add(RES_ELEC);
        res_add(RES_POIS);
        res_add(RES_NETHER);
        res_add(RES_CONF);
        p_ptr->free_act++;
        p_ptr->pspeed += 4;
        p_ptr->hold_life++;
    }
    else if (p_ptr->lev >= 25)
    {
        p_ptr->to_a += 20;
        p_ptr->dis_to_a += 20;

        res_add(RES_TELEPORT);
        res_add(RES_POIS);
        res_add(RES_CONF);
        p_ptr->free_act++;
        p_ptr->pspeed += 2;
    }
    else if (p_ptr->lev >= 10)
    {
        p_ptr->to_a += 10;
        p_ptr->dis_to_a += 10;

        res_add(RES_FEAR);
        res_add(RES_CONF);
        p_ptr->free_act++;
    }
    else
    {
        res_add(RES_POIS);
    }
    if (p_ptr->lev >= 25)
        p_ptr->wizard_sight = TRUE;
}
static void _get_flags(u32b flgs[OF_ARRAY_SIZE]) {
    add_flag(flgs, OF_LEVITATION);
    if (p_ptr->lev >= 45)
    {
        add_flag(flgs, OF_TELEPATHY);
        add_flag(flgs, OF_RES_POIS);
        add_flag(flgs, OF_RES_CONF);
        add_flag(flgs, OF_FREE_ACT);
        add_flag(flgs, OF_SPEED);
    }
    else if (p_ptr->lev >= 35)
    {
        add_flag(flgs, OF_TELEPATHY);
        add_flag(flgs, OF_RES_ACID);
        add_flag(flgs, OF_RES_COLD);
        add_flag(flgs, OF_RES_FIRE);
        add_flag(flgs, OF_RES_ELEC);
        add_flag(flgs, OF_RES_POIS);
        add_flag(flgs, OF_RES_CONF);
        add_flag(flgs, OF_RES_NETHER);
        add_flag(flgs, OF_FREE_ACT);
        add_flag(flgs, OF_SPEED);
        add_flag(flgs, OF_HOLD_LIFE);
    }
    else if (p_ptr->lev >= 25)
    {
        add_flag(flgs, OF_RES_POIS);
        add_flag(flgs, OF_RES_CONF);
        add_flag(flgs, OF_FREE_ACT);
        add_flag(flgs, OF_SPEED);
    }
    else if (p_ptr->lev >= 10)
    {
        add_flag(flgs, OF_RES_FEAR);
        add_flag(flgs, OF_RES_CONF);
        add_flag(flgs, OF_FREE_ACT);
    }
    else
    {
        add_flag(flgs, OF_RES_POIS);
    }
}
static void _gain_level(int new_level) {
    if (p_ptr->current_r_idx == MON_GAZER && new_level >= 10)
    {
        p_ptr->current_r_idx = MON_SPECTATOR;
        msg_print("You have evolved into a Spectator.");
        equip_on_change_race();
        p_ptr->redraw |= PR_MAP | PR_BASIC;
    }
    if (p_ptr->current_r_idx == MON_SPECTATOR && new_level >= 25)
    {
        p_ptr->current_r_idx = MON_BEHOLDER;
        msg_print("You have evolved into a Beholder.");
        equip_on_change_race();
        p_ptr->redraw |= PR_MAP | PR_BASIC;
    }
    if (p_ptr->current_r_idx == MON_BEHOLDER && new_level >= 35)
    {
        p_ptr->current_r_idx = MON_UNDEAD_BEHOLDER;
        msg_print("You have evolved into an Undead Beholder.");
        equip_on_change_race();
        p_ptr->redraw |= PR_MAP | PR_BASIC;
    }
    if (p_ptr->current_r_idx == MON_UNDEAD_BEHOLDER && new_level >= 45)
    {
        p_ptr->current_r_idx = MON_ULTIMATE_BEHOLDER;
        p_ptr->psex = SEX_FEMALE;
        msg_print("You have evolved into an Ultimate Beholder.");
        equip_on_change_race();
        p_ptr->redraw |= PR_MAP | PR_BASIC;
    }
}
static caster_info * _caster_info(void)
{
    static caster_info me = {0};
    static bool init = FALSE;
    if (!init)
    {
        me.magic_desc = "power";
        me.which_stat = A_INT;
        me.encumbrance.max_wgt = 100;
        me.encumbrance.weapon_pct = 100;
        me.encumbrance.enc_wgt = 600;
        me.options = CASTER_ALLOW_DEC_MANA;
        init = TRUE;
    }
    return &me;
}

/**********************************************************************
 * Public
 **********************************************************************/
plr_race_ptr mon_beholder_get_race(void)
{
    static plr_race_ptr me = NULL;
    static cptr   titles[5] =  {"Gazer", "Spectator", "Beholder", "Undead Beholder", "Ultimate Beholder"};
    int           rank = _rank();

    if (!me)
    {           /* dis, dev, sav, stl, srh, fos, thn, thb */
    skills_t bs = { 30,  50,  47,   7,  20,  20,  40,  20};
    skills_t xs = { 10,  20,  15,   1,  20,  20,  16,   7};

        me = plr_race_alloc(RACE_MON_BEHOLDER);
        me->name = "Beholder";
        me->desc = "Beholders are floating orbs of flesh with a single central eye surrounded by "
                    "numerous smaller eyestalks. They attack with their gaze which often confuses or "
                    "even paralyzes their foes. They are unable to wield normal weapons or armor, but "
                    "may equip a single ring on each of their eyestalks, and the number of eyestalks "
                    "increases as the beholder evolves.\n \n"
                    "Beholders are monsters so cannot choose a normal class. Instead, they gain powers "
                    "as they evolve, including some limited offensive capabilities. But the beholders "
                    "primary offense will always be their powerful gaze. Intelligence is the primary "
                    "spell stat and beholders are quite intelligent indeed. Their searching and "
                    "perception are legendary and they are quite capable with magical devices. "
                    "However, they are not very strong and won't be able to stand long against "
                    "multiple foes.\n \n"
                    "The attack of the beholder is unique. Since their gaze is not a normal physical "
                    "attack, they receive no benefit to melee from their Strength and Dexterity. Also, "
                    "the number of attacks is determined by level rather than the normal way: in this "
                    "respect, they resemble monks. Finally, the beholder need not be next to their foes "
                    "in order to attack with melee. They may gaze at distant monsters, though the range "
                    "of their gaze is somewhat restricted.";

        me->skills = bs;
        me->extra_skills = xs;

        me->infra = 8;
        me->exp = 200;
        me->base_hp = 20;
        me->life = 100;
        me->shop_adjust = 140;

        me->hooks.birth = _birth;
        me->hooks.calc_innate_attacks = _calc_innate_attacks;
        me->hooks.calc_innate_bonuses = _calc_innate_bonuses;
        me->hooks.get_spells = _get_spells;
        me->hooks.get_powers = _get_powers;
        me->hooks.caster_info = _caster_info;
        me->hooks.calc_bonuses = _calc_bonuses;
        me->hooks.get_flags = _get_flags;
        me->hooks.gain_level = _gain_level;

        me->flags = RACE_IS_MONSTER;
        me->boss_r_idx = MON_OMARAX;
    }

    if (!birth_hack && !spoiler_hack)
        me->subname = titles[rank];
    me->stats[A_STR] = -3;
    me->stats[A_INT] =  4 + rank;
    me->stats[A_WIS] =  0;
    me->stats[A_DEX] =  1 + rank/2;
    me->stats[A_CON] =  0;
    me->stats[A_CHR] =  0 + rank/2;

    me->pseudo_class_idx = CLASS_MAGE;

    me->equip_template = mon_get_equip_template();

    return me;
}
