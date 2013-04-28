#include "angband.h"

void _charge_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Charge");
		break;
	case SPELL_DESC:
		var_set_string(res, "Attacks monster with your weapons normaly, then move through counter side of the monster.");
		break;
	case SPELL_CAST:
	{
		int dir, x, y;
		var_set_bool(res, FALSE);
		if (p_ptr->riding)
		{
			msg_print("You cannot do it when riding.");
			return;
		}

		if (!get_rep_dir2(&dir)) return;
		if (dir == 5) return;

		y = py + ddy[dir];
		x = px + ddx[dir];

		if (!cave[y][x].m_idx)
		{
			msg_print("There is no monster there.");
			return;
		}

		py_attack(y, x, 0);

		if (player_can_enter(cave[y][x].feat, 0) && !is_trap(cave[y][x].feat))
		{
			y += ddy[dir];
			x += ddx[dir];
			if (player_can_enter(cave[y][x].feat, 0) && !is_trap(cave[y][x].feat) && !cave[y][x].m_idx)
			{
				msg_print(NULL);
				move_player_effect(y, x, MPE_FORGET_FLOW | MPE_HANDLE_STUFF | MPE_DONT_PICKUP);
			}
		}
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void _smash_trap_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Smash Trap");
		break;
	case SPELL_DESC:
		var_set_string(res, "Sets off a trap, then destroy that trap.");
		break;
	case SPELL_CAST:
	{
		int dir, x, y;
		var_set_bool(res, FALSE);
		
		if (!get_rep_dir2(&dir)) return;
		y = py + ddy[dir];
		x = px + ddx[dir];
		move_player(dir, easy_disarm, TRUE);
		
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

/****************************************************************
 * Spell Table and Exports
 ****************************************************************/

static spell_info _spells[] = 
{
   /*lvl cst fail  spell */
    {  8,  5,  40, detect_menace_spell},
	{ 15, 20,   0, _charge_spell},
	{ 20, 15,   0, _smash_trap_spell},
	{ 25, 20,  60, earthquake_spell},
	{ 30, 80,  75, massacre_spell},
	{ -1, -1, -1, NULL}
};

static int _get_spells(spell_info* spells, int max)
{
	int ct = get_spells_aux(spells, max, _spells);
	if (ct == 0)
		msg_print("Rargh! Go kill something for more experience!");
	return ct;
}

static void _calc_bonuses(void)
{
	/* Berserkers are immune to fear (cf fear.c) */
	p_ptr->shero = 1;
	p_ptr->sustain_str = TRUE;
	p_ptr->sustain_dex = TRUE;
	p_ptr->sustain_con = TRUE;
	p_ptr->regenerate = TRUE;
	p_ptr->free_act = TRUE;
	p_ptr->pspeed += 2;
	if (p_ptr->lev >= 30) p_ptr->pspeed++;
	if (p_ptr->lev >= 40) p_ptr->pspeed++;
	if (p_ptr->lev >= 45) p_ptr->pspeed++;
	if (p_ptr->lev >= 50) p_ptr->pspeed++;
	p_ptr->to_a += 10+p_ptr->lev/2;
	p_ptr->dis_to_a += 10+p_ptr->lev/2;
	p_ptr->skill_dig += 100 + p_ptr->lev*8;
	if (p_ptr->lev > 39) p_ptr->reflect = TRUE;
	if (p_ptr->lev > 34) p_ptr->no_stun = TRUE;
	p_ptr->redraw |= PR_STATUS;
}

static void _get_flags(u32b flgs[TR_FLAG_SIZE])
{
	add_flag(flgs, TR_SUST_STR);
	add_flag(flgs, TR_SUST_DEX);
	add_flag(flgs, TR_SUST_CON);
	add_flag(flgs, TR_REGEN);
	add_flag(flgs, TR_FREE_ACT);
	add_flag(flgs, TR_SPEED);
	add_flag(flgs, TR_RES_FEAR);
	if (p_ptr->lev >= 40) add_flag(flgs, TR_REFLECT);
}

static void _get_immunities(u32b flgs[TR_FLAG_SIZE])
{
	add_flag(flgs, TR_RES_FEAR);
}

static void _calc_weapon_bonuses(object_type *o_ptr, weapon_info_t *info_ptr)
{
	int to_h = p_ptr->lev/5;
	int to_d = p_ptr->lev/6;

	info_ptr->to_h += to_h;
	info_ptr->to_d += to_d;
	info_ptr->dis_to_h += to_h;
	info_ptr->dis_to_d += to_d;
	if (info_ptr->wield_how == WIELD_TWO_HANDS && !info_ptr->omoi)
	{
		info_ptr->to_h += to_h;
		info_ptr->to_d += to_d;
		info_ptr->dis_to_h += to_h;
		info_ptr->dis_to_d += to_d;
	}

	info_ptr->xtra_blow += p_ptr->lev/23;
}

static int _get_powers(spell_info* spells, int max)
{
	int ct = 0;

	spell_info* spell = &spells[ct++];
	spell->level = 10;
	spell->cost = 10;
	spell->fail = calculate_fail_rate(spell->level, 70, p_ptr->stat_ind[A_DEX]);
	spell->fn = recall_spell;

	return ct;
}

static caster_info * _caster_info(void)
{
	static caster_info me = {0};
	static bool init = FALSE;
	if (!init)
	{
		me.magic_desc = "brutal power";
		me.which_stat = A_STR;
		me.options = CASTER_USE_HP;
		me.weight = 1000;
		init = TRUE;
	}
	return &me;
}

class_t *berserker_get_class_t(void)
{
	static class_t me = {0};
	static bool init = FALSE;

	if (!init)
	{            /* dis,   dev,  sav,  stl,   srh,  fos, thn,   thb */
	skills_t bs = {-100, -1000, -200, -100,  -100, -100, 120, -2000};
	skills_t xs = {   0,     0,    0,    0,     0,    0,  50,     0};

		me.name = "Berserker";
		me.desc = "A Berserker is a fearful fighter indeed, immune to fear and "
					"paralysis. At high levels, Berserkers can reflect bolt spells "
					"with their tough flesh. Furthermore, they can fight without "
					"weapons, can remove cursed equipment by force, and can even use "
					"their special combat techniques when surrounded by an anti-magic "
					"barrier. Berserkers, however, cannot use any magical devices or "
					"read any scrolls, and are hopeless at all non-combat skills. "
					"Since Berserker Spectres are quite easy to *win* with, their "
					"scores are lowered.\n \n"
					"Berserkers use a unique system of techniques called 'Rage'. They "
					"gain more techniques as they gain levels. They use no magic: "
					"indeed, they cannot use any magic devices or activate any "
					"artifacts, and cannot read scrolls. To offset these great "
					"disadvantages, Berserkers are allowed to have an important class "
					"power - 'Recall'.";

		me.stats[A_STR] =   8;
		me.stats[A_INT] = -20;
		me.stats[A_WIS] = -20;
		me.stats[A_DEX] =   4;
		me.stats[A_CON] =   4;
		me.stats[A_CHR] = -20;
		me.base_skills = bs;
		me.extra_skills = xs;
		me.life = 200;
		me.exp = 160;
		me.pets = 255;
		
		me.calc_bonuses = _calc_bonuses;
		me.get_flags = _get_flags;
		me.get_immunities = _get_immunities;
		me.calc_weapon_bonuses = _calc_weapon_bonuses;
		me.get_powers = _get_powers;
		me.get_spells = _get_spells;
		me.caster_info = _caster_info;
		init = TRUE;
	}

	return &me;
}
