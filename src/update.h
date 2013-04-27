/*
 * Bit flags for the "p_ptr->notice" variable
 */
#define PN_COMBINE	0x00000001L	/* Combine the pack */
#define PN_REORDER	0x00000002L	/* Reorder the pack */
/* xxx (many) */


/*
 * Bit flags for the "p_ptr->update" variable
 */
#define PU_BONUS		0x00000001L	/* Calculate bonuses */
#define PU_TORCH		0x00000002L	/* Calculate torch radius */
/* xxx (many) */
#define PU_HP		0x00000010L	/* Calculate chp and mhp */
#define PU_MANA		0x00000020L	/* Calculate csp and msp */
#define PU_SPELLS	0x00000040L	/* Calculate spells */
/* xxx (many) */
/* xxx (many) */
#define PU_UN_VIEW	0x00010000L	/* Forget view */
/* xxx (many) */
#define PU_VIEW		0x00100000L	/* Update view */
/* xxx */
#define PU_MONSTERS	0x01000000L	/* Update monsters */
#define PU_DISTANCE	0x02000000L	/* Update distances */
/* xxx */
#define PU_FLOW		0x10000000L	/* Update flow */
/* xxx (many) */


/*
 * Bit flags for the "p_ptr->redraw" variable
 */
#define PR_MISC		0x00000001L	/* Display Race/Class */
#define PR_TITLE		0x00000002L	/* Display Title */
#define PR_LEV		0x00000004L	/* Display Level */
#define PR_EXP		0x00000008L	/* Display Experience */
#define PR_STATS		0x00000010L	/* Display Stats */
#define PR_ARMOR		0x00000020L	/* Display Armor */
#define PR_HP		0x00000040L	/* Display Hitpoints */
#define PR_MANA		0x00000080L	/* Display Mana */
#define PR_GOLD		0x00000100L	/* Display Gold */
#define PR_DEPTH		0x00000200L	/* Display Depth */
#define PR_EQUIPPY	0x00000400L	/* Display equippy chars */
#define PR_HEALTH	0x00000800L	/* Display Health Bar */
#define PR_CUT		0x00001000L	/* Display Extra (Cut) */
#define PR_STUN		0x00002000L	/* Display Extra (Stun) */
#define PR_HUNGER	0x00004000L	/* Display Extra (Hunger) */
/* xxx */
#define PR_BLIND		0x00010000L	/* Display Extra (Blind) */
#define PR_CONFUSED	0x00020000L	/* Display Extra (Confused) */
#define PR_AFRAID	0x00040000L	/* Display Extra (Afraid) */
#define PR_POISONED	0x00080000L	/* Display Extra (Poisoned) */
#define PR_STATE		0x00100000L	/* Display Extra (State) */
#define PR_SPEED		0x00200000L	/* Display Extra (Speed) */
#define PR_STUDY		0x00400000L	/* Display Extra (Study) */
/* xxx */
#define PR_EXTRA		0x01000000L	/* Display Extra Info */
#define PR_BASIC		0x02000000L	/* Display Basic Info */
#define PR_MAP		0x04000000L	/* Display Map */
#define PR_WIPE		0x08000000L	/* Hack -- Total Redraw */
/* xxx */
/* xxx */
/* xxx */
/* xxx */

/*
 * Bit flags for the "p_ptr->window" variable (etc)
 */
#define PW_INVEN		0x00000001L	/* Display inven/equip */
#define PW_EQUIP		0x00000002L	/* Display equip/inven */
#define PW_SPELL		0x00000004L	/* Display spell list */
#define PW_PLAYER		0x00000008L	/* Display character */
/* xxx */
/* xxx */
#define PW_MESSAGE	0x00000040L	/* Display messages */
#define PW_OVERHEAD	0x00000080L	/* Display overhead view */
#define PW_MONSTER	0x00000100L	/* Display monster recall */
#define PW_OBJECT		0x00000200L	/* Display object recall */
/* xxx */
#define PW_SNAPSHOT	0x00000800L	/* Display snap-shot */
/* xxx */
/* xxx */
#define PW_BORG_1		0x00004000L	/* Display borg messages */
#define PW_BORG_2		0x00008000L	/* Display borg status */

