#include "angband.h"

static int _get_powers(spell_info* spells, int max)
{
    int ct = 0;

    spell_info* spell = &spells[ct++];

    if (p_ptr->realm1 == REALM_HEX)
    {
        spell->level = 1;
        spell->cost = 0;
        spell->fail = 0;
        spell->fn = hex_stop_spelling_spell;
    }
    else
    {
        spell->level = 25;
        spell->cost = 1;
        spell->fail = calculate_fail_rate(spell->level, 90, p_ptr->stat_ind[A_INT]);
        spell->fn = eat_magic_spell;
    }
    return ct;
}

static void _calc_bonuses(void)
{
    p_ptr->spell_cap += 3;
    p_ptr->to_d_spell += 5 + p_ptr->lev/5;
/*  p_ptr->spell_power += 2; 
    p_ptr->device_power += 2; */
    if (p_ptr->lev >= 25)
        p_ptr->wizard_sight = TRUE;
    if (p_ptr->realm1 == REALM_HEX)
        hex_calc_bonuses();
}
static void _calc_stats(s16b stats[MAX_STATS])
{
    if (p_ptr->realm1 == REALM_HEX)
        hex_calc_stats(stats);
}

static void _calc_weapon_bonuses(obj_ptr obj, plr_attack_info_ptr info)
{
    if (p_ptr->realm1 == REALM_HEX)
        hex_calc_weapon_bonuses(obj, info);
}

static void _get_flags(u32b flgs[OF_ARRAY_SIZE])
{
    add_flag(flgs, OF_SPELL_CAP);
/*  add_flag(flgs, TR_SPELL_POWER);
    add_flag(flgs, TR_MAGIC_MASTERY); */
}

static caster_info * _caster_info(void)
{
    static caster_info me = {0};
    static bool init = FALSE;
    if (!init)
    {
        me.magic_desc = "spell";
        me.which_stat = A_INT;
        me.encumbrance.max_wgt = 430;
        me.encumbrance.weapon_pct = 100;
        me.encumbrance.enc_wgt = 600;
        me.options = CASTER_ALLOW_DEC_MANA | CASTER_GLOVE_ENCUMBRANCE;
        me.realm1_choices = CH_LIFE | CH_SORCERY | CH_NATURE | CH_CHAOS | CH_DEATH |
            CH_TRUMP | CH_ARCANE | CH_ENCHANT | CH_DAEMON | CH_CRUSADE | CH_HEX | CH_ARMAGEDDON;
        init = TRUE;
    }
    return &me;
}

static void _birth(void)
{
    plr_birth_obj_aux(TV_SWORD, SV_DAGGER, 1);
    plr_birth_obj_aux(TV_SOFT_ARMOR, SV_ROBE, 1);
    plr_birth_obj_aux(TV_POTION, SV_POTION_CLARITY, rand_range(10, 20));
    plr_birth_obj_aux(TV_WAND, EFFECT_BOLT_MISSILE, 1);
    plr_birth_spellbooks();
}

static void _timer_on(plr_tim_ptr timer)
{
    if (p_ptr->realm1 != REALM_HEX) return; /* XXX hex should be a separate class */
    if (!hex_spelling_any()) return;
    switch (timer->id)
    {
    case T_CONFUSED:
    case T_PARALYZED:
    case T_STUN:
        stop_hex_spell_all();
        break;
    }
}

plr_class_ptr high_mage_get_class(void)
{
    static plr_class_ptr me = NULL;

    if (!me)
    {           /* dis, dev, sav, stl, srh, fos, thn, thb */
    skills_t bs = { 30,  40,  38,   3,  16,  20,  34,  20};
    skills_t xs = {  7,  15,  11,   0,   0,   0,   6,   7};

        me = plr_class_alloc(CLASS_HIGH_MAGE);
        me->name = "High-Mage";
        me->desc = "High-mages are mages who specialize in one particular field of "
                    "magic and learn it very well - much better than the ordinary mage. "
                    "A high mage's prime statistic is intelligence as this determines "
                    "his spell casting ability.\n \n"
                    "For the price of giving up a second realm of magic, High-mages "
                    "gain substantial benefits in the mana costs, power, minimum levels, and "
                    "failure rates of the spells in their speciality realm. They have "
                    "a class power - 'Eat Magic' - which absorbs mana from wands, "
                    "staves, or rods.\n \n"
                    "And then, only High-Mages are able to cast Hex spells.";

        me->stats[A_STR] = -4;
        me->stats[A_INT] =  4;
        me->stats[A_WIS] =  0;
        me->stats[A_DEX] =  0;
        me->stats[A_CON] = -2;
        me->stats[A_CHR] = -2;
        me->skills = bs;
        me->extra_skills = xs;
        me->life = 94;
        me->base_hp = 0;
        me->exp = 130;
        me->pets = 25;
        me->flags = CLASS_SENSE1_MED | CLASS_SENSE1_WEAK |
                    CLASS_SENSE2_FAST | CLASS_SENSE2_STRONG | CLASS_MAGE_BONUS;
        
        me->hooks.birth = _birth;
        me->hooks.calc_bonuses = _calc_bonuses;
        me->hooks.calc_stats = _calc_stats;
        me->hooks.calc_weapon_bonuses = _calc_weapon_bonuses;
        me->hooks.get_flags = _get_flags;
        me->hooks.caster_info = _caster_info;
        me->hooks.get_powers = _get_powers;
        me->hooks.character_dump = spellbook_character_dump;
        me->hooks.timer_on = _timer_on;
    }
    return me;
}
