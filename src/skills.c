
/* File: skills.c */

/*
 * Sangband skills.  Cost of skills, increase, restore, and reduce skills.
 * Realms, Oaths, and specializations, effect of skills on each other, the
 * skills improvement screen.
 *
 * Copyright (c) 2002
 * Leon Marrick, Julian Lighton, Michael Gorse, Chris Petit
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"


/*
 * Special limits on skills.  A "warrior" is a character that has taken the
 * Oath of Iron.  A "spellcaster" is a magic-using character that has taken
 * the Oath associated with his realm.  A "half-spellcaster" is a magic-user
 * that hasn't taken such an Oath.
 *
 * Note:  Holy warriors can raise their blunt weapon skill to the max.
 */
#define WARRIOR_DEVICE_LIMIT         50  /* Warriors are bad with devices */
#define WARRIOR_SPELLCASTING_LIMIT    0  /* Warriors can't cast spells */

#define NON_WARRIOR_LIMIT            80  /* Only Warriors are great in melee */
#define SPELLCASTER_WEAPON_LIMIT     50  /* Spellcasters aren't good in melee */
#define PIOUS_EDGED_WEAPON_LIMIT     30  /* Pious chars don't like swords */
#define PRIEST_BLUNT_WEAPON_LIMIT    75  /* Priests are OK with blunt weapons */

#define OATH_OF_IRON_REQ             45  /* Skill required to take the Oath of Iron */
#define NON_GUILDBROTHERS_LIMIT      30  /* Burglary limit for non-Guild */


static bool cannot_learn_magic;
static bool cannot_learn_prayers;

/*
 * Variable - currently selected skill
 */
int selected = -1;


/*
 * This function is used to get effective values for all skills.
 *
 * We use min and max to set bounds.  A non-zero min will shift the possible
 * range of outputs.  A non-standard difference between min and max
 * will stretch or shrink it.
 *
 * We use average character power when asked to calculate S_NOSKILL.
 *
 * Non-linear effective values are handled as special cases (see
 * especially "calc_bonuses()".
 */
s16b get_skill(int skill, int min, int max)
{
	int tmp, std_max;


	/* Illegal or empty skill */
	if ((skill < 0) || (skill >= NUM_SKILLS)) return (0);

	/* Verify input maximum */
	if (max <= min) return (min);


	/* Get the current skill percentage */
	tmp = p_ptr->pskills[skill].cur;

	/* Get the standard maximum */
	std_max = PY_MAX_POWER;


	/* If difference between the maximum and the minimum is non-standard, */
	if ((max - min) != std_max)
	{
		/* Stretch or shrink the range of possible outputs to fit (do not randomize). */
		tmp = (tmp * (max - min) + (std_max / 2)) / std_max;
	}

	/* Adjust the output value by the minimum */
	if (min)
	{
		tmp += min;
	}

	/* Return */
	return (tmp);
}


/*
 * Adjust skill effect depending on racial aptitude.
 *
 * See usage in "calc_bonuses()".
 */
s16b get_skill_race(int skill, int min, int max)
{
	int range = max - min;
	int factor;


	/* Some skills are strongly affected by racial aptitude */
	if (skill == S_SWORD || skill == S_HAFTED || skill == S_POLEARM ||
	    skill == S_CROSSBOW || skill == S_BOW || skill == S_SLING ||
	    skill == S_THROWING || skill == S_WRESTLING || skill == S_KARATE)
	{
		/* 1-point change in cost means ~8% change in base ability */
		factor = 2;
	}

	/* Some other skills are affected less by racial aptitude */
	else if (skill == S_DEVICE || skill == S_PERCEPTION || skill == S_STEALTH ||
	         skill == S_DISARM)
	{
		/* 1-point change in cost means ~5% change in base ability */
		factor = 10;
	}

	/* All other skills are not affected at all */
	else
	{
		factor = -999;
	}


	/* Make adjustments, depending on race */
	if ((range > 0) && (factor > -10))
	{
		/* Get racial ability to learn this skill (plus factor) */
		int cost     = factor + race_adj_cost_skill[skill][p_ptr->prace];
		int cost_ave = factor + 10;

		/* The greater the cost to learn, the less effective the skill */
		range = ((range * cost_ave) + (cost / 2)) / (cost);

		/* Adjust maximum */
		max = min + range;
	}

	/* Call the standard function */
	return (get_skill(skill, min, max));
}



/*
 * Determine which melee weapon skill we're using.
 */
int sweapon(int tval)
{
	/* Check for a known weapon */
	if (tval == TV_SWORD) return (S_SWORD);
	if (tval == TV_HAFTED) return (S_HAFTED);
	if ((tval == TV_POLEARM) || (tval == TV_DIGGING)) return (S_POLEARM);

	/* Otherwise, we are using either wrestling or karate */
	return (p_ptr->barehand);
}

/*
 * Determine which archery skill we're using.
 */
int sbow(int sval)
{
	/* Check for a known missile weapon */
	if (is_sling(sval))    return (S_SLING);
	if (is_bow(sval))      return (S_BOW);
	if (is_crossbow(sval)) return (S_CROSSBOW);

	/* Otherwise, assume none */
	return (S_NOSKILL);
}



/*
 * Description of magic realm (if any)
 */
static cptr realm_desc(void)
{
	switch (p_ptr->realm)
	{
		case MAGE:   return ("You know the sorcerous arts.");
		case PRIEST: return ("You serve a greater power.");
		case DRUID:  return ("You control the forces of nature.");
		case NECRO:  return ("You understand the forces of life and death.");
		default:
		{
			if (p_ptr->oath & (OATH_OF_IRON))
				return ("You are a specialist warrior.");
			else if (p_ptr->oath & (BURGLARS_GUILD))
				return ("You are a specialist burglar.");
			else
				return ("You are not familiar with any magical art.");
		}
	}
}

/*
 * Describe a magic-user
 */
static cptr realm_user_desc(void)
{
	switch (p_ptr->realm)
	{
		case MAGE:   return ("wizard");
		case PRIEST: return ("priest");    /* Could stand to be changed */
		case DRUID:  return ("druid");
		case NECRO:  return ("necromancer");
		default:
		{
			if (p_ptr->oath & (BURGLARS_GUILD)) return ("burglar");
			return ("warrior");
		}
	}
}


/*
 * Print something on the message line
 */
static void skill_msg(int attr, const char *msg)
{
	/* Clear part of line, position cursor */
	(void)Term_erase(0, 0, 55);
	(void)Term_gotoxy(2, 0);

	/* Display colored text */
	(void)Term_addstr(-1, attr, msg);
}


/*
 * Clear the message area of the skills screen
 */
static void erase_skill_comment(void)
{
	clear_row(18);
	clear_row(19);
	clear_row(20);
	clear_row(21);
}


/*
 * Print comments or warning about a skill
 */
static void skill_comment(int attr, const char *msg)
{
	/* Erase message */
	erase_skill_comment();

	/* Move the cursor, indent message */
	move_cursor(18, 7);

	/* Print new message */
	c_roff(attr, msg, 2, 78);
}


/*
 * Determine if this skill increase made a new talent available.
 */
static int check_for_new_talent(int skill, int level)
{
	int i;

	/* Scan the list of talents */
	for (i = 0; i < NUM_TALENTS; i++)
	{
		/* Talent uses this skill */
		if (talent_info[i].skill == skill)
		{
			/* Talent has just become available */
			if (talent_info[i].min_level == level)
			{
				/* Talent can in fact be used (or at least browsed) */
				if (can_use_talent(i) > -1) return (i);
			}
		}
	}

	return (-1);
}



/*
 * Calculate character power.
 *
 * The experience needed to attain the current level in each skill is
 * calculated.  Racial cost adjustments are ignored here, as is unspent
 * experience.  The total is then matched against the master experience
 * table, and the highest power level that does not require more
 * experience than this total is accepted.
 *
 * Power is capped at 100.
 */
void calc_power(void)
{
	int i;

	/* Get total experience to attain current skill levels */
	s32b total = calc_spent_exp();

	/*
	 * Scan the experience table, and get the highest level that does not
	 * cost more than the accumulated exp.
	 */
	for (i = 1; i < PY_MAX_POWER; i++)
	{
		/* Stop when exp cost is too high for us to meet */
		if (player_exp[i+1] > total) break;
	}

	/* Save the power (ranges from 1 to 100) */
	p_ptr->power = i;
}


/*
 * Calculate a character's power for exp-gaining purposes.  To avoid
 * abuse, we use total exp, not just spent exp.
 */
int calc_exp_power(void)
{
	int i;
	s32b tot_exp = calc_spent_exp() + p_ptr->exp;

	/*
	 * Scan the experience table, and get the highest level that does not
	 * cost more than the accumulated exp.
	 */
	for (i = 1; i < PY_MAX_POWER; i++)
	{
		/* Stop when exp cost is too high for us to meet */
		if (player_exp[i+1] > tot_exp) break;
	}

	/* Use experience level or actual character power, whichever is higher */
	return (MAX(p_ptr->power, i));
}


/*
 * Calculate the practice penalty for a skill.
 *
 * This function is designed to encourage players not to instantly raise
 * their skills from zero to high levels, without forcing them to do
 * anything, or getting too harsh.
 *
 * Note that intrinsic skill cost or racial adjustments are ignored.  A
 * given amount of "practice_exp" means the same thing for all skills.
 *
 * In lieu of a real solution to the problem of some skills being harder
 * to gain experience in than others, we treat burglary and holy magic
 * more gently.
 */
static void practice_penalty(int skill, s32b *cost)
{
	int cur_level = p_ptr->pskills[skill].cur;
	int practice_level = 0;
	s32b exp = p_ptr->pskills[skill].practice_exp;

	int mult, deficit;
	s32b penalty;

	int i;
	int leeway;


	/* Check skills */
	switch (skill)
	{
		/* These skills have practice requirements */
		case S_SWORD:
		case S_HAFTED:
		case S_POLEARM:
		case S_CROSSBOW:
		case S_BOW:
		case S_SLING:
		case S_THROWING:
		case S_WRESTLING:
		case S_KARATE:
		case S_DEVICE:
		case S_DISARM:
		{
			leeway = 4;
			break;
		}

		case S_BURGLARY:
		{
			leeway = 9;
			break;
		}
		case S_MAGIC:
		{
			if (p_ptr->realm == PRIEST) leeway = 6;
			else                        leeway = 4;
			break;
		}

		/* Other skills don't */
		default:
		{
			return;
		}
	}


	/*
	 * Scan the experience table, and get the highest level that does not
	 * cost more than the accumulated exp.
	 */
	for (i = 0; i <= PY_MAX_POWER; i++)
	{
		/* Stop when exp cost is too high for us to meet */
		if (player_exp[i] > exp) break;

		/* Remember this level */
		practice_level = i;
	}

	/* Get the "practice deficit".  Allow some leeway. */
	deficit = cur_level - (practice_level + leeway);

	/* No (significant) deficit, no penalty. */
	if (deficit <= 0) return;

	/* Use deficit to determine multiplier (base is 100) */
	mult = 20 * deficit;

	/* Use multiplier to determine penalty */
	penalty = *cost * mult / 100L;

	/* Penalty cannot exceed 3000 exp */
	if (penalty > 3000L) penalty = 3000L;

	/* Apply the penalty */
	*cost += penalty;
}


/*
 * Calculate the experience point cost to raise the given skill one
 * percentage point.
 *
 * Cost depends on level of skill, inherent difficulty of skill, and
 * racial suitability.
 *
 * Cost can be raised (sometimes by quite a lot) because the character
 * power is much higher then the skill level, or because the skill is
 * poorly practiced.  The point of these two adjustments is to avoid
 * high-level characters from instantly buying up new skills on the cheap,
 * while still allowing the player to do almost anything he's prepared to
 * pay for.
 *
 * This function handles errors by returning -1.
 */
s32b adv_cost(int skill, bool add_practice_cost)
{
	s32b tmp, cost, divisor;

	int level = p_ptr->pskills[skill].cur;

	/* Paranoia - skill must be less than 100 */
	if (level >= PY_MAX_POWER) return (-1);

	/* Get the difference in exp needed for this skill level and the next */
	tmp = player_exp[level + 1] - player_exp[level];

	/* Multiply by skill cost adjustment (10x inflation) */
	tmp *= (skill_info[skill].cost_adj);

	/* Multiply by racial adjustment (10x inflation) */
	tmp *= race_adj_cost_skill[skill][p_ptr->prace];

	/* Divide by the experience adjustment, and deflate */
	divisor = (long)(EXP_ADJ * 100);


	/* Get the actual cost (standard rounding) */
	cost = (tmp + divisor / 2) / divisor;


	/* Drained skills are three times as easy to restore */
	if (level < p_ptr->pskills[skill].max)
	{
		cost /= 3;
	}

	/* Otherwise, require a minimum cost */
	else if (p_ptr->power >= 10)
	{
		int tmp_pow = MAX(2, (p_ptr->power - 8) * 8 / 10);

		/* Minimum cost depends on character power */
		s32b min_cost = player_exp[tmp_pow] - player_exp[tmp_pow - 1];

		/* It also depends on the inherent cost of the skill */
		min_cost *= skill_info[skill].cost_adj;
		min_cost *= race_adj_cost_skill[skill][p_ptr->prace];

		/* Deflate */
		min_cost /= (EXP_ADJ * 100L);

		/* Enforce minimum */
		if (cost < min_cost) cost = min_cost;

		/* Handle practice penalty */
		if (add_practice_cost) practice_penalty(skill, &cost);
	}

	/* Cost must be at least one */
	if (cost < 1L) cost = 1L;

	/* Round off cost (slightly) */
	cost = round_it(cost, 50);

	/* Return cost */
	return (cost);
}


/*
 * Determine if the character can take a (new) oath
 */
static bool can_take_oath(byte oath)
{
	/* Character cannot (currently) take more than one oath */
	if (p_ptr->oath) return (FALSE);

	/* Check individual oaths */
	switch (oath)
	{
		case OATH_OF_IRON:
		{
			/* Must not have any magic */
			if (p_ptr->pskills[S_MAGIC].max > WARRIOR_SPELLCASTING_LIMIT)
				return (FALSE);

			/* Must not be excessively good at magical devices */
			if (p_ptr->pskills[S_DEVICE].max > WARRIOR_DEVICE_LIMIT)
				return (FALSE);

			/* Require skill at any form of non-magical combat */
			if ((p_ptr->pskills[S_SWORD].max < OATH_OF_IRON_REQ) &&
			    (p_ptr->pskills[S_HAFTED].max < OATH_OF_IRON_REQ) &&
			    (p_ptr->pskills[S_POLEARM].max < OATH_OF_IRON_REQ) &&
			    (p_ptr->pskills[S_CROSSBOW].max < OATH_OF_IRON_REQ) &&
			    (p_ptr->pskills[S_BOW].max < OATH_OF_IRON_REQ) &&
			    (p_ptr->pskills[S_SLING].max < OATH_OF_IRON_REQ + 10) &&
			    (p_ptr->pskills[S_THROWING].max < OATH_OF_IRON_REQ) &&
			    (p_ptr->pskills[S_WRESTLING].max < OATH_OF_IRON_REQ) &&
			    (p_ptr->pskills[S_KARATE].max < OATH_OF_IRON_REQ))
			{
				return (FALSE);
			}
			break;
		}

		case OATH_OF_SORCERY:
		case YAVANNAS_FELLOWSHIP:
		case BLACK_MYSTERY:
		{
			/* Must not be too focused on melee weapons */
			if (p_ptr->pskills[S_SWORD].max > SPELLCASTER_WEAPON_LIMIT)
				return (FALSE);
			if (p_ptr->pskills[S_POLEARM].max > SPELLCASTER_WEAPON_LIMIT)
				return (FALSE);
			if (p_ptr->pskills[S_HAFTED].max > SPELLCASTER_WEAPON_LIMIT)
				return (FALSE);

			/* Must already know the realm of magic */
			if ((oath == OATH_OF_SORCERY) && (p_ptr->realm != MAGE))
				return (FALSE);
			if ((oath == YAVANNAS_FELLOWSHIP) && (p_ptr->realm != DRUID))
				return (FALSE);
			if ((oath == BLACK_MYSTERY) && (p_ptr->realm != NECRO))
				return (FALSE);

			break;
		}

		case COVENANT_OF_FAITH:
		{
			/* Must already be pious */
			if (p_ptr->realm != PRIEST) return (FALSE);

			/* Must not be too focused on melee weapons */
			if (p_ptr->pskills[S_SWORD].max > PIOUS_EDGED_WEAPON_LIMIT)
				return (FALSE);
			if (p_ptr->pskills[S_POLEARM].max > PIOUS_EDGED_WEAPON_LIMIT)
				return (FALSE);
			if (p_ptr->pskills[S_HAFTED].max > PRIEST_BLUNT_WEAPON_LIMIT)
				return (FALSE);

			break;
		}

		case BURGLARS_GUILD:
		{
			/* Must have a Burglary skill of at least 25 */
			if (p_ptr->pskills[S_BURGLARY].max < LEV_REQ_GUILD) return (FALSE);

			break;
		}

		default:
		{
			return (FALSE);
		}
	}

	/* Allow this oath */
	return (TRUE);
}



/* Warning defines */
#define PIOUS_EDGED            1
#define PRIEST_BLUNT_WEAPON    2
#define SPELLCASTER_WEAPON     3
#define NON_WARRIOR            4
#define WARRIOR_DEVICE         5
#define WARRIOR_SPELLS         6
#define PIETY_AND_DOMINION     7
#define THIEVING               8


/*
 * Warn players if raising a skill would cost them opportunities to take
 * certain oaths or magics.
 */
static bool can_raise_skill_confirm(int warning)
{
	cptr warn_text = "";
	char ch;

	bool accept = FALSE;

	/* Display waning text */
	switch (warning)
	{
		case PIOUS_EDGED:
		{
			warn_text = "If you raise this skill any higher, you will not be able to learn Piety.";
			break;
		}

		case PRIEST_BLUNT_WEAPON:
		{
			warn_text = "If you raise this skill any higher, you will not be able to subscribe to the Covenant of Faith.";
			break;
		}

		case SPELLCASTER_WEAPON:
		{
			warn_text = "If you raise this skill any higher, you will not be able to take a spellcaster's Oath.  You can still become a non-specialist spellcaster, though.";
			break;
		}

		case NON_WARRIOR:
		{
			warn_text = "If you raise this skill any higher, you will not be able to learn magic   (however, pious characters who have not subscribed to the Covenant of Faith can raise their blunt weapons skill to the maximum).";
			break;
		}

		case WARRIOR_DEVICE:
		{
			warn_text = "If you raise this skill any higher, you will not be able to take the Oath of Iron.";
			break;
		}

		case WARRIOR_SPELLS:
		{
			warn_text = "If you learn spells, you will not be able to take the Oath of Iron.";
			break;
		}

		case PIETY_AND_DOMINION:
		{
			warn_text = "Never can one character learn both holy Piety and dark Blood Dominion.  If you choose to raise one skill, the other will be set to zero.";
			break;
		}

		case THIEVING:
		{
			warn_text = "If you raise this skill any higher, shopkeepers will start to notice; your Charisma will drop by 2.";
		}


		/* Called with an undefined warning - assume acceptable  XXX XXX */
		default:  return (TRUE);
	}


	/* Warning */
	skill_comment(TERM_L_BLUE, warn_text);

	/* Ask */
	skill_msg(TERM_WHITE, "Advance this skill? [y/n] ");

	/* Ask until we get a clear answer */
	while (TRUE)
	{
		/* Answer */
		ch = inkey();

		/* Note accept */
		if ((ch == 'y') || (ch == 'Y'))
		{
			accept = TRUE;
			break;
		}

		/* Note cancel */
		if ((ch == 'n') || (ch == 'N') || (ch == ESCAPE))
		{
			accept = FALSE;
			skill_msg(TERM_WHITE, "Cancelled.");
			break;
		}
	}


	/* Erase message */
	erase_skill_comment();

	/* Hide cursor XXX */
	move_cursor(26, 0);

	/* Return our answer */
	return (accept);
}


/*
 * Enforce limits on skills.
 *
 * Optionally, output explanatory messages.
 *
 * Allow skills to be raised automatically unless absolutely forbidden,
 * raised after any warnings are read, or raised only if always permitted.
 *
 * Return TRUE if skill can be raised by 1%.
 */
static int can_raise_skill(int skill, bool verbose, int auto_raise)
{
	int lev = p_ptr->pskills[skill].max;

	/* Assume able to learn magic */
	cannot_learn_magic   = FALSE;
	cannot_learn_prayers = FALSE;

	/* Can never raise any skill past 100 */
	if (p_ptr->pskills[skill].cur >= PY_MAX_POWER) return (-1);


	/* If we've taken an oath, some warnings are unnecessary */
	if ((p_ptr->oath) && (!auto_raise)) auto_raise = 1;

	/* If the skill is not at maximum, we always allow advances */
	if (p_ptr->pskills[skill].cur < p_ptr->pskills[skill].max)
		return (TRUE);


	/* Handle individual skills */
	switch (skill)
	{
		case S_SWORD:
		case S_POLEARM:
		{
			/* Character has no magic realm */
			if (p_ptr->realm == NONE)
			{
				/* We haven't taken the Oath of Iron */
				if (!(p_ptr->oath & (OATH_OF_IRON)))
				{
					/* Next skill raise would exceed the piety limit */
					if (lev == PIOUS_EDGED_WEAPON_LIMIT)
					{
						/* We already can't become a priest */
						if ((p_ptr->pskills[S_SWORD].max >
						         PIOUS_EDGED_WEAPON_LIMIT) ||
							 (p_ptr->pskills[S_POLEARM].max >
							      PIOUS_EDGED_WEAPON_LIMIT))
						{
							return (TRUE);
						}

						/* Automatically accept or reject */
						if (auto_raise) return (auto_raise > 0);

						/* Manually accept or reject */
						else
						{
							return (can_raise_skill_confirm(PIOUS_EDGED));
						}
					}

					/* Next skill raise would exceed the spellcaster limit */
					if (lev == SPELLCASTER_WEAPON_LIMIT)
					{
						/* We already can't become a pure spellcaster */
						if ((p_ptr->pskills[S_SWORD].max >
						         SPELLCASTER_WEAPON_LIMIT) ||
							 (p_ptr->pskills[S_POLEARM].max >
							      SPELLCASTER_WEAPON_LIMIT) ||
							 (p_ptr->pskills[S_HAFTED].max >
							      SPELLCASTER_WEAPON_LIMIT))
						{
							return (TRUE);
						}

						if (auto_raise) return (auto_raise > 0);
						else
						{
							return (can_raise_skill_confirm(SPELLCASTER_WEAPON));
						}
					}

					/* Next skill raise would exceed the half-spellcaster limit */
					if (lev == NON_WARRIOR_LIMIT)
					{
						/* We already can't become a half-spellcaster */
						if ((p_ptr->pskills[S_SWORD].max > NON_WARRIOR_LIMIT) ||
							 (p_ptr->pskills[S_POLEARM].max > NON_WARRIOR_LIMIT) ||
							 (p_ptr->pskills[S_HAFTED].max > NON_WARRIOR_LIMIT))
						{
							return (TRUE);
						}

						if (auto_raise) return (auto_raise > 0);
						else
						{
							return (can_raise_skill_confirm(NON_WARRIOR));
						}
					}
				}

				/* Allow all other raises */
				return (TRUE);
			}

			/* Character is among the pious */
			else if (p_ptr->realm == PRIEST)
			{
				if (lev < PIOUS_EDGED_WEAPON_LIMIT) return (TRUE);

				else if (verbose) prt("The pious cannot raise their edged weapons skills any higher than this.", 1, 2);
				return (-1);
			}


			/* Character is a pure spellcaster of another realm */
			else if ((p_ptr->oath & (OATH_OF_SORCERY)) ||
			         (p_ptr->oath & (YAVANNAS_FELLOWSHIP)) ||
			         (p_ptr->oath & (BLACK_MYSTERY)))
			{
				/* Allow raises up to the limit */
				if (lev < SPELLCASTER_WEAPON_LIMIT) return (TRUE);

				if (verbose) prt("A specialist spellcaster cannot raise his weapons skills any higher than this.", 1, 2);
				return (-1);
			}

			/* Character is a non-specialist spellcaster */
			else
			{
				/* Next skill raise would exceed the pure spellcaster limit */
				if (lev == SPELLCASTER_WEAPON_LIMIT)
				{
					if (auto_raise) return (auto_raise > 0);
					else
					{
						return (can_raise_skill_confirm(SPELLCASTER_WEAPON));
					}
				}

				/* Allow raises up to the limit */
				else if (lev < NON_WARRIOR_LIMIT) return (TRUE);

				if (verbose) prt("A magic-user cannot raise his weapons skills any higher than this.", 1, 2);
				return (-1);
			}

			break;
		}

		case S_HAFTED:
		{
			/* Character has no magic realm */
			if (p_ptr->realm == NONE)
			{
				/* We haven't taken the Oath of Iron */
				if (!(p_ptr->oath & (OATH_OF_IRON)))
				{
					/* Next skill raise would exceed the pure spellcaster limit */
					if (lev == SPELLCASTER_WEAPON_LIMIT)
					{
						/* We already can't become a pure spellcaster */
						if ((p_ptr->pskills[S_SWORD].max >
						         SPELLCASTER_WEAPON_LIMIT) ||
							 (p_ptr->pskills[S_POLEARM].max >
							      SPELLCASTER_WEAPON_LIMIT) ||
							 (p_ptr->pskills[S_HAFTED].max >
							      SPELLCASTER_WEAPON_LIMIT))
						{
							return (TRUE);
						}

						if (auto_raise) return (auto_raise > 0);
						else
						{
							return (can_raise_skill_confirm(SPELLCASTER_WEAPON));
						}
					}

					/* Next skill raise would exceed the half-spellcaster limit */
					if (lev == NON_WARRIOR_LIMIT)
					{
						/* We already can't become a half-spellcaster */
						if ((p_ptr->pskills[S_SWORD].max > NON_WARRIOR_LIMIT) ||
							 (p_ptr->pskills[S_POLEARM].max > NON_WARRIOR_LIMIT) ||
							 (p_ptr->pskills[S_HAFTED].max > NON_WARRIOR_LIMIT))
						{
							return (TRUE);
						}

						if (auto_raise) return (auto_raise > 0);
						else
						{
							return (can_raise_skill_confirm(NON_WARRIOR));
						}
					}
				}

				/* Allow all other raises */
				return (TRUE);
			}


			/* Character is among the pious */
			else if (p_ptr->realm == PRIEST)
			{
				/* Character has taken the Covenant of Faith */
				if (p_ptr->oath & (COVENANT_OF_FAITH))
				{
					if (lev < PRIEST_BLUNT_WEAPON_LIMIT) return (TRUE);

					if (verbose) prt("The Covenant of Faith forbids you to focus on blunt weapons any more than this.", 1, 2);
					return (-1);
				}

				/* Character hasn't */
				else
				{
					/* Next skill raise would forbid specialization in piety */
					if (lev == PRIEST_BLUNT_WEAPON_LIMIT)
					{
						if (auto_raise) return (auto_raise > 0);
						else
						{
							return (can_raise_skill_confirm(PRIEST_BLUNT_WEAPON));
						}
					}

					/* No limits */
					return (TRUE);
				}
			}

			/* Character is a pure spellcaster of another realm */
			else if ((p_ptr->oath & (OATH_OF_SORCERY)) ||
			         (p_ptr->oath & (YAVANNAS_FELLOWSHIP)) ||
			         (p_ptr->oath & (BLACK_MYSTERY)))
			{
				if (lev < SPELLCASTER_WEAPON_LIMIT) return (TRUE);

				if (verbose) prt(format("Oath-bound %ss cannot raise their weapon skills any higher than this.", realm_user_desc()), 1, 2);
				return (-1);
			}

			/* Character is a non-specialist spellcaster */
			else
			{
				/* Next skill raise would forbid specialization in magic */
				if (lev == SPELLCASTER_WEAPON_LIMIT)
				{
					if (auto_raise) return (auto_raise > 0);
					else
					{
						return (can_raise_skill_confirm(SPELLCASTER_WEAPON));
					}
				}

				if (lev < NON_WARRIOR_LIMIT) return (TRUE);

				if (verbose) prt("Spellcasters of your realm cannot raise their weapons skills any higher than this.", 1, 2);
				return (-1);
			}

			break;
		}

		case S_WRESTLING:
		case S_KARATE:
		{
			/* Reminder of how to switch between martial arts */
			if ((lev == 0) && (verbose))
			{
				skill_comment(TERM_L_BLUE,
					"You may use the '|' key to change barehanded combat methods.");
			}

			break;
		}

		case S_MAGIC:
		{
			/* The Oath of Iron forbids the learning of magic */
			if (p_ptr->oath & (OATH_OF_IRON))
			{
				if (verbose) prt("The Oath of Iron forbids the learning of magic.",
					1, 2);
				return (-1);
			}

			/* Must not be too focused on melee weapons */
			if ((p_ptr->pskills[S_SWORD].max > NON_WARRIOR_LIMIT) ||
			    (p_ptr->pskills[S_POLEARM].max > NON_WARRIOR_LIMIT) ||
			    (p_ptr->pskills[S_HAFTED].max > NON_WARRIOR_LIMIT))
			{
				cannot_learn_magic = TRUE;
			}

			/* Check for priestly rules */
			if ((p_ptr->pskills[S_SWORD].max > PIOUS_EDGED_WEAPON_LIMIT) ||
			    (p_ptr->pskills[S_POLEARM].max > PIOUS_EDGED_WEAPON_LIMIT))
			{
				cannot_learn_prayers = TRUE;
			}

			/* Explain limitations */
			if ((cannot_learn_magic) && (cannot_learn_prayers))
			{
				if (verbose) prt("You are too much the fighter to learn spells or prayers.",
					1, 2);
				return (-1);
			}

			/* Explain limitations */
			else if (cannot_learn_magic)
			{
				if (verbose) prt("You are too much the fighter to learn spells, but you can learn prayers.",
					1, 2);
				return (TRUE);
			}

			/* Explain limitations */
			else if (cannot_learn_prayers)
			{
				if (verbose) prt("You are too much the fighter to learn prayers, but you can learn spells.",
					1, 2);
				return (TRUE);
			}

			/* Notice possibility of taking the Oath of Iron */
			if ((!p_ptr->oath) &&
			    (p_ptr->pskills[S_DEVICE].max <= WARRIOR_DEVICE_LIMIT) &&
			   ((p_ptr->pskills[S_SWORD].max >= OATH_OF_IRON_REQ) ||
			    (p_ptr->pskills[S_HAFTED].max >= OATH_OF_IRON_REQ) ||
			    (p_ptr->pskills[S_POLEARM].max >= OATH_OF_IRON_REQ) ||
			    (p_ptr->pskills[S_CROSSBOW].max >= OATH_OF_IRON_REQ) ||
			    (p_ptr->pskills[S_BOW].max >= OATH_OF_IRON_REQ) ||
			    (p_ptr->pskills[S_SLING].max >= OATH_OF_IRON_REQ + 10) ||
			    (p_ptr->pskills[S_THROWING].max >= OATH_OF_IRON_REQ) ||
			    (p_ptr->pskills[S_WRESTLING].max >= OATH_OF_IRON_REQ) ||
			    (p_ptr->pskills[S_KARATE].max >= OATH_OF_IRON_REQ)))
			{
				if (auto_raise) return (auto_raise > 0);
				else
				{
					return (can_raise_skill_confirm(WARRIOR_SPELLS));
				}
			}

			break;
		}

		case S_MPOWER:
		{
			/* The Oath of Iron forbids the learning of magic */
			if (p_ptr->oath & (OATH_OF_IRON))
			{
				if (verbose) prt("The Oath of Iron forbids the learning of magic.", 1, 2);
				return (-1);
			}

			/* No realm chosen -- Ask the player to increase Spellcasting */
			else if (!p_ptr->realm)
			{
				if (verbose) prt("Raise your Spellcasting skill to choose a magic realm.", 1, 2);
				return (-1);
			}

			break;
		}

		case S_DEVICE:
		{
			/* The Oath of Iron limits the use of magical devices */
			if (p_ptr->oath & (OATH_OF_IRON))
			{
				if (lev < WARRIOR_DEVICE_LIMIT) return (TRUE);

				if (verbose) prt("Warriors cannot raise their magic mastery skill any higher than this.", 1, 2);
				return (-1);
			}

			/* Warn if we would lose the chance to take the Oath of Iron */
			else if ((lev == WARRIOR_DEVICE_LIMIT) &&
			         (can_take_oath(OATH_OF_IRON)))
			{
				if (auto_raise) return (auto_raise > 0);
				else
				{
					return (can_raise_skill_confirm(WARRIOR_DEVICE));
				}
			}

			/* Allow raises */
			return (TRUE);

			break;
		}

		case S_BURGLARY:
		{
			/* Must join the Burglars' Guild to get very good at Burglary */
			if (!(p_ptr->oath & (BURGLARS_GUILD)))
			{
				if (lev < NON_GUILDBROTHERS_LIMIT) return (TRUE);

				if (verbose) prt("To raise this skill further, you must first join the Burglars' Guild.", 1, 2);
				return (-1);
			}

			/* Allow raises */
			return (TRUE);

			break;
		}

		case S_PIETY:
		{
			/* Necromantic magic forbids holy alliance */
			if (p_ptr->realm == NECRO)
			{
				if (verbose) prt("Users of the necromantic arts cannot learn Holy Alliance.", 1, 2);

				/* Paranoia -- set Holy Alliance to zero */
				p_ptr->pskills[S_PIETY].max = p_ptr->pskills[S_PIETY].cur = 0;

				return (-1);
			}

			/* Increasing Piety sets Blood Dominion to zero */
			if (p_ptr->pskills[S_DOMINION].max > 0)
			{
				/* We never automatically raise a forbidden skill */
				if (auto_raise) return (-1);

				/* We may allow the player to manually raise the skill */
				else
				{
					return (can_raise_skill_confirm(PIETY_AND_DOMINION));
				}
			}
			break;
		}

		case S_DOMINION:
		{
			/* Holy lore forbids gaining dark knowledge */
			if (p_ptr->oath & (COVENANT_OF_FAITH))
			{
				if (verbose) prt("Acolytes of holy lore cannot learn Blood Dominion.", 1, 2);

				/* Paranoia -- set Blood Dominion to zero */
				p_ptr->pskills[S_DOMINION].max = p_ptr->pskills[S_DOMINION].cur = 0;

				return (-1);
			}

			/* Increasing Blood Dominion sets Piety to zero */
			if (p_ptr->pskills[S_PIETY].max > 0)
			{
				/* We never automatically raise a forbidden skill */
				if (auto_raise) return (-1);

				/* We may allow the player to manually raise the skill */
				else
				{
					return (can_raise_skill_confirm(PIETY_AND_DOMINION));
				}
			}
			break;
		}

		default:
		{
			/* Assume no limits */
			return (TRUE);
		}
	}


	/* Assume no limits */
	return (TRUE);
}


/*
 * Increase or decrease the given skill.  This function is used for all
 * adjustments to skills other than those purchased by the player.
 *
 * Will never raise a skill above 100% or below 0.
 * Should not raise skills if this restricts the player's options.  XXX XXX
 *
 * Should never be called with a value for change that is not -1 or 1.
 */
bool alter_skill(int skill, int change, bool perm)
{
	/* Check inputs */
	if (change >  1) change =  1;
	if (change < -1) change = -1;
	if (change == 0) return (FALSE);

	/* We want to raise a skill */
	if (change == 1)
	{
		/* Check to see if the skill can be raised */
		if (can_raise_skill(skill, FALSE, -1) <= 0) return (FALSE);

		/* Adjust maximum skill */
		if (p_ptr->pskills[skill].max == p_ptr->pskills[skill].cur)
		{
			int new_talent;

			/* Adjust maximum skill */
			p_ptr->pskills[skill].max++;

			/* Check for new talents */
			new_talent = check_for_new_talent(skill, p_ptr->pskills[skill].max);

			/* Note that a new talent is available */
			if (new_talent >= 0)
			{
				msg_format("You can now use the talent \"%s\".",
					talent_info[new_talent].name);
			}
		}

		/* Adjust skill */
		p_ptr->pskills[skill].cur++;
	}

	/* We want to lower a skill */
	else if (change == -1)
	{
		/* Cannot reduce this skill any more */
		if (p_ptr->pskills[skill].cur == 0)
		{
			if (!perm) return (FALSE);
			else if (p_ptr->pskills[skill].max == 0) return (FALSE);
		}

		/* Adjust skill */
		if (p_ptr->pskills[skill].cur >= 1) p_ptr->pskills[skill].cur--;

		/* Optionally, adjust maximum skill */
		if (perm)
		{
			if (p_ptr->pskills[skill].max >= 1)
			    p_ptr->pskills[skill].max--;
		}
	}


	/* Recalculate character power */
	calc_power();

	/* Redraw and update some stuff */
	p_ptr->redraw |= (PR_EXP | PR_HP | PR_TITLE);
	p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS | PU_SCORE);


	/* Success */
	return (TRUE);
}


/*
 * Raise skills randomly.  Used for potions of Gain Skill and similar.
 *
 * Return TRUE if anything was noticed.
 */
bool raise_skills(int amount)
{
	int i, skill, level, cost;
	s32b k;

	bool notice = FALSE;
	s16b skill_raise_chance[NUM_SK_USED];
	s32b skill_raise_total = 0L;

	/* Build a table of increasable skills */
	for (i = 0; i < NUM_SK_USED; i++)
	{
		/* Get maximum level of this skill */
		level = p_ptr->pskills[i].max;

		/* Bias in favour of higher-level skills */
		skill_raise_chance[i] = level * level;

		/* Sum up probabilities */
		skill_raise_total += level * level;
	}

	/* Raise skills -- if any exist to raise */
	if (skill_raise_total)
	{
		for (i = 0; i < 200; i++)
		{
			/* Randomize */
			k = rand_int(skill_raise_total);

			/* Find the skill */
			for (skill = S_SWORD; skill < NUM_SK_USED; skill++)
			{
				/* Found the entry */
				if (k < skill_raise_chance[skill]) break;

				/* Decrement */
				k -= skill_raise_chance[skill];
			}

			/* Get the cost of raising this skill */
			cost = adv_cost(skill, TRUE);

			/* Cost too high.  Try again. */
			if (cost > amount) continue;

			/* Try to raise the skill */
			if (alter_skill(skill, 1, TRUE))
			{
				/* Notice */
				notice = TRUE;

				/* Message */
				msg_format("You feel your %s improving.",
					skill_info[skill].desc);

				/* Charge cost */
				amount -= cost;


				/* Handle special cases XXX XXX */
				if ((skill == S_INFUSION) &&
					 (p_ptr->pskills[skill].max >= LEV_REQ_GRENADE))
				{
					/* Hack - learn about potions of essences */
					k_info[lookup_kind(TV_POTION,
						SV_POTION_GRENADE)].special |=
						(SPECIAL_AWARE | SPECIAL_EVER_SEEN);
				}
			}
		}
	}

	/* Stash leftover experience in the unspent pool */
	if (amount > 0)
	{
		p_ptr->exp += amount;
		notice = TRUE;
	}

	/* Return "notice" */
	return (notice);
}


/*
 * Raise a given skill by one percent, if allowed.
 *
 * Used when purchasing skill advances.
 */
static int adv_skill(int skill, bool pay_exp)
{
	s32b cost;
	int advance;

	/* Get the cost of raising this skill */
	cost = adv_cost(skill, TRUE);

	/* Note maximum level reached */
	if (cost == -1)
	{
		prt("You have raised this skill to the maximum.", 1, 2);
		pause_for(250);
		return (-1);
	}

	/* Note too expensive */
	if ((pay_exp) && (cost > p_ptr->exp))
	{
		prt("You do not have enough unspent experience to raise this skill.", 1, 2);
		pause_for(250);
		return (-1);
	}

	/* Check to see if the skill can be raised, output error messages */
	advance = can_raise_skill(skill, TRUE, 0);

	/* Skill is forbidden, or raise was cancelled */
	if (advance <= 0) return (advance);


	/* Advance the skill by 1% */
	p_ptr->pskills[skill].cur++;

	/* Advance maximum skill if appropriate */
	if (p_ptr->pskills[skill].cur > p_ptr->pskills[skill].max)
	{
		int new_talent;

		p_ptr->pskills[skill].max = p_ptr->pskills[skill].cur;

		/* Check for new talents */
		new_talent = check_for_new_talent(skill, p_ptr->pskills[skill].max);

		/* Note that a new talent is available */
		if (new_talent >= 0)
		{
			/* Print notice */
			skill_comment(TERM_L_BLUE, format("You can now use the talent \"%s\".",
				talent_info[new_talent].name));
		}

		/* Archery */
		if ((skill == S_CROSSBOW) || (skill == S_BOW) || (skill == S_SLING))
		{
			int old_ammo_tval = p_ptr->ammo_tval;
			int i, j;

			/* Hack -- assume we're ready to use this skill */
			if (skill == S_CROSSBOW) p_ptr->ammo_tval = TV_BOLT;
			if (skill == S_BOW) p_ptr->ammo_tval = TV_ARROW;
			if (skill == S_SLING) p_ptr->ammo_tval = TV_SHOT;

			/* Do we get a increase in shooting speed? */
			i = missile_bonus(TR_PVAL_SHOTS, p_ptr->pskills[skill].cur);
			j = missile_bonus(TR_PVAL_SHOTS, p_ptr->pskills[skill].cur - 1);

			if (i > j) skill_comment(TERM_L_BLUE, "You now shoot more quickly.");

			/* Do we get a increase in shooting force? */
			i = missile_bonus(TR_PVAL_MIGHT, p_ptr->pskills[skill].cur);
			j = missile_bonus(TR_PVAL_MIGHT, p_ptr->pskills[skill].cur - 1);

			if (i > j) skill_comment(TERM_L_BLUE, "You now shoot with greater force.");

			/* Hack -- restore actual ammo */
			p_ptr->ammo_tval = old_ammo_tval;
		}

		/* Wrestling */
		if (skill == S_WRESTLING)
		{
			if (p_ptr->pskills[skill].max == LEV_REQ_WRESTLE_STR_BONUS1)
				skill_comment(TERM_L_BLUE, "Your strength increases.");
			if (p_ptr->pskills[skill].max == LEV_REQ_WRESTLE_STR_BONUS2)
				skill_comment(TERM_L_BLUE, "Your strength increases.");
			if (p_ptr->pskills[skill].max == LEV_REQ_WRESTLE_DEX_BONUS1)
				skill_comment(TERM_L_BLUE, "Your dexterity increases.");
		}

		/* Karate */
		if (skill == S_KARATE)
		{
			if (p_ptr->pskills[skill].max == LEV_REQ_KARATE_STR_BONUS1)
				skill_comment(TERM_L_BLUE, "Your strength increases.");
			if (p_ptr->pskills[skill].max == LEV_REQ_KARATE_DEX_BONUS1)
				skill_comment(TERM_L_BLUE, "Your dexterity increases.");
			if (p_ptr->pskills[skill].max == LEV_REQ_KARATE_DEX_BONUS2)
				skill_comment(TERM_L_BLUE, "Your dexterity increases.");
		}

		/* Spellpower */
		if (skill == S_MPOWER)
		{
			/* Recalculate mana */
			calc_mana();

			/* Note current mana -- if any */
			if (p_ptr->msp)
			{
				skill_comment(TERM_L_BLUE,
					format("Your maximum mana is now %d.", p_ptr->msp));
			}
		}

		/* Piety */
		if (skill == S_PIETY)
		{
			/* We have an increased light radius */
			if (p_ptr->pskills[skill].max == LEV_REQ_XTRA_LIGHT)
			{
				/* Print notice */
				skill_comment(TERM_L_BLUE, "You start to shine with holy light.");
			}
		}

		/* Advancing burglary */
		if (skill == S_BURGLARY)
		{
			/* We can steal */
			if (p_ptr->pskills[skill].max == LEV_REQ_BURGLE)
			{
				/* Print notice */
				skill_comment(TERM_L_BLUE, "You can now steal things from monsters.  You also get accuracy bonuses with light melee weapons and penalties with heavy ones.  Your Charisma drops by 2.");
			}

			/* We can lock doors */
			if (p_ptr->pskills[skill].max == LEV_REQ_LOCK_DOOR)
			{
				/* Print notice */
				skill_comment(TERM_L_BLUE, "You can now lock closed doors.");
			}

			/* We can set traps */
			if (p_ptr->pskills[skill].max == LEV_REQ_TRAP)
			{
				/* Print notice */
				skill_comment(TERM_L_BLUE, "You can now set traps.");
			}
		}

		/* Perception */
		if (skill == S_PERCEPTION)
		{
			/* We can get precognition messages */
			if (p_ptr->pskills[skill].max == LEV_REQ_PRECOG)
			{
				/* Print notice */
				skill_comment(TERM_L_BLUE, "You will sometimes feel the presence of monsters or objects on levels you enter.  Note that not all precognition messages are true.");
			}
		}

		/* Alchemy */
		if (skill == S_ALCHEMY)
		{
			/* We can make potions and scrolls */
			if (p_ptr->pskills[skill].max == LEV_REQ_ALCHEMY)
			{
				/* Print notice */
				skill_comment(TERM_L_BLUE, "You can now make potions and scrolls, and will now save empty bottles and blank parchments.");

				/* Start saving bottles and parchments */
				p_ptr->suppress_bottle = FALSE;
			}

			/* We can make rings and amulets */
			if (p_ptr->pskills[skill].max == LEV_REQ_ALCHEMY_RING)
			{
				/* Print notice */
				skill_comment(TERM_L_BLUE, "You can now make rings and amulets.");
			}
		}

		/* Infusion */
		if (skill == S_INFUSION)
		{
			/* We can infuse forged items */
			if (p_ptr->pskills[skill].max == LEV_REQ_INFUSE)
			{
				/* Print notice */
				skill_comment(TERM_L_BLUE, "You can now infuse forged items and search for essences.");
			}

			/* We can make grenades */
			if (p_ptr->pskills[skill].max == LEV_REQ_GRENADE)
			{
				/* Print notice */
				if (p_ptr->pskills[S_ALCHEMY].cur >= LEV_REQ_ALCHEMY)
				{
					skill_comment(TERM_L_BLUE, "You can now create potions of essences to throw at your foes.");
				}
				else
				{
					skill_comment(TERM_L_BLUE, "Once you raise your Alchemy skill to 10, you will be able to create potions of essences to throw at your foes.");
				}

				/* Hack - learn about potions of essences */
				k_info[lookup_kind(TV_POTION, SV_POTION_GRENADE)].special |=
					(SPECIAL_AWARE | SPECIAL_EVER_SEEN);
			}
		}
	}

	/* Spend experience */
	if (pay_exp) p_ptr->exp -= cost;

	/* Recalculate character power */
	calc_power();


	/* Redraw and update some stuff (later) */
	p_ptr->redraw |= (PR_EXP | PR_HP | PR_TITLE);
	p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS | PU_SCORE);

	/* Success */
	return (TRUE);
}



/*
 * Use '*' to take oaths or do other skill-specific things.
 */
static bool special_skill_command(int skill, bool *must_accept)
{
	int answer;


	/* Determine if this skill allows any special commands */
	switch (skill)
	{
		case S_SWORD:
		case S_HAFTED:
		case S_POLEARM:
		case S_WRESTLING:
		case S_KARATE:
		case S_SLING:
		case S_BOW:
		case S_CROSSBOW:
		case S_THROWING:
		{
			/* Normal skill requirement to take this Oath */
			int req_skill = OATH_OF_IRON_REQ;

			/* Slings have a higher requirement */
			if (skill == S_SLING) req_skill += 10;

			/* Can take the Oath of Iron */
			if ((p_ptr->pskills[skill].cur >= req_skill) &&
			    (can_take_oath(OATH_OF_IRON)))
			{
				/* Warning */
				skill_comment(TERM_L_UMBER, "The Oath of Iron is the greatest of all the commitments you can make, for it alone inducts you into the secrets of the most powerful warrior arts.  But be warned!  Those who take the Oath of Iron can learn no magic, and find magical devices hard to master.\n     Once taken, an Oath cannot be revoked!");

				/* Ask */
				skill_msg(TERM_WHITE, "Take the Oath of Iron? [y/n] ");

				/* Answer */
				answer = inkey();

				/* The Oath of Iron */
				if ((answer == 'Y') || (answer == 'y'))
				{
					p_ptr->oath |= (OATH_OF_IRON);

					/* Clear info text */
					erase_skill_comment();

					/* Refuse to cancel */
					*must_accept = TRUE;

					/* Message about new talents */
					skill_comment(TERM_UMBER, "You may be able to use several new talents depending on your skill.");

					/* Wait for it */
					skill_msg(TERM_WHITE, "Press any key to continue.");
					(void)inkey();
				}
				else
				{
					skill_msg(TERM_WHITE, "Cancelled.");
				}
			}

			/* Nothing happens */
			else return (FALSE);

			break;
		}

		case S_WIZARDRY:
		{
			/* Check if an oath allowed */
			if (can_take_oath(OATH_OF_SORCERY))
			{
				/* Warning */
				skill_comment(TERM_L_RED, format("No wizards' secrets are hidden from %s who takes the Oath of Sorcery, but specialization in magic comes at a significant cost in hitpoints and warrior skills.\n     Once taken, an Oath cannot be revoked!", sex_info[p_ptr->psex].pronoun));

				/* Ask */
				skill_msg(TERM_WHITE, "Take the Oath of Sorcery? [y/n] ");

				/* Answer */
				answer = inkey();

				/* The Oath of Sorcery */
				if ((answer == 'Y') || (answer == 'y'))
				{
					p_ptr->oath |= (OATH_OF_SORCERY);

					/* Refuse to cancel */
					*must_accept = TRUE;
				}
				else
				{
					skill_msg(TERM_WHITE, "Cancelled.");
				}
			}

			/* Nothing happens */
			else return (FALSE);

			break;
		}

		case S_PIETY:
		{
			/* Check if an oath allowed */
			if (can_take_oath(COVENANT_OF_FAITH))
			{
				/* Warning */
				skill_comment(TERM_L_BLUE, format("Mighty and generous is the Almighty, and blessed indeed are those who serve Him.  Subscribing to the Covenant of Faith takes intense commitment however; hitpoints and most warrior skills suffer somewhat.\n     Once taken, an Oath cannot be revoked!"));

				/* Ask */
				skill_msg(TERM_WHITE, "Subscribe to the Covenant of Faith? [y/n] ");

				/* Answer */
				answer = inkey();

				/* The Covenant of Faith */
				if ((answer == 'Y') || (answer == 'y'))
				{
					p_ptr->oath |= (COVENANT_OF_FAITH);

					/* Refuse to cancel */
					*must_accept = TRUE;
				}
				else
				{
					skill_msg(TERM_WHITE, "Cancelled.");
				}
			}

			/* Nothing happens */
			else return (FALSE);

			break;
		}

		case S_NATURE:
		{
			/* Check if an oath allowed */
			if (can_take_oath(YAVANNAS_FELLOWSHIP))
			{
				/* Warning */
				skill_comment(TERM_L_GREEN, "The lore of the natural world offers great power, but only the mystics of Yavanna's Fellowship can hope to comprehend it fully.  So intent are they on their chosen paths, however, that they have relatively few hitpoints and poor warrior skills.\n     Once taken, an Oath cannot be revoked!");

				/* Ask */
				skill_msg(TERM_WHITE, "Join Yavanna's Fellowship? [y/n] ");

				/* Answer */
				answer = inkey();

				/* Yavanna's Fellowship */
				if ((answer == 'Y') || (answer == 'y'))
				{
					p_ptr->oath |= (YAVANNAS_FELLOWSHIP);

					/* Refuse to cancel */
					*must_accept = TRUE;
				}
				else
				{
					skill_msg(TERM_WHITE, "Cancelled.");
				}
			}

			/* Nothing happens */
			else return (FALSE);

			break;
		}

		case S_DOMINION:
		{
			/* Check if an oath allowed */
			if (can_take_oath(BLACK_MYSTERY))
			{
				/* Warning */
				skill_comment(TERM_PURPLE, "The rituals of the true necromancers are the very stuff of nightmares.  This most perilous of magics requires extraordinary dedication; mastery comes only at the price of lost hitpoints and reduced combat skills.\n     Once taken, an Oath cannot be revoked!");

				/* Ask */
				skill_msg(TERM_WHITE, "Bind yourself to the Black Mystery? [y/n] ");

				/* Answer */
				answer = inkey();

				/* The Black Mystery */
				if ((answer == 'Y') || (answer == 'y'))
				{
					p_ptr->oath |= (BLACK_MYSTERY);

					/* Refuse to cancel */
					*must_accept = TRUE;
				}
				else
				{
					skill_msg(TERM_WHITE, "Cancelled.");
				}
			}

			/* Nothing happens */
			else return (FALSE);

			break;
		}

		case S_BURGLARY:
		{
			/* Check if an oath allowed */
			if (can_take_oath(BURGLARS_GUILD))
			{
				/* Warning */
				skill_comment(TERM_SLATE, format("The lore and cunning of the %s of Misrule is shared only with fellow Guild members.  Scofflaws, cutpurses, wastrels, vagabonds, keenfingers, and common thugs need not apply; such scum are mere prey to the Shadowknives.\n     But remember one thing:  \"Once a Guild%s, always a Guild%s!\"", (p_ptr->psex == 0 ? "Daughters" : "Sons"), (p_ptr->psex == 0 ? "sister" : "brother"), (p_ptr->psex == 0 ? "sister" : "brother")));

				/* Ask */
				if (p_ptr->psex == 0)
					skill_msg(TERM_WHITE, "Become a Sister in the Guild of Burglars? [y/n] ");
				else
					skill_msg(TERM_WHITE, "Become a Brother in the Guild of Burglars? [y/n] ");

				/* Answer */
				answer = inkey();

				/* The Guild of Burglars */
				if ((answer == 'Y') || (answer == 'y'))
				{
					p_ptr->oath |= (BURGLARS_GUILD);

					/* Refuse to cancel */
					*must_accept = TRUE;
				}
				else
				{
					skill_msg(TERM_WHITE, "Cancelled.");
				}
			}

			/* Nothing happens */
			else return (FALSE);

			break;
		}

		default:
		{
			/* Nothing happens */
			return (FALSE);
		}
	}

	/* Erase message */
	erase_skill_comment();

	/* Something happened */
	return (TRUE);
}


/*
 * Print information about the currently selected skill.
 */
static void prt_skill_select(int skill)
{
	char out[100];
	char buf[100];
	cptr cmddesc = "";
	int lev;
	byte attr = TERM_WHITE;


	/* Get and display cost to advance this skill */
	if (can_raise_skill(skill, FALSE, 1) > 0)
	{
		/* Get the cost with and without the practice penalty */
		u16b cost   = adv_cost(skill, TRUE);
		u16b b_cost = adv_cost(skill, FALSE);

		sprintf(out, "XP needed to advance %s: ",
			skill_info[skill].name);

		/* Display */
		move_cursor(0, 0);
		roff(out, 2, 80);

		/* Determine a color to display the exp cost in */
		if      (cost > MIN(b_cost * 5, b_cost + 2500)) attr = TERM_RED;
		else if (cost > MIN(b_cost * 3, b_cost + 1500)) attr = TERM_ORANGE;
		else if (cost > MIN(5*b_cost/3, b_cost +  300)) attr = TERM_YELLOW;
		else if (cost > b_cost)                         attr = TERM_SLATE;
		else                                            attr = TERM_WHITE;

		/* Display the exp cost */
		c_roff(attr, format("%d", cost), 2, 80);
	}

	else if (p_ptr->pskills[skill].cur == PY_MAX_POWER)
	{
		skill_msg(TERM_L_GREEN,
			format("You have mastered %s.", skill_info[skill].name));
	}
	else if (p_ptr->pskills[skill].cur == 0)
	{
		skill_msg(TERM_L_RED,
			format("%s is forbidden to you.", skill_info[skill].name));
	}
	else
	{
		skill_msg(TERM_WHITE, "You cannot raise this skill further.");
	}

	/* Print unspent exp */
	sprintf(out, "Unspent XP: %ld", p_ptr->exp);
	prt(out, 0, 56);


	/* Special information for the magic level skill for non-spellcasters */
	if ((skill == S_MAGIC) && (p_ptr->realm == NONE))
	{
		/* Print a special message for those forbidden to learn magic */
		if (p_ptr->oath & (OATH_OF_IRON))
		{
			sprintf(out, "The Oath of Iron forbids you to learn magic.");
		}

		/* Print a special message for able to start spellcasting */
		else
		{
			sprintf(out, "Allows you to choose a realm of magic.");
		}
	}

	/* Generic skill description */
	else
	{
		sprintf(out, "Improves your %s.", skill_info[skill].desc);
	}

	/* Display the skill description */
	move_cursor(1, 0);
	c_roff_centered(TERM_L_BLUE, out, 0, 80);

	/* Display a rate of learning indicator */
	if (TRUE)
	{
		cptr rate_desc = "";

		int adj_cost = race_adj_cost_skill[skill][p_ptr->prace];

		/* Describe rate of learning */
		if      (adj_cost >= 17) rate_desc = "extremely slowly";
		else if (adj_cost >= 14) rate_desc = "very slowly";
		else if (adj_cost >= 12) rate_desc = "slowly";
		else if (adj_cost == 11) rate_desc = "a bit slowly";
		else if (adj_cost == 10) rate_desc = "at normal speed";
		else if (adj_cost ==  9) rate_desc = "relatively quickly";
		else if (adj_cost ==  8) rate_desc = "quickly";
		else if (adj_cost <=  7) rate_desc = "very quickly";

		/* Note unusual rate of learning */
		if (adj_cost != 10)
		{
			strcpy(buf, format("(A %s learns this skill %s.)",
				race_info[p_ptr->prace].title, rate_desc));

			/* Display a rate of learning indicator */
			move_cursor(2, 0);
			c_roff_centered(TERM_L_WHITE, buf, 0, 80);
		}
	}


	/* Get this skill's level */
	lev = p_ptr->pskills[skill].cur;

	/* Determine if this skill allows any special commands */
	switch (skill)
	{
		case S_SWORD:
		case S_HAFTED:
		case S_POLEARM:
		case S_WRESTLING:
		case S_KARATE:
		case S_SLING:
		case S_BOW:
		case S_CROSSBOW:
		{
			/* Can take the Oath of Iron */
			if (can_take_oath(OATH_OF_IRON))
			{
				cmddesc = "Oath of Iron";
				attr = TERM_L_UMBER;
			}
			break;
		}

		case S_WIZARDRY:
		{
			/* Check if an oath allowed */
			if (can_take_oath(OATH_OF_SORCERY))
			{
				cmddesc = "Oath of Sorcery";
				attr = TERM_L_RED;
			}
			break;
		}

		case S_PIETY:
		{
			/* Check if an oath allowed */
			if (can_take_oath(COVENANT_OF_FAITH))
			{
				cmddesc = "Covenant of Faith";
				attr = TERM_BLUE;
			}
			break;
		}

		case S_NATURE:
		{
			/* Check if an oath allowed */
			if (can_take_oath(YAVANNAS_FELLOWSHIP))
			{
				cmddesc = "Yavanna's Fellowship";
				attr = TERM_GREEN;
			}
			break;
		}

		case S_DOMINION:
		{
			/* Check if an oath allowed */
			if (can_take_oath(BLACK_MYSTERY))
			{
				cmddesc = "Black Mystery";
				attr = TERM_PURPLE;
			}
			break;
		}

		case S_BURGLARY:
		{
			/* Check if an oath allowed */
			if (can_take_oath(BURGLARS_GUILD))
			{
				cmddesc = "Burglar's Guild";
				attr = TERM_SLATE;
			}
			break;
		}
	}

	/* Display the special command key */
	if (strlen(cmddesc))
	{
		(void)Term_gotoxy(27, 23);
		c_roff(attr, format("*) %s", cmddesc), 0, 0);
	}

	/* Special key to raise skill back up to maximum */
	else if (p_ptr->pskills[skill].cur < p_ptr->pskills[skill].max)
	{
		(void)Term_gotoxy(27, 23);
		c_roff(TERM_WHITE, "!) Recover skill", 0, 0);
	}
}


/*
 * Print the given skill and its rank in the appropriate column
 * and row.
 */
static void prt_skill_rank(int skill)
{
	char buf1[38], buf2[18];
	int row, col;
	char c;

	bool drained = (p_ptr->pskills[skill].cur < p_ptr->pskills[skill].max);

	byte a = TERM_WHITE;

	/* Highlight selected skill */
	if (skill == selected) a = TERM_YELLOW;


	/* Skip unused skills */
	if (!(skill_info[skill].name)) return;

	/* Skill cannot be raised further */
	if (can_raise_skill(skill, FALSE, 1) <= 0)
	{
		/* Skill is not allowed at all */
		if (p_ptr->pskills[skill].cur == 0)
		{
			a = TERM_L_DARK;
			if (skill == selected) a = TERM_SLATE;
		}

		/* Skill is maxed out */
		else
		{
			a = TERM_L_GREEN;
			if (skill == selected) a = TERM_GREEN;
		}
	}

	/* Skill has been drained */
	else if (p_ptr->pskills[skill].cur < p_ptr->pskills[skill].max)
	{
		/* Darker colors for drained skills */
		if (skill == selected) a = TERM_ORANGE;
		else                   a = TERM_SLATE;
	}

	/* Skill is zero */
	else if (p_ptr->pskills[skill].max == 0 && !(skill == selected))
	{
		a = TERM_L_WHITE;
	}

	/* Work out the row and column of the screen to use */
	row = 3 + skill;
	if (skill < ((NUM_SK_USED + 1) / 2)) col = 0;
	else
	{
		col = 40;
		row -= (NUM_SK_USED + 1) / 2;
	}

	/* Display the skill percentage */
	sprintf(buf2, "%3d%%", p_ptr->pskills[skill].cur);

	/* The character corresponding to the skill */
	c = index_chars_lower[skill];

	/* Format the string */
	sprintf(buf1, "%c) %-18s%c %s", c, skill_info[skill].name,
		(drained ? '!' : ':'), buf2);

	/* Print the skill */
	c_prt(a, buf1, row, col);

	/* Move the cursor */
	(void)Term_gotoxy(col + 28, row);

	/* Get and display cost to advance this skill (unless forbidden) */
	if (can_raise_skill(skill, FALSE, 1) > 0)
	{
		byte attr;

		/* Get the cost with and without the practice penalty */
		u16b cost   = adv_cost(skill, TRUE);
		u16b b_cost = adv_cost(skill, FALSE);

		/* Determine a color to display the exp cost in */
		if      (cost > MIN(b_cost * 5, b_cost + 2500)) attr = TERM_RED;
		else if (cost > MIN(b_cost * 3, b_cost + 1500)) attr = TERM_ORANGE;
		else if (cost > MIN(b_cost*5/3, b_cost +  300)) attr = TERM_YELLOW;
		else if (cost > b_cost)                         attr = TERM_SLATE;
		else                                            attr = TERM_WHITE;

		/* Display the exp cost */
		c_roff(attr, format(" %5d ", cost), 0, 0);

		/* Skill has some investment in it */
		if (p_ptr->pskills[skill].max)
		{
			if (cost <= p_ptr->exp)
			{
				/* Color the ':' green if skill is not too costly to raise */
				a = (skill == selected) ? TERM_GREEN : TERM_L_GREEN;
			}
			else
			{
				/* Color the ':' red if skill is too costly to raise */
				a = (skill == selected) ? TERM_RED : TERM_L_RED;
			}

			/* Print a ":" or a "!" */
			(void)Term_putch(col + 21, row, a, (drained ? '!' : ':'));
		}

	}
	else
	{
		/* Display the exp cost */
		c_roff(TERM_L_DARK, "   -   ", 0, 0);
	}
}

/*
 * Print out all the skills, along with their ranks.
 */
static void print_all_skills(bool must_accept)
{
	int i;

	char buf[100];

	/* Get realm color */
	byte attr = realm_color();

	/* Clear all of the screen except the special message area */
	for (i = 0; i < Term->hgt; i++)
	{
		if ((i < 17) || (i > 20)) clear_row(i);
	}

	/* Print the static information */
	prt(format("%c-%c) Select skills                        +/=) Advance skills",
		index_chars_lower[0], index_chars_lower[NUM_SK_USED - 1]),
		23, 5);

	if (!must_accept)
		prt("ESC) Cancel      RETURN) Accept             ?) Get help      ",
			24, 5);
	else
		prt("                 RETURN) Accept             ?) Get help      ",
			24, 5);

	c_prt(TERM_L_BLUE, format("Power: %d", p_ptr->power), 24, 69);


	/* Center realm description */
	center_string(buf, sizeof(buf), realm_desc(), 78);

	/* Print out realm description */
	c_prt(attr, buf, 21, 0);

	/* Print all the skills */
	for (i = 0; i < NUM_SK_USED; i++) prt_skill_rank(i);
}


/*
 * Handle the case where raising one skill also increases others.
 *
 * First skill must be undrained.  No skill can benefit if its maximum
 * is higher than the first skill's.
 */
static void raise_other_skills(int skill)
{
	int level = p_ptr->pskills[skill].max;

	/* Recovering a drained skill has no effect on other skills */
	if (p_ptr->pskills[skill].cur < p_ptr->pskills[skill].max) return;

	/* Burglary */
	if (skill == S_BURGLARY)
	{
		/* Raise disarming 3/4rds as fast as burglary rises */
		if (level > p_ptr->pskills[S_DISARM].max)
		{
			if (level % 4 != 0) (void)adv_skill(S_DISARM, FALSE);
		}

		/* Raise perception 1/3rd as fast as burglary rises */
		if (level > p_ptr->pskills[S_PERCEPTION].max)
		{
			if (level % 3 == 0) (void)adv_skill(S_PERCEPTION, FALSE);
		}

		/* Raise stealth 1/3rd as fast as burglary rises */
		if (level > p_ptr->pskills[S_STEALTH].max)
		{
			if (level % 3 == 1) (void)adv_skill(S_STEALTH, FALSE);
		}

		/* Raise dodging 1/4th as fast as burglary rises */
		if (level > p_ptr->pskills[S_DODGING].max)
		{
			if (level % 4 == 2) (void)adv_skill(S_DODGING, FALSE);
		}
	}

	/* Swordsmanship */
	if (skill == S_SWORD)
	{
		/* Raise jousting 1/2th as fast as swordsmanship rises */
		if (level > p_ptr->pskills[S_POLEARM].max)
		{
			if (level % 2 == 0) (void)adv_skill(S_POLEARM, FALSE);
		}

		/* Raise clubbing 1/4th as fast as swordsmanship rises */
		if (level > p_ptr->pskills[S_HAFTED].max)
		{
			if (level % 4 == 2) (void)adv_skill(S_HAFTED, FALSE);
		}
	}

	/* Clubbing */
	if (skill == S_HAFTED)
	{
		/* Raise swordsmanship 1/4th as fast as clubbing rises */
		if (level > p_ptr->pskills[S_SWORD].max)
		{
			if (level % 4 == 0) (void)adv_skill(S_SWORD, FALSE);
		}

		/* Raise jousting 1/4th as fast as clubbing rises */
		if (level > p_ptr->pskills[S_POLEARM].max)
		{
			if (level % 4 == 2) (void)adv_skill(S_POLEARM, FALSE);
		}
	}

	/* Jousting */
	if (skill == S_POLEARM)
	{
		/* Raise swordsmanship 1/2th as fast as jousting rises */
		if (level > p_ptr->pskills[S_SWORD].max)
		{
			if (level % 2 == 0) (void)adv_skill(S_SWORD, FALSE);
		}

		/* Raise clubbing 1/4th as fast as jousting rises */
		if (level > p_ptr->pskills[S_HAFTED].max)
		{
			if (level % 4 == 2) (void)adv_skill(S_HAFTED, FALSE);
		}
	}

	/* Crossbows */
	if (skill == S_CROSSBOW)
	{
		/* Raise bows 1/2th as fast as crossbows rises */
		if (level > p_ptr->pskills[S_BOW].max)
		{
			if (level % 2 == 0) (void)adv_skill(S_BOW, FALSE);
		}
	}

	/* Bows */
	if (skill == S_BOW)
	{
		/* Raise crossbows 1/2th as fast as bows rises */
		if (level > p_ptr->pskills[S_CROSSBOW].max)
		{
			if (level % 2 == 0) (void)adv_skill(S_CROSSBOW, FALSE);
		}
	}
}


/*
 * Make certain that a player who has invested in Piety wants to become a
 * Necromancer, or that one who has invested in Blood Dominion wants to
 * become a Priest.
 */
static bool realm_check(int realm)
{
	cptr warn_text = "", lost_skill = "", realm_text = "";
	int warn_attr = TERM_WHITE;

	/* Some realms need no special checks */
	if ((realm != PRIEST) && (realm != NECRO)) return (TRUE);

	/* We only need to check sometimes */
	if ((realm == PRIEST) && (p_ptr->pskills[S_DOMINION].max == 0))
		return (TRUE);

	if ((realm == NECRO) && (p_ptr->pskills[S_PIETY].max == 0))
		return (TRUE);


	/* Get warning text */
	if (realm == PRIEST)
	{
		realm_text = "Piety";
		warn_text = "Acolytes of divine lore must purge themselves of all within them that is unholy; all your Blood Dominion will be lost if you cultivate Piety.";
		warn_attr = TERM_L_BLUE;
		lost_skill = "Blood Dominion";
	}
	if (realm == NECRO)
	{
		realm_text = "Necromancy";
		warn_text = "Students of the Dark Arts cannot retain any ties to the Divine; all your Holy Alliance will be lost if you cultivate necromancy.";
		warn_attr = TERM_L_PURPLE;
		lost_skill = "Holy Alliance";
	}

	/* Warning */
	skill_comment(TERM_L_BLUE, warn_text);

	/* Ask */
	prt("", 0, 0);
	skill_msg(TERM_WHITE, format("Choose %s, and forfeit all %s? [y/n] ", realm_text, lost_skill));

	/* Ask until we get a clear answer */
	while (TRUE)
	{
		/* Answer */
		char ch = inkey();

		/* Note accept */
		if ((ch == 'y') || (ch == 'Y'))
		{
			skill_comment(TERM_WHITE, "");
			return (TRUE);
		}

		/* Note cancel */
		if ((ch == 'n') || (ch == 'N') || (ch == ESCAPE))
		{
			skill_msg(TERM_WHITE, "Cancelled.");
			skill_comment(TERM_WHITE, "");
			return (FALSE);
		}
	}

	return (FALSE);
}


/*
 * Make certain that a player who has invested in Piety wants to become a
 * Necromancer, or that one who has invested in Blood Dominion wants to
 * become a Priest.
 */
static bool choose_realm(void)
{
	char ch;

	/* May pick a realm */
	skill_msg(TERM_WHITE, "You may pick a realm of magic.");
	(void)inkey();

	/* Apply limitations */
	if ((cannot_learn_magic) && (cannot_learn_prayers))
	{
		skill_msg(TERM_WHITE, "You can learn no magic.");
	}

	/* Apply limitations */
	else if (cannot_learn_magic)
	{
		skill_msg(TERM_WHITE, "Press 'P' to study Piety.");
	}

	/* Apply limitations */
	else if (cannot_learn_prayers)
	{
		prt("Will you study S)orcery, D)ruidic magic, or N)ecromancy?", 0, 2);
	}

	/* No limitations */
	else
	{
		prt("Will you study S)orcery, P)iety, D)ruidic magic, or N)ecromancy? ", 0, 2);
	}

	/* Get command */
	ch = inkey();


	switch (ch)
	{
		case 's':
		case 'S':
		{
			if (!cannot_learn_magic)
			{
				p_ptr->realm = MAGE;
			}
			break;
		}
		case 'p':
		case 'P':
		{
			if (!cannot_learn_prayers)
			{
				if (realm_check(PRIEST))
					p_ptr->realm = PRIEST;
			}
			break;
		}
		case 'd':
		case 'D':
		{
			if (!cannot_learn_magic)
			{
				p_ptr->realm = DRUID;
			}
			break;
		}
		case 'n':
		case 'N':
		{
			if (!cannot_learn_magic)
			{
				if (realm_check(NECRO))
					p_ptr->realm = NECRO;
			}
			break;
		}
		default:
		{
			break;
		}
	}

	/* Allow cancel */
	if (!p_ptr->realm)
	{
		/* Hack -- undo skill purchase */
		p_ptr->pskills[selected].cur = 0;
		p_ptr->pskills[selected].max = 0;
		p_ptr->exp += adv_cost(selected, TRUE);
		return (FALSE);
	}

	/* New magic realm */
	mp_ptr = &magic_info[p_ptr->realm];

	/* We are now a user of holy lore */
	if (p_ptr->realm == PRIEST)
	{
		/* Set Blood Dominion to zero */
		p_ptr->pskills[S_DOMINION].max = 0;
		p_ptr->pskills[S_DOMINION].cur = 0;
	}

	/* We are now a user of necromantic arts */
	if (p_ptr->realm == NECRO)
	{
		/* Set Piety to zero */
		p_ptr->pskills[S_PIETY].max = 0;
		p_ptr->pskills[S_PIETY].cur = 0;
	}

	/* Update preferences for chosen realm */
	(void)process_pref_file("realm.prf");

	return (TRUE);
}



/*
 * Allow the player to examine and improve his skill levels.
 */
void do_cmd_skills(void)
{
	char ch;
	int num;

	int i;

	int old_rows = screen_rows;

	bool accepted = FALSE;
	bool must_accept = FALSE;

	/* Old player's skills and unused experience */
	skill_data old_pskills[NUM_SKILLS];
	int old_exp;
	byte old_oath, old_realm;


	/*
	 * Winners cannot continue to raise their skills (no trying everything
	 * in a single game).
	 */
	if (p_ptr->total_winner)
	{
		/* Message */
		message(MSG_L_BLUE, 0, "You have won the game!  You need learn nothing more!");
		return;
	}


	/* Save screen */
	screen_save();

	/* Set to 25 screen rows */
	(void)Term_rows(FALSE);

	/* Clear screen */
	(void)Term_clear();

	/* Select the skill last advanced */
	selected = p_ptr->lastadv;


	/* Save old status */
	for (i = 0; i < NUM_SKILLS; i++)
	{
		old_pskills[i].cur = p_ptr->pskills[i].cur;
		old_pskills[i].max = p_ptr->pskills[i].max;
	}
	old_exp = p_ptr->exp;
	old_oath = p_ptr->oath;
	old_realm = p_ptr->realm;


	/* Continue until satisfied */
	while (TRUE)
	{
		/* Print out the skills */
		print_all_skills(must_accept);

		/* Print information on the current skill */
		prt_skill_select(selected);

		/* Hack -- hide the cursor  XXX XXX */
		(void)Term_gotoxy(0, 26);

		/* Get a command */
		ch = inkey();

		/* Cancel */
		if (ch == ESCAPE)
		{
			if (must_accept)
			{
				skill_msg(realm_color(), "Once taken, an Oath is binding!");
				(void)inkey();
				continue;
			}
			else
			{
				accepted = FALSE;
				break;
			}
		}

		/* Confirm */
		if ((ch == '\n') || (ch == '\r'))
		{
			accepted = TRUE;
			break;
		}

		/* Help me. */
		if (ch == '?')
		{
			p_ptr->get_help_index = HELP_SKILLS + selected;
			do_cmd_help();
			(void)Term_clear();
			continue;
		}

		/* Move about the list -- go up one skill */
		if ((ch == '8') || ((rogue_like_commands) && (ch == 'k')))
		{
			if (selected > 0) selected--;
			continue;
		}

		/* Move about the list -- go down one skill */
		if ((ch == '2') || ((rogue_like_commands) && (ch == 'j')))
		{
			if (selected < NUM_SK_USED-1) selected++;
			continue;
		}

		/* Move about the list -- go left one column */
		if ((ch == '4') || ((rogue_like_commands) && (ch == 'h')))
		{
			if (selected >= (NUM_SK_USED+1) / 2)
			    selected -= (NUM_SK_USED+1) / 2;

			if (selected < 0) selected = 0;
			continue;
		}

		/* Move about the list -- go right one column */
		if ((ch == '6') || ((rogue_like_commands) && (ch == 'l')))
		{
			if (selected <  (NUM_SK_USED+1) / 2)
			    selected += (NUM_SK_USED+1) / 2;

			if (selected >= NUM_SK_USED) selected = NUM_SK_USED - 1;
			continue;
		}

		/* Allow skill-specific actions */
		if (ch == '*')
		{
			/* If something happened, continue */
			if (special_skill_command(selected, &must_accept)) continue;
		}

		/* Advance the skill if the key was '+' or '=' */
		if (ch == '+' || ch == '=')
		{
			int advance = adv_skill(selected, TRUE);

			/* Cannot raise the skill */
			if (advance < 0)
			{
				skill_msg(TERM_WHITE, "You cannot raise this skill further.");
				(void)Term_fresh();
				pause_for(250);
			}

			/* Choose not to raise the skill */
			else if (advance == 0)
			{
				skill_msg(TERM_WHITE, "Cancelled...");
				(void)Term_fresh();
				pause_for(250);
			}

			/* Can raise the skill */
			else
			{
				/* Choose a magic realm */
				if ((selected == S_MAGIC) && (!p_ptr->realm))
				{
					/* Allow cancel */
					if (!choose_realm()) continue;
				}

				/* Piety and Blood Dominion never mix */
				else if (selected == S_PIETY)
				{
					p_ptr->pskills[S_DOMINION].cur = 0;
					p_ptr->pskills[S_DOMINION].max = 0;
				}

				else if (selected == S_DOMINION)
				{
					p_ptr->pskills[S_PIETY].cur = 0;
					p_ptr->pskills[S_PIETY].max = 0;
				}

				/* Handle skills that help other skills */
				raise_other_skills(selected);
			}

			prt_skill_rank(selected);
			continue;
		}

		/* Fix the skill */
		if (ch == '!')
		{
			while (p_ptr->pskills[selected].cur < p_ptr->pskills[selected].max)
			{
				/* Cannot raise the skill */
				if (adv_skill(selected, TRUE) <= 0)
				{
					skill_msg(TERM_WHITE, "You cannot raise this skill further.");
					(void)inkey();
					break;
				}
			}

			prt_skill_rank(selected);
			continue;
		}

		/* Some keys are only used to clear messages */
		if (ch == ' ')
		{
			continue;
		}


		/* Attempt to use the command as an index */
		num = get_index(ch, TRUE);

		/* Jump to a skill */
		if ((num >= 0) && (num < NUM_SK_USED))
		{
			selected = num;
		}

		/* Illegal skill index */
		else
		{
			bell("Illegal skill option.");
		}
	}

	/* Remember selected skill */
	p_ptr->lastadv = selected;


	/* Check attempt to cancel */
	if (!accepted)
	{
		/* You cannot revoke an Oath */
		if (must_accept)
		{
			skill_msg(realm_color(), "Once taken, an Oath is binding!");
			(void)inkey();
		}
		else
		{
			bool changed = FALSE;

			for (i = 0; i < NUM_SKILLS; i++)
			{
				if (p_ptr->pskills[i].cur != old_pskills[i].cur)
				{
					p_ptr->pskills[i].cur = old_pskills[i].cur;
					changed = TRUE;
				}
				if (p_ptr->pskills[i].max != old_pskills[i].max)
				{
					p_ptr->pskills[i].max = old_pskills[i].max;
					changed = TRUE;
				}
			}

			if (p_ptr->exp != old_exp)
			{
				p_ptr->exp = old_exp;
				changed = TRUE;
			}
			if (p_ptr->realm != old_realm)
			{
				p_ptr->realm = old_realm;
				mp_ptr = &magic_info[NONE];
				changed = TRUE;
			}

			/* Recalculate character power */
			calc_power();

			/* Flash a message */
			if (changed)
			{
				skill_msg(TERM_WHITE, "Cancelled...");
				pause_for(250);
			}
		}
	}

	/* Flush any pending messages */
	message_flush();


	/* Set to 50 screen rows, if we were showing 50 before */
	if (old_rows == 50)
	{
		p_ptr->redraw |= (PR_MAP | PR_BASIC | PR_EXTRA);
		(void)Term_rows(TRUE);
	}

	/* Restore main screen */
	screen_load();


	/* If one bare-handed skill is obviously the primary, choose it */
	if ((p_ptr->barehand == S_KARATE) &&
	       (p_ptr->pskills[S_WRESTLING].cur >
	    3 * p_ptr->pskills[S_KARATE].cur))
	{
		do_cmd_barehanded();
	}
	if ((p_ptr->barehand == S_WRESTLING) &&
	       (p_ptr->pskills[S_KARATE].cur >
	    3 * p_ptr->pskills[S_WRESTLING].cur))
	{
		do_cmd_barehanded();
	}

	/* Update various things */
	p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_TORCH);

	/* Update stuff */
	p_ptr->update |= (PU_MONSTERS);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_CMDLIST);

	/* Handle stuff */
	handle_stuff();
}
