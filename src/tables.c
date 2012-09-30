/* File: tables.c */

/* Purpose: Angband Tables */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"




/*
 * Global array for looping through the "keypad directions"
 */
s16b ddd[9] =
{ 2, 8, 6, 4, 3, 1, 9, 7, 5 };

/*
 * Global arrays for converting "keypad direction" into offsets
 */
s16b ddx[10] =
{ 0, -1, 0, 1, -1, 0, 1, -1, 0, 1 };

s16b ddy[10] =
{ 0, 1, 1, 1, 0, 0, 0, -1, -1, -1 };

/*
 * Global arrays for optimizing "ddx[ddd[i]]" and "ddy[ddd[i]]"
 */
s16b ddx_ddd[9] =
{ 0, 0, 1, -1, 1, -1, 1, -1, 0 };

s16b ddy_ddd[9] =
{ 1, -1, 0, 0, 1, 1, -1, -1, 0 };



/*
* Global array for converting numbers to uppercase hexadecimal digit
 * This array can also be used to convert a number to an octal digit
 */
char hexsym[16] =
{
	'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};


/*
 * Stat Table (INT/WIS) -- Number of half-spells per level
 */
byte adj_mag_study[] =
{
	0	/* 3 */,
	0	/* 4 */,
	0	/* 5 */,
	0	/* 6 */,
	0	/* 7 */,
	1	/* 8 */,
	1	/* 9 */,
	1	/* 10 */,
	1	/* 11 */,
	2	/* 12 */,
	2	/* 13 */,
	2	/* 14 */,
	2	/* 15 */,
	2	/* 16 */,
	2	/* 17 */,
	2	/* 18/00-18/09 */,
	2	/* 18/10-18/19 */,
	2	/* 18/20-18/29 */,
	2	/* 18/30-18/39 */,
	2	/* 18/40-18/49 */,
	3	/* 18/50-18/59 */,
	3	/* 18/60-18/69 */,
	3	/* 18/70-18/79 */,
	3	/* 18/80-18/89 */,
	4	/* 18/90-18/99 */,
	4	/* 18/100-18/109 */,
	4	/* 18/110-18/119 */,
	5	/* 18/120-18/129 */,
	5	/* 18/130-18/139 */,
	5	/* 18/140-18/149 */,
	5	/* 18/150-18/159 */,
	5	/* 18/160-18/169 */,
	5	/* 18/170-18/179 */,
	5	/* 18/180-18/189 */,
	5	/* 18/190-18/199 */,
	5	/* 18/200-18/209 */,
	6	/* 18/210-18/219 */,
	6	/* 18/220+ */
};


/*
 * Stat Table (INT/WIS) -- extra half-mana-points per level
 */
byte adj_mag_mana[] =
{
	0       /* 3 */,
	0       /* 4 */,
	0       /* 5 */,
	0       /* 6 */,
	0       /* 7 */,
	1       /* 8 */,
	2       /* 9 */,
	2       /* 10 */,
	2       /* 11 */,
	2       /* 12 */,
	2       /* 13 */,
	2       /* 14 */,
	2       /* 15 */,
	2       /* 16 */,
	2       /* 17 */,
	3       /* 18/00-18/09 */,
	3       /* 18/10-18/19 */,
	3       /* 18/20-18/29 */,
	3       /* 18/30-18/39 */,
	3       /* 18/40-18/49 */,
	4       /* 18/50-18/59 */,
	4       /* 18/60-18/69 */,
	5       /* 18/70-18/79 */,
	6       /* 18/80-18/89 */,
	7       /* 18/90-18/99 */,
	8       /* 18/100-18/109 */,
	9       /* 18/110-18/119 */,
	10      /* 18/120-18/129 */,
	11      /* 18/130-18/139 */,
	12      /* 18/140-18/149 */,
	13      /* 18/150-18/159 */,
	14      /* 18/160-18/169 */,
	15      /* 18/170-18/179 */,
	16      /* 18/180-18/189 */,
	16      /* 18/190-18/199 */,
	17      /* 18/200-18/209 */,
	17      /* 18/210-18/219 */,
	18      /* 18/220+ */
};


/*
 * Stat Table (INT/WIS) -- Minimum failure rate (percentage)
 */
byte adj_mag_fail[] =
{
	99      /* 3 */,
	99      /* 4 */,
	99      /* 5 */,
	99      /* 6 */,
	99      /* 7 */,
	50      /* 8 */,
	30      /* 9 */,
	20      /* 10 */,
	15      /* 11 */,
	12      /* 12 */,
	11      /* 13 */,
	10      /* 14 */,
	9       /* 15 */,
	8       /* 16 */,
	7       /* 17 */,
	6       /* 18/00-18/09 */,
	6       /* 18/10-18/19 */,
	5       /* 18/20-18/29 */,
	5       /* 18/30-18/39 */,
	5       /* 18/40-18/49 */,
	4       /* 18/50-18/59 */,
	4       /* 18/60-18/69 */,
	4       /* 18/70-18/79 */,
	4       /* 18/80-18/89 */,
	3       /* 18/90-18/99 */,
	3       /* 18/100-18/109 */,
	2       /* 18/110-18/119 */,
	2       /* 18/120-18/129 */,
	2       /* 18/130-18/139 */,
	2       /* 18/140-18/149 */,
	1       /* 18/150-18/159 */,
	1       /* 18/160-18/169 */,
	1       /* 18/170-18/179 */,
	1       /* 18/180-18/189 */,
	1       /* 18/190-18/199 */,
	0       /* 18/200-18/209 */,
	0       /* 18/210-18/219 */,
	0       /* 18/220+ */
};


/*
 * Stat Table (INT/WIS) -- Various things
 */
byte adj_mag_stat[] =
{
	0       /* 3 */,
	0       /* 4 */,
	0       /* 5 */,
	0       /* 6 */,
	0       /* 7 */,
	1       /* 8 */,
	1       /* 9 */,
	1       /* 10 */,
	1       /* 11 */,
	1       /* 12 */,
	1       /* 13 */,
	1       /* 14 */,
	2       /* 15 */,
	2       /* 16 */,
	2       /* 17 */,
	3       /* 18/00-18/09 */,
	3       /* 18/10-18/19 */,
	3       /* 18/20-18/29 */,
	3       /* 18/30-18/39 */,
	3       /* 18/40-18/49 */,
	4       /* 18/50-18/59 */,
	4       /* 18/60-18/69 */,
	5       /* 18/70-18/79 */,
	6       /* 18/80-18/89 */,
	7       /* 18/90-18/99 */,
	8       /* 18/100-18/109 */,
	9       /* 18/110-18/119 */,
	10      /* 18/120-18/129 */,
	11      /* 18/130-18/139 */,
	12      /* 18/140-18/149 */,
	13      /* 18/150-18/159 */,
	14      /* 18/160-18/169 */,
	15      /* 18/170-18/179 */,
	16      /* 18/180-18/189 */,
	17      /* 18/190-18/199 */,
	18      /* 18/200-18/209 */,
	19      /* 18/210-18/219 */,
	20      /* 18/220+ */
};


/*
 * Stat Table (CHR) -- payment percentages
 */
byte adj_chr_gold[] =
{
	130     /* 3 */,
	125     /* 4 */,
	122     /* 5 */,
	120     /* 6 */,
	118     /* 7 */,
	116     /* 8 */,
	114     /* 9 */,
	112     /* 10 */,
	110     /* 11 */,
	108     /* 12 */,
	106     /* 13 */,
	104     /* 14 */,
	103     /* 15 */,
	102     /* 16 */,
	101     /* 17 */,
	100     /* 18/00-18/09 */,
	99      /* 18/10-18/19 */,
	98      /* 18/20-18/29 */,
	97      /* 18/30-18/39 */,
	96      /* 18/40-18/49 */,
	95      /* 18/50-18/59 */,
	94      /* 18/60-18/69 */,
	93      /* 18/70-18/79 */,
	92      /* 18/80-18/89 */,
	91      /* 18/90-18/99 */,
	90      /* 18/100-18/109 */,
	89      /* 18/110-18/119 */,
	88      /* 18/120-18/129 */,
	87      /* 18/130-18/139 */,
	86      /* 18/140-18/149 */,
	85      /* 18/150-18/159 */,
	84      /* 18/160-18/169 */,
	83      /* 18/170-18/179 */,
	82      /* 18/180-18/189 */,
	81      /* 18/190-18/199 */,
	80      /* 18/200-18/209 */,
	79      /* 18/210-18/219 */,
	78      /* 18/220+ */
};


/*
 * Stat Table (INT) -- Magic devices
 */
byte adj_int_dev[] =
{
	0       /* 3 */,
	0       /* 4 */,
	0       /* 5 */,
	0       /* 6 */,
	0       /* 7 */,
	1       /* 8 */,
	1       /* 9 */,
	1       /* 10 */,
	1       /* 11 */,
	1       /* 12 */,
	1       /* 13 */,
	1       /* 14 */,
	2       /* 15 */,
	2       /* 16 */,
	2       /* 17 */,
	3       /* 18/00-18/09 */,
	3       /* 18/10-18/19 */,
	4       /* 18/20-18/29 */,
	4       /* 18/30-18/39 */,
	5       /* 18/40-18/49 */,
	5       /* 18/50-18/59 */,
	6       /* 18/60-18/69 */,
	6       /* 18/70-18/79 */,
	7       /* 18/80-18/89 */,
	7       /* 18/90-18/99 */,
	8       /* 18/100-18/109 */,
	9       /* 18/110-18/119 */,
	10      /* 18/120-18/129 */,
	11      /* 18/130-18/139 */,
	12      /* 18/140-18/149 */,
	13      /* 18/150-18/159 */,
	14      /* 18/160-18/169 */,
	15      /* 18/170-18/179 */,
	16      /* 18/180-18/189 */,
	17      /* 18/190-18/199 */,
	18      /* 18/200-18/209 */,
	19      /* 18/210-18/219 */,
	20      /* 18/220+ */
};


/*
 * Stat Table (WIS) -- Saving throw
 */
byte adj_wis_sav[] =
{
	0       /* 3 */,
	0       /* 4 */,
	0       /* 5 */,
	0       /* 6 */,
	0       /* 7 */,
	1       /* 8 */,
	1       /* 9 */,
	1       /* 10 */,
	1       /* 11 */,
	1       /* 12 */,
	1       /* 13 */,
	1       /* 14 */,
	2       /* 15 */,
	2       /* 16 */,
	2       /* 17 */,
	3       /* 18/00-18/09 */,
	3       /* 18/10-18/19 */,
	3       /* 18/20-18/29 */,
	3       /* 18/30-18/39 */,
	3       /* 18/40-18/49 */,
	4       /* 18/50-18/59 */,
	4       /* 18/60-18/69 */,
	5       /* 18/70-18/79 */,
	5       /* 18/80-18/89 */,
	6       /* 18/90-18/99 */,
	7       /* 18/100-18/109 */,
	8       /* 18/110-18/119 */,
	9       /* 18/120-18/129 */,
	10      /* 18/130-18/139 */,
	11      /* 18/140-18/149 */,
	12      /* 18/150-18/159 */,
	13      /* 18/160-18/169 */,
	14      /* 18/170-18/179 */,
	15      /* 18/180-18/189 */,
	16      /* 18/190-18/199 */,
	17      /* 18/200-18/209 */,
	18      /* 18/210-18/219 */,
	19      /* 18/220+ */
};


/*
 * Stat Table (DEX) -- disarming
 */
byte adj_dex_dis[] =
{
	0       /* 3 */,
	0       /* 4 */,
	0       /* 5 */,
	0       /* 6 */,
	0       /* 7 */,
	0       /* 8 */,
	0       /* 9 */,
	0       /* 10 */,
	0       /* 11 */,
	0       /* 12 */,
	1       /* 13 */,
	1       /* 14 */,
	1       /* 15 */,
	2       /* 16 */,
	2       /* 17 */,
	4       /* 18/00-18/09 */,
	4       /* 18/10-18/19 */,
	4       /* 18/20-18/29 */,
	4       /* 18/30-18/39 */,
	5       /* 18/40-18/49 */,
	5       /* 18/50-18/59 */,
	5       /* 18/60-18/69 */,
	6       /* 18/70-18/79 */,
	6       /* 18/80-18/89 */,
	7       /* 18/90-18/99 */,
	8       /* 18/100-18/109 */,
	8       /* 18/110-18/119 */,
	8       /* 18/120-18/129 */,
	8       /* 18/130-18/139 */,
	8       /* 18/140-18/149 */,
	9       /* 18/150-18/159 */,
	9       /* 18/160-18/169 */,
	9       /* 18/170-18/179 */,
	9       /* 18/180-18/189 */,
	9       /* 18/190-18/199 */,
	10      /* 18/200-18/209 */,
	10      /* 18/210-18/219 */,
	10      /* 18/220+ */
};


/*
 * Stat Table (INT) -- disarming
 */
byte adj_int_dis[] =
{
	0       /* 3 */,
	0       /* 4 */,
	0       /* 5 */,
	0       /* 6 */,
	0       /* 7 */,
	1       /* 8 */,
	1       /* 9 */,
	1       /* 10 */,
	1       /* 11 */,
	1       /* 12 */,
	1       /* 13 */,
	1       /* 14 */,
	2       /* 15 */,
	2       /* 16 */,
	2       /* 17 */,
	3       /* 18/00-18/09 */,
	3       /* 18/10-18/19 */,
	3       /* 18/20-18/29 */,
	4       /* 18/30-18/39 */,
	4       /* 18/40-18/49 */,
	5       /* 18/50-18/59 */,
	6       /* 18/60-18/69 */,
	7       /* 18/70-18/79 */,
	8       /* 18/80-18/89 */,
	9       /* 18/90-18/99 */,
	10      /* 18/100-18/109 */,
	10      /* 18/110-18/119 */,
	11      /* 18/120-18/129 */,
	12      /* 18/130-18/139 */,
	13      /* 18/140-18/149 */,
	14      /* 18/150-18/159 */,
	15      /* 18/160-18/169 */,
	16      /* 18/170-18/179 */,
	17      /* 18/180-18/189 */,
	18      /* 18/190-18/199 */,
	19      /* 18/200-18/209 */,
	19      /* 18/210-18/219 */,
	20      /* 18/220+ */
};


/*
 * Stat Table (DEX) -- bonus to ac (plus 128)
 */
byte adj_dex_ta[] =
{
	128 + -4        /* 3 */,
	128 + -3        /* 4 */,
	128 + -2        /* 5 */,
	128 + -1        /* 6 */,
	128 + 0 /* 7 */,
	128 + 0 /* 8 */,
	128 + 0 /* 9 */,
	128 + 0 /* 10 */,
	128 + 0 /* 11 */,
	128 + 0 /* 12 */,
	128 + 0 /* 13 */,
	128 + 0 /* 14 */,
	128 + 1 /* 15 */,
	128 + 1 /* 16 */,
	128 + 1 /* 17 */,
	128 + 2 /* 18/00-18/09 */,
	128 + 2 /* 18/10-18/19 */,
	128 + 2 /* 18/20-18/29 */,
	128 + 2 /* 18/30-18/39 */,
	128 + 2 /* 18/40-18/49 */,
	128 + 3 /* 18/50-18/59 */,
	128 + 3 /* 18/60-18/69 */,
	128 + 3 /* 18/70-18/79 */,
	128 + 4 /* 18/80-18/89 */,
	128 + 5 /* 18/90-18/99 */,
	128 + 6 /* 18/100-18/109 */,
	128 + 7 /* 18/110-18/119 */,
	128 + 8 /* 18/120-18/129 */,
	128 + 9 /* 18/130-18/139 */,
	128 + 9 /* 18/140-18/149 */,
	128 + 10        /* 18/150-18/159 */,
	128 + 11        /* 18/160-18/169 */,
	128 + 12        /* 18/170-18/179 */,
	128 + 13        /* 18/180-18/189 */,
	128 + 14        /* 18/190-18/199 */,
	128 + 15        /* 18/200-18/209 */,
	128 + 15        /* 18/210-18/219 */,
	128 + 16        /* 18/220+ */
};


/*
 * Stat Table (STR) -- bonus to dam (plus 128)
 */
byte adj_str_td[] =
{
	128 + -2        /* 3 */,
	128 + -2        /* 4 */,
	128 + -1        /* 5 */,
	128 + -1        /* 6 */,
	128 + 0 /* 7 */,
	128 + 0 /* 8 */,
	128 + 0 /* 9 */,
	128 + 0 /* 10 */,
	128 + 0 /* 11 */,
	128 + 0 /* 12 */,
	128 + 0 /* 13 */,
	128 + 0 /* 14 */,
	128 + 0 /* 15 */,
	128 + 1 /* 16 */,
	128 + 2 /* 17 */,
	128 + 2 /* 18/00-18/09 */,
	128 + 2 /* 18/10-18/19 */,
	128 + 3 /* 18/20-18/29 */,
	128 + 3 /* 18/30-18/39 */,
	128 + 3 /* 18/40-18/49 */,
	128 + 3 /* 18/50-18/59 */,
	128 + 3 /* 18/60-18/69 */,
	128 + 4 /* 18/70-18/79 */,
	128 + 5 /* 18/80-18/89 */,
	128 + 5 /* 18/90-18/99 */,
	128 + 6 /* 18/100-18/109 */,
	128 + 7 /* 18/110-18/119 */,
	128 + 8 /* 18/120-18/129 */,
	128 + 9 /* 18/130-18/139 */,
	128 + 10        /* 18/140-18/149 */,
	128 + 11        /* 18/150-18/159 */,
	128 + 12        /* 18/160-18/169 */,
	128 + 13        /* 18/170-18/179 */,
	128 + 14        /* 18/180-18/189 */,
	128 + 15        /* 18/190-18/199 */,
	128 + 16        /* 18/200-18/209 */,
	128 + 18        /* 18/210-18/219 */,
	128 + 20        /* 18/220+ */
};


/*
 * Stat Table (DEX) -- bonus to hit (plus 128)
 */
byte adj_dex_th[] =
{
	128 + -3        /* 3 */,
	128 + -2        /* 4 */,
	128 + -2        /* 5 */,
	128 + -1        /* 6 */,
	128 + -1        /* 7 */,
	128 + 0 /* 8 */,
	128 + 0 /* 9 */,
	128 + 0 /* 10 */,
	128 + 0 /* 11 */,
	128 + 0 /* 12 */,
	128 + 0 /* 13 */,
	128 + 0 /* 14 */,
	128 + 0 /* 15 */,
	128 + 1 /* 16 */,
	128 + 2 /* 17 */,
	128 + 3 /* 18/00-18/09 */,
	128 + 3 /* 18/10-18/19 */,
	128 + 3 /* 18/20-18/29 */,
	128 + 3 /* 18/30-18/39 */,
	128 + 3 /* 18/40-18/49 */,
	128 + 4 /* 18/50-18/59 */,
	128 + 4 /* 18/60-18/69 */,
	128 + 4 /* 18/70-18/79 */,
	128 + 4 /* 18/80-18/89 */,
	128 + 5 /* 18/90-18/99 */,
	128 + 6 /* 18/100-18/109 */,
	128 + 7 /* 18/110-18/119 */,
	128 + 8 /* 18/120-18/129 */,
	128 + 9 /* 18/130-18/139 */,
	128 + 9 /* 18/140-18/149 */,
	128 + 10        /* 18/150-18/159 */,
	128 + 11        /* 18/160-18/169 */,
	128 + 12        /* 18/170-18/179 */,
	128 + 13        /* 18/180-18/189 */,
	128 + 14        /* 18/190-18/199 */,
	128 + 15        /* 18/200-18/209 */,
	128 + 15        /* 18/210-18/219 */,
	128 + 16        /* 18/220+ */
};


/*
 * Stat Table (STR) -- bonus to hit (plus 128)
 */
byte adj_str_th[] =
{
	128 + -3        /* 3 */,
	128 + -2        /* 4 */,
	128 + -1        /* 5 */,
	128 + -1        /* 6 */,
	128 + 0 /* 7 */,
	128 + 0 /* 8 */,
	128 + 0 /* 9 */,
	128 + 0 /* 10 */,
	128 + 0 /* 11 */,
	128 + 0 /* 12 */,
	128 + 0 /* 13 */,
	128 + 0 /* 14 */,
	128 + 0 /* 15 */,
	128 + 0 /* 16 */,
	128 + 0 /* 17 */,
	128 + 1 /* 18/00-18/09 */,
	128 + 1 /* 18/10-18/19 */,
	128 + 1 /* 18/20-18/29 */,
	128 + 1 /* 18/30-18/39 */,
	128 + 1 /* 18/40-18/49 */,
	128 + 1 /* 18/50-18/59 */,
	128 + 1 /* 18/60-18/69 */,
	128 + 2 /* 18/70-18/79 */,
	128 + 3 /* 18/80-18/89 */,
	128 + 4 /* 18/90-18/99 */,
	128 + 5 /* 18/100-18/109 */,
	128 + 6 /* 18/110-18/119 */,
	128 + 7 /* 18/120-18/129 */,
	128 + 8 /* 18/130-18/139 */,
	128 + 9 /* 18/140-18/149 */,
	128 + 10        /* 18/150-18/159 */,
	128 + 11        /* 18/160-18/169 */,
	128 + 12        /* 18/170-18/179 */,
	128 + 13        /* 18/180-18/189 */,
	128 + 14        /* 18/190-18/199 */,
	128 + 15        /* 18/200-18/209 */,
	128 + 15        /* 18/210-18/219 */,
	128 + 16        /* 18/220+ */
};


/*
 * Stat Table (STR) -- weight limit in deca-pounds
 */
byte adj_str_wgt[] =
{
	5       /* 3 */,
	6       /* 4 */,
	7       /* 5 */,
	8       /* 6 */,
	9       /* 7 */,
	10      /* 8 */,
	11      /* 9 */,
	12      /* 10 */,
	13      /* 11 */,
	14      /* 12 */,
	15      /* 13 */,
	16      /* 14 */,
	17      /* 15 */,
	18      /* 16 */,
	19      /* 17 */,
	20      /* 18/00-18/09 */,
	22      /* 18/10-18/19 */,
	24      /* 18/20-18/29 */,
	26      /* 18/30-18/39 */,
	28      /* 18/40-18/49 */,
	30      /* 18/50-18/59 */,
	31      /* 18/60-18/69 */,
	31      /* 18/70-18/79 */,
	32      /* 18/80-18/89 */,
	32      /* 18/90-18/99 */,
	33      /* 18/100-18/109 */,
	33      /* 18/110-18/119 */,
	34      /* 18/120-18/129 */,
	34      /* 18/130-18/139 */,
	35      /* 18/140-18/149 */,
	35      /* 18/150-18/159 */,
	36      /* 18/160-18/169 */,
	36      /* 18/170-18/179 */,
	37      /* 18/180-18/189 */,
	37      /* 18/190-18/199 */,
	38      /* 18/200-18/209 */,
	38      /* 18/210-18/219 */,
	39      /* 18/220+ */
};


/*
 * Stat Table (STR) -- weapon weight limit in pounds
 */
byte adj_str_hold[] =
{
	4       /* 3 */,
	5       /* 4 */,
	6       /* 5 */,
	7       /* 6 */,
	8       /* 7 */,
	10      /* 8 */,
	12      /* 9 */,
	14      /* 10 */,
	16      /* 11 */,
	18      /* 12 */,
	20      /* 13 */,
	22      /* 14 */,
	24      /* 15 */,
	26      /* 16 */,
	28      /* 17 */,
	30      /* 18/00-18/09 */,
	30      /* 18/10-18/19 */,
	35      /* 18/20-18/29 */,
	40      /* 18/30-18/39 */,
	45      /* 18/40-18/49 */,
	50      /* 18/50-18/59 */,
	55      /* 18/60-18/69 */,
	60      /* 18/70-18/79 */,
	65      /* 18/80-18/89 */,
	70      /* 18/90-18/99 */,
	80      /* 18/100-18/109 */,
	80      /* 18/110-18/119 */,
	80      /* 18/120-18/129 */,
	80      /* 18/130-18/139 */,
	80      /* 18/140-18/149 */,
	90      /* 18/150-18/159 */,
	90      /* 18/160-18/169 */,
	90      /* 18/170-18/179 */,
	90      /* 18/180-18/189 */,
	90      /* 18/190-18/199 */,
	100     /* 18/200-18/209 */,
	100     /* 18/210-18/219 */,
	100     /* 18/220+ */
};


/*
 * Stat Table (STR) -- digging value
 */
byte adj_str_dig[] =
{
	0       /* 3 */,
	0       /* 4 */,
	1       /* 5 */,
	2       /* 6 */,
	3       /* 7 */,
	4       /* 8 */,
	4       /* 9 */,
	5       /* 10 */,
	5       /* 11 */,
	6       /* 12 */,
	6       /* 13 */,
	7       /* 14 */,
	7       /* 15 */,
	8       /* 16 */,
	8       /* 17 */,
	9       /* 18/00-18/09 */,
	10      /* 18/10-18/19 */,
	12      /* 18/20-18/29 */,
	15      /* 18/30-18/39 */,
	20      /* 18/40-18/49 */,
	25      /* 18/50-18/59 */,
	30      /* 18/60-18/69 */,
	35      /* 18/70-18/79 */,
	40      /* 18/80-18/89 */,
	45      /* 18/90-18/99 */,
	50      /* 18/100-18/109 */,
	55      /* 18/110-18/119 */,
	60      /* 18/120-18/129 */,
	65      /* 18/130-18/139 */,
	70      /* 18/140-18/149 */,
	75      /* 18/150-18/159 */,
	80      /* 18/160-18/169 */,
	85      /* 18/170-18/179 */,
	90      /* 18/180-18/189 */,
	95      /* 18/190-18/199 */,
	100     /* 18/200-18/209 */,
	100     /* 18/210-18/219 */,
	100     /* 18/220+ */
};


/*
 * Stat Table (STR) -- help index into the "blow" table
 */
byte adj_str_blow[] =
{
	3       /* 3 */,
	4       /* 4 */,
	5       /* 5 */,
	6       /* 6 */,
	7       /* 7 */,
	8       /* 8 */,
	9       /* 9 */,
	10      /* 10 */,
	11      /* 11 */,
	12      /* 12 */,
	13      /* 13 */,
	14      /* 14 */,
	15      /* 15 */,
	16      /* 16 */,
	17      /* 17 */,
	20 /* 18/00-18/09 */,
	30 /* 18/10-18/19 */,
	40 /* 18/20-18/29 */,
	50 /* 18/30-18/39 */,
	60 /* 18/40-18/49 */,
	70 /* 18/50-18/59 */,
	80 /* 18/60-18/69 */,
	90 /* 18/70-18/79 */,
	100 /* 18/80-18/89 */,
	110 /* 18/90-18/99 */,
	120 /* 18/100-18/109 */,
	130 /* 18/110-18/119 */,
	140 /* 18/120-18/129 */,
	150 /* 18/130-18/139 */,
	160 /* 18/140-18/149 */,
	170 /* 18/150-18/159 */,
	180 /* 18/160-18/169 */,
	190 /* 18/170-18/179 */,
	200 /* 18/180-18/189 */,
	210 /* 18/190-18/199 */,
	220 /* 18/200-18/209 */,
	230 /* 18/210-18/219 */,
	240 /* 18/220+ */
};


/*
 * Stat Table (DEX) -- index into the "blow" table
 */
byte adj_dex_blow[] =
{
	0       /* 3 */,
	0       /* 4 */,
	0       /* 5 */,
	0       /* 6 */,
	0       /* 7 */,
	0       /* 8 */,
	0       /* 9 */,
	1       /* 10 */,
	1       /* 11 */,
	1       /* 12 */,
	1       /* 13 */,
	1       /* 14 */,
	1       /* 15 */,
	1       /* 16 */,
	1       /* 17 */,
	1       /* 18/00-18/09 */,
	2       /* 18/10-18/19 */,
	2       /* 18/20-18/29 */,
	2       /* 18/30-18/39 */,
	2       /* 18/40-18/49 */,
	3       /* 18/50-18/59 */,
	3       /* 18/60-18/69 */,
	4       /* 18/70-18/79 */,
	4       /* 18/80-18/89 */,
	5       /* 18/90-18/99 */,
	6       /* 18/100-18/109 */,
	7       /* 18/110-18/119 */,
	8       /* 18/120-18/129 */,
	9       /* 18/130-18/139 */,
	10      /* 18/140-18/149 */,
	11      /* 18/150-18/159 */,
	12      /* 18/160-18/169 */,
	14      /* 18/170-18/179 */,
	16      /* 18/180-18/189 */,
	18      /* 18/190-18/199 */,
	20      /* 18/200-18/209 */,
	20      /* 18/210-18/219 */,
	20      /* 18/220+ */
};


/*
 * Stat Table (DEX) -- chance of avoiding "theft" and "falling"
 */
byte adj_dex_safe[] =
{
	0       /* 3 */,
	1       /* 4 */,
	2       /* 5 */,
	3       /* 6 */,
	4       /* 7 */,
	5       /* 8 */,
	5       /* 9 */,
	6       /* 10 */,
	6       /* 11 */,
	7       /* 12 */,
	7       /* 13 */,
	8       /* 14 */,
	8       /* 15 */,
	9       /* 16 */,
	9       /* 17 */,
	10      /* 18/00-18/09 */,
	10      /* 18/10-18/19 */,
	15      /* 18/20-18/29 */,
	15      /* 18/30-18/39 */,
	20      /* 18/40-18/49 */,
	25      /* 18/50-18/59 */,
	30      /* 18/60-18/69 */,
	35      /* 18/70-18/79 */,
	40      /* 18/80-18/89 */,
	45      /* 18/90-18/99 */,
	50      /* 18/100-18/109 */,
	60      /* 18/110-18/119 */,
	70      /* 18/120-18/129 */,
	80      /* 18/130-18/139 */,
	90      /* 18/140-18/149 */,
	100     /* 18/150-18/159 */,
	100     /* 18/160-18/169 */,
	100     /* 18/170-18/179 */,
	100     /* 18/180-18/189 */,
	100     /* 18/190-18/199 */,
	100     /* 18/200-18/209 */,
	100     /* 18/210-18/219 */,
	100     /* 18/220+ */
};


/*
 * Stat Table (CON) -- base regeneration rate
 */
byte adj_con_fix[] =
{
	0       /* 3 */,
	0       /* 4 */,
	0       /* 5 */,
	0       /* 6 */,
	0       /* 7 */,
	0       /* 8 */,
	0       /* 9 */,
	0       /* 10 */,
	0       /* 11 */,
	0       /* 12 */,
	0       /* 13 */,
	1       /* 14 */,
	1       /* 15 */,
	1       /* 16 */,
	1       /* 17 */,
	2       /* 18/00-18/09 */,
	2       /* 18/10-18/19 */,
	2       /* 18/20-18/29 */,
	2       /* 18/30-18/39 */,
	2       /* 18/40-18/49 */,
	3       /* 18/50-18/59 */,
	3       /* 18/60-18/69 */,
	3       /* 18/70-18/79 */,
	3       /* 18/80-18/89 */,
	3       /* 18/90-18/99 */,
	4       /* 18/100-18/109 */,
	4       /* 18/110-18/119 */,
	5       /* 18/120-18/129 */,
	6       /* 18/130-18/139 */,
	6       /* 18/140-18/149 */,
	7       /* 18/150-18/159 */,
	7       /* 18/160-18/169 */,
	8       /* 18/170-18/179 */,
	8       /* 18/180-18/189 */,
	8       /* 18/190-18/199 */,
	9       /* 18/200-18/209 */,
	9       /* 18/210-18/219 */,
	9       /* 18/220+ */
};


/*
 * Stat Table (CON) -- extra half-hitpoints per level (plus 128)
 */
byte adj_con_mhp[] =
{
	128 + -5        /* 3 */,
	128 + -3        /* 4 */,
	128 + -2        /* 5 */,
	128 + -1        /* 6 */,
	128 + 0 /* 7 */,
	128 + 0 /* 8 */,
	128 + 0 /* 9 */,
	128 + 0 /* 10 */,
	128 + 0 /* 11 */,
	128 + 0 /* 12 */,
	128 + 0 /* 13 */,
	128 + 0 /* 14 */,
	128 + 1 /* 15 */,
	128 + 1 /* 16 */,
	128 + 2 /* 17 */,
	128 + 3 /* 18/00-18/09 */,
	128 + 4 /* 18/10-18/19 */,
	128 + 4 /* 18/20-18/29 */,
	128 + 4 /* 18/30-18/39 */,
	128 + 4 /* 18/40-18/49 */,
	128 + 5 /* 18/50-18/59 */,
	128 + 6 /* 18/60-18/69 */,
	128 + 7 /* 18/70-18/79 */,
	128 + 8 /* 18/80-18/89 */,
	128 + 9 /* 18/90-18/99 */,
	128 + 10        /* 18/100-18/109 */,
	128 + 11        /* 18/110-18/119 */,
	128 + 12        /* 18/120-18/129 */,
	128 + 13        /* 18/130-18/139 */,
	128 + 14        /* 18/140-18/149 */,
	128 + 15        /* 18/150-18/159 */,
	128 + 16        /* 18/160-18/169 */,
	128 + 18        /* 18/170-18/179 */,
	128 + 20        /* 18/180-18/189 */,
	128 + 22        /* 18/190-18/199 */,
	128 + 25        /* 18/200-18/209 */,
	128 + 26        /* 18/210-18/219 */,
	128 + 27        /* 18/220+ */
};


/*
 * This table is used to help calculate the number of blows the player can
 * make in a single round of attacks (one player turn) with a normal weapon.
 *
 * This number ranges from a single blow/round for weak players to up to six
 * blows/round for powerful warriors.
 *
 * Note that certain artifacts and ego-items give "bonus" blows/round.
 *
 * First, from the player class, we extract some values:
 *
 *    Warrior --> num = 6; mul = 5; div = MAX(30, weapon_weight);
 *    Mage    --> num = 4; mul = 2; div = MAX(40, weapon_weight);
 *    Priest  --> num = 5; mul = 3; div = MAX(35, weapon_weight);
 *    Rogue   --> num = 5; mul = 3; div = MAX(30, weapon_weight);
 *    Ranger  --> num = 5; mul = 4; div = MAX(35, weapon_weight);
 *    Paladin --> num = 5; mul = 4; div = MAX(30, weapon_weight);
 *
 * To get "P", we look up the relevant "adj_str_blow[]" (see above),
 * multiply it by "mul", and then divide it by "div", rounding down.
 *
 * To get "D", we look up the relevant "adj_dex_blow[]" (see above),
 * note especially column 6 (DEX 18/101) and 11 (DEX 18/150).
 *
 * The player gets "blows_table[P][D]" blows/round, as shown below,
 * up to a maximum of "num" blows/round, plus any "bonus" blows/round.
 */
byte blows_table[12][12] =
{
	/* P/D */
	/* 0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11+ */

	/* 0  */
	{  1,   1,   1,   1,   1,   1,   2,   2,   2,   2,   2,   3 },

	/* 1  */
	{  1,   1,   1,   1,   2,   2,   3,   3,   3,   4,   4,   4 },

	/* 2  */
	{  1,   1,   2,   2,   3,   3,   4,   4,   4,   5,   5,   5 },

	/* 3  */
	{  1,   2,   2,   3,   3,   4,   4,   4,   5,   5,   5,   5 },

	/* 4  */
	{  1,   2,   2,   3,   3,   4,   4,   5,   5,   5,   5,   5 },

	/* 5  */
	{  2,   2,   3,   3,   4,   4,   5,   5,   5,   5,   5,   6 },

	/* 6  */
	{  2,   2,   3,   3,   4,   4,   5,   5,   5,   5,   5,   6 },

	/* 7  */
	{  2,   3,   3,   4,   4,   4,   5,   5,   5,   5,   5,   6 },

	/* 8  */
	{  3,   3,   3,   4,   4,   4,   5,   5,   5,   5,   6,   6 },

	/* 9  */
	{  3,   3,   4,   4,   4,   4,   5,   5,   5,   5,   6,   6 },

	/* 10 */
	{  3,   3,   4,   4,   4,   4,   5,   5,   5,   6,   6,   6 },

	/* 11+ */
	{  3,   3,   4,   4,   4,   4,   5,   5,   6,   6,   6,   6 },
};


s16b arena_monsters[MAX_ARENA_MONS] =
{
	 30,	 43,	102,	118,	126,	149,	173,
	183,	188,	191,	216,	230,	238,	244,
	255,	262,	293,	297,	321,	349,	372,
	401,	415,	454,	464,	485,	538,	631,
	641
};


/*
 * This table allows quick conversion from "speed" to "energy"
 * The basic function WAS ((S>=110) ? (S-110) : (100 / (120-S)))
 * Note that table access is *much* quicker than computation.
 *
 * Note that the table has been changed at high speeds.  From
 * "Slow (-40)" to "Fast (+30)" is pretty much unchanged, but
 * at speeds above "Fast (+30)", one approaches an asymptotic
 * effective limit of 50 energy per turn.  This means that it
 * is relatively easy to reach "Fast (+30)" and get about 40
 * energy per turn, but then speed becomes very "expensive",
 * and you must get all the way to "Fast (+50)" to reach the
 * point of getting 45 energy per turn.  After that point,
 * further increases in speed are more or less pointless,
 * except to balance out heavy inventory.
 *
 * Note that currently the fastest monster is "Fast (+30)".
 *
 * It should be possible to lower the energy threshold from
 * 100 units to 50 units, though this may interact badly with
 * the (compiled out) small random energy boost code.  It may
 * also tend to cause more "clumping" at high speeds.
 */
byte extract_energy[300] =
{
	/* Slow */     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	/* Slow */     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	/* Slow */     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	/* Slow */     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	/* Slow */     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	/* Slow */     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	/* S-50 */     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	/* S-40 */     2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
	/* S-30 */     2,  2,  2,  2,  2,  2,  2,  3,  3,  3,
	/* S-20 */     3,  3,  3,  3,  3,  4,  4,  4,  4,  4,
	/* S-10 */     5,  5,  5,  5,  6,  6,  7,  7,  8,  9,
	/* Norm */    10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
	/* F+10 */    20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
	/* F+20 */    30, 31, 32, 33, 34, 35, 36, 36, 37, 37,
	/* F+30 */    38, 38, 39, 39, 40, 40, 40, 41, 41, 41,
	/* F+40 */    42, 42, 42, 43, 43, 43, 44, 44, 44, 44,
	/* F+50 */    45, 45, 45, 45, 45, 46, 46, 46, 46, 46,
	/* F+60 */    47, 47, 47, 47, 47, 48, 48, 48, 48, 48,
	/* F+70 */    49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
	/* Fast */    49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
	/* Virtual */  49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
	/* Virtual */  49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
	/* Virtual */  49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
	/* Virtual */  49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
	/* Virtual */  49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
	/* Virtual */  49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
	/* Virtual */  49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
	/* Virtual */  49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
	/* Virtual */  49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
	/* Virtual */  49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
};




/*
 * Base experience levels, may be adjusted up for race and/or class
 */
s32b player_exp[PY_MAX_LEVEL] =
{
	10,
	25,
	45,
	70,
	100,
	140,
	200,
	280,
	380,
	500,
	650,
	850,
	1100,
	1400,
	1800,
	2300,
	2900,
	3600,
	4400,
	5400,
	6800,
	8400,
	10200,
	12500,
	17500,
	25000,
	35000L,
	50000L,
	75000L,
	100000L,
	150000L,
	200000L,
	275000L,
	350000L,
	450000L,
	550000L,
	700000L,
	850000L,
	1000000L,
	1250000L,
	1500000L,
	1800000L,
	2100000L,
	2400000L,
	2700000L,
	3000000L,
	3500000L,
	4000000L,
	4500000L,
	5000000L
};


/*
 * Player Sexes
 *
 *      Title,
 *      Winner
 */
player_sex sex_info[MAX_SEXES] =
{
	{
		"Female",
		"Queen"
	},

	{
		"Male",
		"King"
	},
	{
		"Neuter",
		"Ruler"
	}
};


/*
 * Info on the spells/realms
 */
magic_type realm_info[MAX_REALM][64] =
{
	/* Nothing */
	{
		{ 0 }
	},
	/* Symbiotic Magic */
	{
		{  1,  2, 25,   4},
		{  3,  3, 30,   5},
		{  6,  7, 35,   7},
		{  9, 10, 40,  10},
		{ 12, 15, 50,  15},
		{ 16, 17, 55,  20},
		{ 20, 26, 40,  15},
		{ 99,  0,  0,   0},

		{ 99,  0,  0,   0},
		{ 99,  0,  0,   0},
		{ 99,  0,  0,   0},
		{ 99,  0,  0,   0},
		{ 99,  0,  0,   0},
		{ 99,  0,  0,   0},
		{ 99,  0,  0,   0},
		{ 99,  0,  0,   0},

		{ 24, 30, 60,  36},
		{ 26, 35, 60,  30},
		{ 30, 40, 60,  40},
		{ 35, 43, 63,  50},
		{ 36, 90, 40, 100},
		{ 50,100, 50, 200},
		{ 99,  0,  0,   0},
		{ 99,  0,  0,   0},

		{ 99,  0,  0,   0},
		{ 99,  0,  0,   0},
		{ 99,  0,  0,   0},
		{ 99,  0,  0,   0},
		{ 99,  0,  0,   0},
		{ 99,  0,  0,   0},
		{ 99,  0,  0,   0},
		{ 99,  0,  0,   0}
	},

	/* Harper Music */
	{
		{ 1,  1,  10,   2},
		{ 2,  0,   0,   0},
		{ 3,  2,  20,   3},
		{ 4,  3,  20,   4},
		{ 5,  3,  20,   6},
		{ 7,  3,  30,   8},
		{ 9,  3,  30,   10},
		{ 10,  3,  30,   12},

		{ 12,  5,   40,   20},
		{ 16,  10,  40,   25},
		{ 18,  16,  45,   30},
		{ 20,  20,  50,   35},
		{ 23,  3,  50,   38},
		{ 24,  4,   50,   41},
		{ 26,  8,  60,   42},
		{ 27,  9,  70,   46},

		{ 10, 100,  20,   13},
		{ 22,  23,  30,   26},
		{ 23,  18,  35,   27},
		{ 26,  12,  37,   29},
		{ 32,  30,  41,   36},
		{ 33,  18,  43,   40},
		{ 38,  50,  46,   42},
		{ 45,  60,  50,   56},

		{ 23,   6,  20,   23},
		{ 26,  25,  30,   26},
		{ 29,   8,  41,   30},
		{ 34,  16,  43,   35},
		{ 35,  40,  46,   50},
		{ 40,  20,  50,   68},
		{ 42,  20,  62,   73},
		{ 50,  20,  70,  200}
	},
	/* Druidistic */
	{
		{ 1, 1, 10,  4},
		{ 2, 0, 15,  6},
		{ 4, 4, 18,  8},
		{ 5, 2, 20, 10},
		{ 6, 8, 24, 13},
		{ 8, 9, 26, 16},
		{ 9, 10, 28, 20},
		{ 10, 12, 34, 25},

		{ 11, 11, 30, 30},
		{ 12, 10, 35, 38},
		{ 14, 14, 38, 42},
		{ 15, 12, 40, 46},
		{ 16, 18, 45, 49},
		{ 18, 19, 46, 55},
		{ 19, 20, 48, 58},
		{ 20, 22, 54, 62},

		{ 21,  8, 57, 68},
		{ 22, 26, 60, 75},
		{ 24, 26, 62, 78},
		{ 25, 30, 65, 82},
		{ 26, 35, 68, 85},
		{ 28, 38, 70, 90},
		{ 29, 41, 73, 93},
		{ 30, 43, 75, 96},

		{ 31, 45, 80, 100},
		{ 32, 46, 82, 103},
		{ 34, 50, 83, 110},
		{ 35, 51, 85, 115},
		{ 36, 53, 86, 119},
		{ 38, 60, 89, 123},
		{ 39, 65, 90, 128},
		{ 40, 80, 91, 130},

		{ 41, 80, 92, 130},
		{ 42, 86, 93, 135},
		{ 44, 20, 94, 145},
		{ 45, 90, 95, 165},
		{ 46,  2, 95, 180},
		{ 48, 100, 95, 200},
		{ 49, 105, 95, 210},
		{ 50, 255, 95, 220},
	},

	/* Daemon -SCSCSC- */
	{
		{ 1, 1, 10, 4},
		{ 2, 2, 12, 4},
		{ 3, 4, 16, 6},
		{ 3, 7, 20, 8},
		{ 4, 4, 22, 8},
		{ 5, 8, 25, 8},
		{ 7, 9, 28, 10},
		{ 12, 10, 32, 15},

		{ 3, 2, 15, 6},
		{ 5, 6, 18, 8},
		{ 6, 6, 20, 10},
		{ 10, 14, 30, 12},
		{ 12, 8, 34, 14},
		{ 16, 12, 36, 16},
		{ 18, 16, 40, 18},
		{ 20, 18, 50, 19},

		{ 20, 25, 55, 45},
		{ 25, 20, 60, 52},
		{ 28, 25, 64, 55},
		{ 32, 44, 68, 59},
		{ 38, 35, 72, 65},
		{ 44, 75, 85, 80},
		{ 22, 18, 50, 50},
		{ 25, 25, 60, 65},

		{ 26, 18, 55, 55},
		{ 30, 28, 63, 67},
		{ 38, 34, 70, 75},
		{ 45, 72, 85, 110},
		{ 40, 35, 90, 120},
		{ 42, 58, 92, 130},
		{ 45, 40, 94, 140},
		{ 48, 60, 95, 150},
	},
};


/*
 * PernAngband uses this array instead of the spell flags table -- DG
 */
u32b fake_spell_flags[MAX_REALM][9][2]=
{
	{
		{0 }
	},

	{
		{0x00000000, 0x000000ff},
		{0x00000000, 0x0000ff00},
		{0x00000000, 0x00ff0000},
		{0x00000000, 0xff000000},
	},

	{
		{0x00000000, 0x000000ff},
		{0x00000000, 0x0000ff00},
		{0x00000000, 0x00ff0000},
		{0x00000000, 0xff000000},
	},
	/* Druidistic */
	{
		{0x00000000, 0x000000ff},
		{0x00000000, 0x0000ff00},
		{0x00000000, 0x00ff0000},
		{0x00000000, 0xff000000},
		{0x000000ff, 0x00000000},
	},

	/* Daemon */
	{
		{0x00000000, 0x000000ff},
		{0x00000000, 0x0000ff00},
		{0x00000000, 0x003f0000},
		{0x00000000, 0x0fc00000},
		{0x00000000, 0xf0000000},
	},
};

cptr realm_names[][2] =
{
	{ "no magic", "" },
	{ "Symbiotic", "Specific realm.\n" },
	{ "Music", "Specific realm.\n" },
	{ "Druidic", "Specific realm.\n" },
	{ "Daemon", "Specific realm.\n" },
};


/*
 * Names of the spells
 */
cptr spell_names[MAX_REALM][64][2] =
{
	/*** Nothing ***/
	{
		{NULL, NULL},
	},

	/* Symbiotic Spellbooks */
	{
		/* Common Symbiotic Spellbooks */
		{
			"Minor Symbiotic Healing",
			"Heals your currently equipped monster a little"
		},
		{
			"Tangled Creepers",
			"Slows creatures are around you"
		},
		{
			"Vampiric healing",
			"Steals life force other creatures and pass it to your pet"
		},
		{
			"Life Transfer",
			"Transfers from your life-force to your pet's"
		},
		{
			"Satisfy Hunger",
			"Magically feeds you"
		},
		{
			"Minor Symbiotic Powers",
			"Allows you to use a lesser ability of your pet"
		},
		{
			"Summon Never-moving Pet",
			"Summons a charmed creature from the plane of unmoving"
		},
		{"", ""},

		{"", ""},
		{"", ""},
		{"", ""},
		{"", ""},
		{"", ""},
		{"", ""},
		{"", ""},
		{"", ""},

		/* Rare Symbiotic Spellbooks */
		{
			"Mana Healing",
			"Replaces your pet's lost life-force with magical energy"
		},
		{
			"Summon Never-moving Pets",
			"Summons many pets from the plane of unmoving"
		},
		{
			"Major Symbiotic Healing",
			"Heals your currently equipped monster"
		},
		{
			"Healing",
			"Uses magic energy to heal you"
		},
		{
			"Major Symbiotic Powers",
			"Allows you to use an ability of your monster"
		},
		{
			"Use Enemy's Powers",
			"Allows you to turn the powers of your enemies against them"
		},
		{"", ""},
		{"", ""},


		{"", ""},
		{"", ""},
		{"", ""},
		{"", ""},
		{"", ""},
		{"", ""},
		{"", ""},
		{"", ""},
	},
	{
		/******* Apprentice Handbook *******/
		{
			"Song of Holding",
			"Slows down all monsters listening the song."
		},
		{
			"Stop singing",
			"Stops your current song"
		},
		{
			"Horrible Note",
			"You scream a directed beam of sound."
		},
		{
			"Stun Pattern",
			"Stuns all monsters listening the song."
		},
		{
			"Flow of Life",
			"Heals you as long as you sing."
		},
		{
			"Clairaudience",
			"Provides telepathy as long as you sing."
		},
		{
			"Song of the Sun",
			"Provides light as long as you sing."
		},
		{
			"Heroic Ballad",
			"Increases your fighting abilities as long as you sing."
		},

		/******* Minstrel's Music *******/
		{
			"Song of Awareness",
			"Detect magic items as long as you sing."
		},
		{
			"Song of Lore",
			"Identify objects near you on the ground as long as you sing."
		},
		{
			"Spatial Transfer",
			"Keeps on teleporting you as long as you sing."
		},
		{
			"Doomcall",
			"This note will wreck your foes."
		},
		{
			"Illusion Pattern",
			"Tries to confuse all monsters listening the song."
		},
		{
			"Wrecking Pattern",
			"Destroy walls around you as long as you sing."
		},
		{
			"Finrod's Resistance",
			"Provides resistance to the elements as long as you sing."
		},
		{
			"Hobbit Melodies",
			"Increases your speed as long as you sing."
		},

		/******* Harps of Rivendell *******/
		{
			"Song of Probing",
			"Learn everything about a monster."
		},
		{
			"World Contortion",
			"Teleports away all your foes."
		},
		{
			"Fellowship Chant",
			"Fire of beam of calming sounds, that can charm monsters..."
		},
		{
			"Song of the Tempest",
			"Creates a beam of pure sound."
		},
		{
			"Telchar's Forge",
			"Imbues the power of the sound into a weapon."
		},
		{
			"Akallabeth",
			"All monsters that hear the song get hit by pure sound."
		},
		{
			"Ambarkanta",
			"As you sing, the level is reshaped."
		},
		{
			"Lay of Gil-Galad",
			"Summons allies to your help."
		},

		/******* Lays of Beleriand *******/
		{
			"Felagund's Disguise",
			"Renders you invisible as long as you sing."
		},
		{
			"Wizardry of Sauron",
			"Fires a powerful bolt of sound."
		},
		{
			"The Voice of Saruman",
			"Charms all monsters that hear the song."
		},
		{
			"Ainulindale",
			"Provides Ainur-like powers as long as you sing."
		},
		{
			"The Horns of Ylmir",
			"Destroys the area around you."
		},
		{
			"Fingolfin's Challenge",
			"Gives you invulnerability as long as you sing."
		},
		{
			"Firiel's Song",
			"Calls upon the dead to your help."
		},
		{
			"Luthien's Mourning",
			"Turns you into a wraith being as long as you sing."
		},

	},
	/* Druidistic */
	{
		/* Elemental Stone[Earth] */
		{
			"Tunnel",
			"Creates a burrow in the ground, though it's exit is randomly placed."
		},
		{
			"Canalize Mana",
			"Drains the soil of it's energy and gives it to you."
		},
		{
			"Acid Bolt",
			"Creates a jet of acid which is sprayed on an enemy."
		},
		{
			"Mana Path",
			"Imbues your feet with magic energy, infusing the earth with magic with each step."
		},
		{
			"Forest Generation",
			"Speeds the growth of trees around you, to the point that they grow in mere seconds."
		},
		{
			"Druidic Acid Beam",
			"A beam of acid which follows paths of magic."
		},
		{
			"Raise Mountains",
			"Raises to the soil around you to the full glory of a mountain."
		},
		{
			"Stone Skin",
			"Hardens your skin so it's as hard as stone."
		},

		/* Elemental Stone[Fire] */
		{
			"Infravision",
			"Enables you to see heat patterns"
		},
		{
			"Fire Bolt",
			"Creates a flaming bolt which rushes to it's target"
		},
		{
			"Fire Ball",
			"A small globe of fire that explodes into a ball of flame at the chosen place."
		},
		{
			"Uncover Traps",
			"Makes any trap in the area clear to the eye."
		},
		{
			"Fire Beam",
			"A scorching beam of fire."
		},
		{
			"Druidic Fire Bolt",
			"A bolt of blazing fire which follows paths of magic."
		},
		{
			"Druidic Fire Beam",
			"A beam which can be directed by paths of magic."
		},
		{
			"Create Lava",
			"Makes lava spurt randomly from the floor around you."
		},

		/* Elemental Stone[Air] */
		{
			"Winds of Mana",
			"Infuses the ground around you with magical signature."
		},
		{
			"Summon Air Elemental",
			"Summons an elemental from the plane of air, though beware, it might turn on you."
		},
		{
			"Whispers from Afar",
			"Contacts unknown nature beings, who know the nature of things."
		},
		{
			"The Winds of Manwe",
			"Powerful winds blow monsters away from you."
		},
		{
			"Bird View",
			"Maps the whole area, as if you could look at it from above."
		},
		{
			"*Whispers from Afar*",
			"Contacts unknown nature beings, who know the exact nature of things."
		},
		{
			"Windy Speed",
			"Imbues you with the quickness of the wind."
		},
		{
			"The Thunders of Manwe",
			"Sends a roaring beam of sound, as if countless lightnings just thundered, and it can be directed by mana signatures."
		},

		/* Elemental Stone[Water] */
		{
			"Summon Aquatic Golems",
			"Steals an aquatic golem from the dwelling place of the gnomes, though beware! These machines can sometimes know when they are stolen!"
		},
		{
			"Walk over Water",
			"Makes you weight less then a feather, thus enabling to walk over water"
		},
		{
			"Flood",
			"Floods the surrounding area with water."
		},
		{
			"Summon Water Elementals",
			"Summons an elemental from the plane of water."
		},
		{
			"Purification",
			"Heals you from harm"
		},
		{
			"Call of the Deep",
			"Allows you dive into water, and come out safely on any other water body."
		},
		{
			"Tidal Wave",
			"Creates a large wave at a point, which spreads in all directions, and leaves deep water spots afterwards."
		},
		{
			"Flood Level",
			"Powerful spell which reconstructs the level and floods it with water"
		},

		/* Elemental Stone[Mana] */
		{
			"Glyph of Warding",
			"Creates a powerful gylph on the floor which forbids any creature except it's maker from getting close to it."
		},
		{
			"Orb of Mana",
			"A powerful orb of mana explodes and causes havoc."
		},
		{
			"Gather Mana",
			"Not coded yet."
		},
		{
			"Mirror of Mana",
			"A barrier of mana surrounds you, which because of it's intensity it twists spells, and reflects all bolts."
		},
		{
			"Activate Rune of Mana",
			"Not coded yet."
		},
		{
			"Combine the 5 Elements",
			"Five destructive bolts, one of cold, one of fire, one of electricity, one of acid and one of pure mana shoot towards the target."
		},
		{
			"Shield of Mana",
			"Protects you from almost any harm by a strong field of mana."
		},
		{
			"Drain Level Mana",
			"Gathers mana from the whole area, into one single projectile of awesome strength"
		},
	},

	/* Demon -SCSCSC- */
	{
		/* Dark Incantations */
		{
			"Detect Good",
			"Detects all good beings near you."
		},
		{
			"Phase Door",
			"Teleport you a few squares."
		},
		{
			"Resist Fire",
			"Makes you resistant to fire for a short duration."
		},
		{
			"Unearthly Blessing",
			"Temporarily protects you and increases your fighting accuracy."
		},
		{
			"Steal Thoughts",
			"Attempts to confuse monsters."
		},
		{
			"Demon Eyes",
			"Allows you to detect all monsters for a short duration."
		},
		{
			"Mend Flesh",
			"Heals you."
		},
		{
			"Vision",
			"Reveals the level around you."
		},

		/* Immortal Rituals */
		{
			"Detect Angels and Demons",
			"Detects angels and demons near you."
		},
		{
			"Protection from Good",
			"Deflects attacks made by good beings."
		},
		{
			"Invisibility",
			"Turns you invisible."
		},
		{
			"Manes Summoning",
			"Summons an inferior demon to your aid."
		},
		{
			"Demoncloak",
			"Protects you from fire and cold and shields you."
		},
		{
			"Breath Fire",
			"Unleashes a powerful fire ball."
		},
		{
			"Fire Blade",
			"Hastes you, surrounds you with fire and makes you fight with berserk fury."
		},
		{
			"Circle of Madness",
			"Fires explosions of chaos, confusion and charm."
		},

		/* Minions of Azathoth */
		{
			"Bladecalm",
			"It needs to be coded :)"
		},
		{
			"Control Demons",
			"Attempts to charm demons."
		},
		{
			"Revive",
			"It will revive dead demons."
		},
		{
			"Trap Demonsoul",
			"Attempts to enslave a demon."
		},
		{
			"Discharge Minions",
			"Makes all your pets explode like powerful bombs."
		},
		{
			"Summon Demons",
			"Calls some demons to your aid."
		},

		/* Demonthoughts */
		{
			"Rain of Lava",
			"Creates a ball of lava."
		},
		{
			"Kiss of the Succubus",
			"Unleashes a ball of nexus."
		},
		{
			"Immortality",
			"Allows you to resist the ravages of time."
		},
		{
			"Glyph of Warding",
			"It will create a glyph that prevents monsters movement."
		},
		{
			"Lava Storm",
			"Creates a powerful ball of lava."
		},
		{
			"Demonform",
			"Turns you into a demon lord for a short duration."
		},

		/* Hellfire Tome */
		{
			"Unholy word",
			"Heals and cures you."
		},
		{
			"Hellfire",
			"Creates 2 balls of powerful hellfire."
		},
		{
			"Armageddon",
			"Sends into oblivion all monsters near you."
		},
		{
			"Shield of the Damned",
			"Makes you nearly invulnerable for a short duration."
		},
	},
};


/*
 * Hack -- the "basic" color names (see "TERM_xxx")
 */
cptr color_names[16] =
{
	"Dark",
	"White",
	"Slate",
	"Orange",
	"Red",
	"Green",
	"Blue",
	"Umber",
	"Light Dark",
	"Light Slate",
	"Violet",
	"Yellow",
	"Light Red",
	"Light Green",
	"Light Blue",
	"Light Umber",
};


/*
 * Abbreviations of healthy stats
 */
cptr stat_names[6] =
{
	"STR", "INT", "WIS", "DEX", "CON", "CHR"
};

/*
 * Abbreviations of damaged stats
 */
cptr stat_names_reduced[6] =
{
	"Str", "Int", "Wis", "Dex", "Con", "Chr"
};


/*
 * Certain "screens" always use the main screen, including News, Birth,
 * Dungeon, Tomb-stone, High-scores, Macros, Colors, Visuals, Options.
 *
 * Later, special flags may allow sub-windows to "steal" stuff from the
 * main window, including File dump (help), File dump (artifacts, uniques),
 * Character screen, Small scale map, Previous Messages, Store screen, etc.
 *
 * The "ctrl-i" (tab) command flips the "Display inven/equip" and "Display
 * equip/inven" flags for all windows.
 *
 * The "ctrl-g" command (or pseudo-command) should perhaps grab a snapshot
 * of the main screen into any interested windows.
 */
cptr window_flag_desc[32] =
{
	"Display inven/equip",
	"Display equip/inven",
	"Display spell list",
	"Display character",
	"Show visible monsters",
	"Display IRC messages",
	"Display messages",
	"Display overhead view",
	"Display monster recall",
	"Display object recall",
	NULL,
	"Display snap-shot",
	NULL,
	NULL,
	"Display borg messages",
	"Display borg status",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};


/*
 * Available Options
 *
 * Option Screen Sets:
 *
 *      Set 1: User Interface
 *      Set 2: Disturbance
 *      Set 3: Inventory
 *      Set 4: Game Play
 *      Set 5: ToME
 *      Set 6: Birth
 *
 * Note that bits 28-31 of set 0 are currently unused.
 */
option_type option_info[] =
{
	/*** User-Interface ***/

	{ &rogue_like_commands,         FALSE,  1,      0,
	"rogue_like_commands",          "Rogue-like commands" },

	{ &quick_messages,              TRUE,  1,       1,
	"quick_messages",               "Activate quick messages" },

	{ &other_query_flag,            FALSE,  1,      2,
	"other_query_flag",             "Prompt for various information" },

	{ &carry_query_flag,            FALSE,  1,      3,
	"carry_query_flag",             "Prompt before picking things up" },

	{ &use_old_target,              FALSE,  1,      4,
	"use_old_target",               "Use old target by default" },

	{ &always_pickup,               FALSE,  1,      5,
	"always_pickup",                "Pick things up by default" },

	{ &prompt_pickup_heavy,         TRUE,   1,      6,
	"prompt_pickup_heavy",          "Prompt before picking up heavy objects" },

	{ &always_repeat,               TRUE,   1,      7,
	"always_repeat",                "Repeat obvious commands" },

	{ &depth_in_feet,               FALSE,  1,      8,
	"depth_in_feet",                "Show dungeon level in feet" },

	{ &stack_force_notes,           TRUE,  1,       9,
	"stack_force_notes",            "Merge inscriptions when stacking" },

	{ &stack_force_costs,           FALSE,  1,      10,
	"stack_force_costs",            "Merge discounts when stacking" },

	{ &show_labels,                 TRUE,   1,      11,
	"show_labels",                  "Show labels in object listings" },

	{ &show_weights,                TRUE,   1,       12,
	"show_weights",                 "Show weights in object listings" },

	{ &show_inven_graph,            TRUE,   1,      13,
	"show_inven_graph",             "Show graphics in inventory list" },

	{ &show_equip_graph,            TRUE,   1,      14,
	"show_equip_graph",             "Show graphics in equipment list" },

	{ &show_store_graph,            TRUE,   1,      15,
	"show_store_graph",             "Show graphics in stores" },

	{ &show_choices,                TRUE,  1,       16,
	"show_choices",                 "Show choices in certain sub-windows" },

	{ &show_details,                TRUE,  1,       17,
	"show_details",                 "Show details in certain sub-windows" },

	{ &ring_bell,                   FALSE,   1,     18,
	"ring_bell",                    "Audible bell (on errors, etc)" },
	/* Changed to default to FALSE -- it's so extremely annoying!!! -TY */

	{ &use_color,                   TRUE,   1,      19,
	"use_color",                    "Use color if possible (slow)" },


	/*** Disturbance ***/

	{ &find_ignore_stairs,          FALSE,   2,     0,
	"find_ignore_stairs",           "Run past stairs" },

	{ &find_ignore_doors,           TRUE,   2,      1,
	"find_ignore_doors",            "Run through open doors" },

	{ &find_cut,                    FALSE,   2,     2,
	"find_cut",                     "Run past known corners" },

	{ &find_examine,                TRUE,   2,      3,
	"find_examine",                 "Run into potential corners" },

	{ &disturb_move,                FALSE,   2,     4,
	"disturb_move",                 "Disturb whenever any monster moves" },

	{ &disturb_near,                TRUE,   2,      5,
	"disturb_near",                 "Disturb whenever viewable monster moves" },

	{ &disturb_panel,               TRUE,   2,      6,
	"disturb_panel",                "Disturb whenever map panel changes" },

	{ &disturb_detect,              TRUE,   2,      21,
	"disturb_detect",               "Disturb whenever leaving trap-detected area" },

	{ &disturb_state,               TRUE,   2,      7,
	"disturb_state",                "Disturb whenever player state changes" },

	{ &disturb_minor,               TRUE,   2,      8,
	"disturb_minor",                "Disturb whenever boring things happen" },

	{ &disturb_other,               FALSE,   2,      9,
	"disturb_other",                "Disturb whenever random things happen" },

	{ &alert_hitpoint,              FALSE,  2,      10,
	"alert_hitpoint",               "Alert user to critical hitpoints" },

	{ &alert_failure,               FALSE,  2,      11,
	"alert_failure",                "Alert user to various failures" },

	{ &last_words,                  TRUE,   2,      12,
	"last_words",                   "Get last words when the character dies" },

	{ &speak_unique,                TRUE,   2,      13,
	"speak_unique",                 "Allow shopkeepers and uniques to speak" },

	{ &auto_destroy,                TRUE,   2,      14,
	"auto_destroy",                 "No query to destroy known worthless items" },

	{ &wear_confirm,                TRUE,   2,      15,
	"confirm_wear",                 "Confirm to wear/wield known cursed items" },

	{ &confirm_stairs,              FALSE,  2,      16,
	"confirm_stairs",               "Prompt before exiting a dungeon level" },

	{ &disturb_pets,                FALSE,  2,      17,
	"disturb_pets",                 "Disturb when visible pets move" },

#ifdef ALLOW_EASY_OPEN
	{ &easy_open,                   TRUE,   2,      18,
	"easy_open",                    "Automatically open doors" },
#endif /* ALLOW_EASY_OPEN */

#ifdef ALLOW_EASY_DISARM
	{ &easy_disarm,                 TRUE,   2,      19,
	"easy_disarm",                  "Automatically disarm traps" },
#endif /* ALLOW_EASY_DISARM */

	{ &easy_tunnel,                 FALSE,  2,      20,
	"easy_tunnel",                  "Automatically tunnel walls" },

	/*** Game-Play ***/

	{ &auto_haggle,                 TRUE,  3,       0,
	"auto_haggle",                  "Auto-haggle in stores" },

	{ &auto_scum,                   TRUE,  3,       1,
	"auto_scum",                    "Auto-scum for good levels" },

	{ &stack_allow_items,           TRUE,   3,      2,
	"stack_allow_items",            "Allow weapons and armour to stack" },

	{ &stack_allow_wands,           TRUE,   3,      3,
	"stack_allow_wands",            "Allow wands/staffs/rods to stack" },

	{ &expand_look,                 FALSE,  3,      4,
	"expand_look",                  "Expand the power of the look command" },

	{ &expand_list,                 FALSE,  3,      5,
	"expand_list",                  "Expand the power of the list commands" },

	{ &view_perma_grids,            TRUE,   3,      6,
	"view_perma_grids",             "Map remembers all perma-lit grids" },

	{ &view_torch_grids,            FALSE,  3,      7,
	"view_torch_grids",             "Map remembers all torch-lit grids" },

	{ &monster_lite,                TRUE,   3,      19,
	"monster_lite",                 "Allow some monsters to carry light" },

	{ &dungeon_align,               TRUE,   3,      8,
	"dungeon_align",                "Generate dungeons with aligned rooms" },

	{ &dungeon_stair,               TRUE,   3,      9,
	"dungeon_stair",                "Generate dungeons with connected stairs" },

	{ &flow_by_sound,               FALSE,  3,      10,
	"flow_by_sound",                "Monsters chase current location (v.slow)" },

	{ &flow_by_smell,               FALSE,  3,      11,
	"flow_by_smell",                "Monsters chase recent locations (v.slow)" },

	 { &player_symbols,              FALSE, 3,      12,
	"player_symbols",               "Use special symbols for the player char"},

	{ &plain_descriptions,          TRUE,   3,      13,
	"plain_descriptions",           "Plain object descriptions" },

	{ &smart_learn,                 FALSE,  3,      14,
	"smart_learn",                  "Monsters learn from their mistakes" },

	{ &smart_cheat,                 FALSE,  3,      15,
	"smart_cheat",                  "Monsters exploit players weaknesses" },

	{ &stupid_monsters,             FALSE,  3,      16,
	"stupid_monsters",              "Monsters behave stupidly" },

	{ &small_levels,                TRUE,   3,      17,
	"small_levels",                 "Allow unusually small dungeon levels" },

	{ &empty_levels,                TRUE,   3,      18,
	"empty_levels",                 "Allow empty 'arena' levels" },

	/*** Efficiency ***/

	{ &view_reduce_lite,            FALSE,  4,      0,
	"view_reduce_lite",             "Reduce lite-radius when running" },

	{ &view_reduce_view,            FALSE,  4,      1,
	"view_reduce_view",             "Reduce view-radius in town" },

	{ &avoid_abort,                 FALSE,  4,      2,
	"avoid_abort",                  "Avoid checking for user abort" },

	{ &avoid_other,                 FALSE,  4,      3,
	"avoid_other",                  "Avoid processing special colors(slow)" },

	{ &flush_failure,               TRUE,   4,      4,
	"flush_failure",                "Flush input on various failures" },

	{ &flush_disturb,               FALSE,  4,      5,
	"flush_disturb",                "Flush input whenever disturbed" },

	{ &flush_command,               FALSE,  4,      6,
	"flush_command",                "Flush input before every command" },

	{ &fresh_before,                TRUE,   4,      7,
	"fresh_before",                 "Flush output before every command" },

	{ &fresh_after,                 FALSE,  4,      8,
	"fresh_after",                  "Flush output after every command" },

	{ &fresh_message,               FALSE,  4,      9,
	"fresh_message",                "Flush output after every message" },

	{ &compress_savefile,           TRUE,   4,      10,
	"compress_savefile",            "Compress messages in savefiles" },

	{ &hilite_player,               FALSE,  4,      11,
	"hilite_player",                "Hilite the player with the cursor" },

	{ &view_yellow_lite,            FALSE,  4,      12,
	"view_yellow_lite",             "Use special colors for torch-lit grids" },

	{ &view_bright_lite,            FALSE,  4,      13,
	"view_bright_lite",             "Use special colors for 'viewable' grids" },

	{ &view_granite_lite,           FALSE,   4,     14,
	"view_granite_lite",            "Use special colors for wall grids (slow)" },

	{ &view_special_lite,           FALSE,  4,      15,
	"view_special_lite",            "Use special colors for floor grids (slow)" },

	{ &center_player,               FALSE,  4,      16,
	"center_player",                "Center the view on the player (very slow)" },

	/*** ToME options ***/

#if 0 /* It's controlled by insanity instead :) - pelpel */

	{ &flavored_attacks,            TRUE,   5,      0,
	"flavored_attacks",             "Show silly messages when fighting" },

#endif /* 0 */

	{ &(p_body.help.enabled),       TRUE,   5,      1,
	"ingame_help",                  "Ingame contextual help" },

	{ &exp_need,                    FALSE,  5,      2,
	"exp_need",                     "Show the experience needed for next level" },

	{ &autoload_old_colors,         FALSE,  5,      3,
	"old_colors",                   "Use the old(Z) coloring scheme(reload the game)" },

	{ &auto_more,                   FALSE,  5,      4,
	"auto_more",                    "Automatically clear '-more-' prompts" },

	{ &player_char_health,          TRUE,   5,      6,
	"player_char_health",           "Player char represent his/her health" },

	{ &linear_stats,                FALSE,  5,      7,
	"linear_stats",                 "Stats are represented in a linear way" },

	/*** Birth Options ***/

#if 0 /* XXX free -- no more used */
	{ &vanilla_town,                FALSE,  6,      0,
	"vanilla_town",                 "Use 'vanilla' town without quests and wilderness" },
#endif

	{ &maximize,                    TRUE,   6,      1,
	"maximize",                     "Maximize stats" },

	{ &preserve,                    TRUE,   6,      2,
	"preserve",                     "Preserve artifacts" },

	{ &autoroll,                    TRUE,   6,      3,
	"autoroll",                     "Specify 'minimal' stats" },

	{ &point_based,                 FALSE,  6,      17,
	"point_based",                  "Generate character using a point system" },
#if 0
	{ &special_lvls,                TRUE,   6,      4,
	"special_lvls",                 "Allow the use of special, unique, levels" },
#endif
	{ &permanent_levels,            FALSE,  6,      5,
	"permanent_levels",             "Generate persistent dungeons" },

	{ &ironman_rooms,               FALSE,  6,      6,
	"ironman_rooms",                "Always generate very unusual rooms" },

	{ &take_notes,                  TRUE,   6,      7,
	"take_notes",                   "Allow notes to be written to a file" },

	{ &auto_notes,                  TRUE,   6,      8,
	"auto_notes",                   "Automatically note important events" },

#if 0 /* when Ill get some ideas */
	{ &rand_birth,                  FALSE,  6,      9,
	"rand_birth",                   "Random present at birth" },
#endif
	{ &fast_autoroller,             FALSE,  6,      10,
	"fast_autoroller",              "Fast autoroller(NOT on multiuser systems)" },

	{ &cth_monsters,                FALSE,  6,      11,
	"cth_monsters",                 "Allow use of lovecraftian monsters" },

#if 0
	{ &pern_monsters,               TRUE,   6,      12,
	"pern_monsters",                "Allow use of some Pern related monsters" },
#endif
	{ &zang_monsters,               FALSE,  6,      13,
	"zang_monsters",                "Allow use of 'Zangbandish' monsters" },

	{ &joke_monsters,               FALSE,  6,      14,
	"joke_monsters",                "Allow use of some 'joke' monsters" },

/* XXX */

	{ &always_small_level,          FALSE,  6,      16,
	"always_small_level",           "Always make small levels" },

	{ &fate_option,                 TRUE,   6,      18,
	"fate_option",                  "You can receive fates, good or bad" },

	/* XXX 17 is used BEFORE */

	/*** Stacking ***/

	{ &testing_stack,               TRUE,  7,       0,
	"testing_stack",                "Allow objects to stack on floor" },

	{ &testing_carry,               TRUE,  7,       1,
	"testing_carry",                "Allow monsters to carry objects" },


	/*** End of Table ***/

	{ NULL,                         0, 0, 0,
	NULL,                           NULL }
};


cptr chaos_patrons[MAX_PATRON] =
{
	"Slortar",
	"Mabelode",
	"Chardros",
	"Hionhurn",
	"Xiombarg",

	"Pyaray",
	"Balaan",
	"Arioch",
	"Eequor",
	"Narjhan",

	"Balo",
	"Khorne",
	"Slaanesh",
	"Nurgle",
	"Tzeentch",

	"Khaine"
};

int chaos_stats[MAX_PATRON] =
{
	A_CON,  /* Slortar */
	A_CON,  /* Mabelode */
	A_STR,  /* Chardros */
	A_STR,  /* Hionhurn */
	A_STR,  /* Xiombarg */

	A_INT,  /* Pyaray */
	A_STR,  /* Balaan */
	A_INT,  /* Arioch */
	A_CON,  /* Eequor */
	A_CHR,  /* Narjhan */

	-1,     /* Balo */
	A_STR,  /* Khorne */
	A_CHR,  /* Slaanesh */
	A_CON,  /* Nurgle */
	A_INT,  /* Tzeentch */

	A_STR,  /* Khaine */
};




int chaos_rewards[MAX_PATRON][20] =
{
	/* Slortar the Old: */
	{
		REW_WRATH, REW_CURSE_WP, REW_CURSE_AR, REW_RUIN_ABL, REW_LOSE_ABL,
		REW_IGNORE, REW_IGNORE, REW_IGNORE, REW_POLY_WND, REW_POLY_SLF,
		REW_POLY_SLF, REW_POLY_SLF, REW_GAIN_ABL, REW_GAIN_ABL, REW_GAIN_EXP,
		REW_GOOD_OBJ, REW_CHAOS_WP, REW_GREA_OBJ, REW_AUGM_ABL, REW_AUGM_ABL
	},

	/* Mabelode the Faceless: */
	{
		REW_WRATH, REW_CURSE_WP, REW_CURSE_AR, REW_H_SUMMON, REW_SUMMON_M,
		REW_SUMMON_M, REW_IGNORE, REW_IGNORE, REW_POLY_WND, REW_POLY_WND,
		REW_POLY_SLF, REW_HEAL_FUL, REW_HEAL_FUL, REW_GAIN_ABL, REW_SER_UNDE,
		REW_CHAOS_WP, REW_GOOD_OBJ, REW_GOOD_OBJ, REW_GOOD_OBS, REW_GOOD_OBS
	},

	/* Chardros the Reaper: */
	{
		REW_WRATH, REW_WRATH, REW_HURT_LOT, REW_PISS_OFF, REW_H_SUMMON,
		REW_SUMMON_M, REW_IGNORE, REW_IGNORE, REW_DESTRUCT, REW_SER_UNDE,
		REW_GENOCIDE, REW_MASS_GEN, REW_MASS_GEN, REW_DISPEL_C, REW_GOOD_OBJ,
		REW_CHAOS_WP, REW_GOOD_OBS, REW_GOOD_OBS, REW_AUGM_ABL, REW_AUGM_ABL
	},

	/* Hionhurn the Executioner: */
	{
		REW_WRATH, REW_WRATH, REW_CURSE_WP, REW_CURSE_AR, REW_RUIN_ABL,
		REW_IGNORE, REW_IGNORE, REW_SER_UNDE, REW_DESTRUCT, REW_GENOCIDE,
		REW_MASS_GEN, REW_MASS_GEN, REW_HEAL_FUL, REW_GAIN_ABL, REW_GAIN_ABL,
		REW_CHAOS_WP, REW_GOOD_OBS, REW_GOOD_OBS, REW_AUGM_ABL, REW_AUGM_ABL
	},

	/* Xiombarg the Sword-Queen: */
	{
		REW_TY_CURSE, REW_TY_CURSE, REW_PISS_OFF, REW_RUIN_ABL, REW_LOSE_ABL,
		REW_IGNORE, REW_POLY_SLF, REW_POLY_SLF, REW_POLY_WND, REW_POLY_WND,
		REW_GENOCIDE, REW_DISPEL_C, REW_GOOD_OBJ, REW_GOOD_OBJ, REW_SER_MONS,
		REW_GAIN_ABL, REW_CHAOS_WP, REW_GAIN_EXP, REW_AUGM_ABL, REW_GOOD_OBS
	},


	/* Pyaray the Tentacled Whisperer of Impossible Secretes: */
	{
		REW_WRATH, REW_TY_CURSE, REW_PISS_OFF, REW_H_SUMMON, REW_H_SUMMON,
		REW_IGNORE, REW_IGNORE, REW_IGNORE, REW_POLY_WND, REW_POLY_SLF,
		REW_POLY_SLF, REW_SER_DEMO, REW_HEAL_FUL, REW_GAIN_ABL, REW_GAIN_ABL,
		REW_CHAOS_WP, REW_DO_HAVOC, REW_GOOD_OBJ, REW_GREA_OBJ, REW_GREA_OBS
	},

	/* Balaan the Grim: */
	{
		REW_TY_CURSE, REW_HURT_LOT, REW_CURSE_WP, REW_CURSE_AR, REW_RUIN_ABL,
		REW_SUMMON_M, REW_LOSE_EXP, REW_POLY_SLF, REW_POLY_SLF, REW_POLY_WND,
		REW_SER_UNDE, REW_HEAL_FUL, REW_HEAL_FUL, REW_GAIN_EXP, REW_GAIN_EXP,
		REW_CHAOS_WP, REW_GOOD_OBJ, REW_GOOD_OBS, REW_GREA_OBS, REW_AUGM_ABL
	},

	/* Arioch, Duke of Hell: */
	{
		REW_WRATH, REW_PISS_OFF, REW_RUIN_ABL, REW_LOSE_EXP, REW_H_SUMMON,
		REW_IGNORE, REW_IGNORE, REW_IGNORE, REW_IGNORE, REW_POLY_SLF,
		REW_POLY_SLF, REW_MASS_GEN, REW_SER_DEMO, REW_HEAL_FUL, REW_CHAOS_WP,
		REW_CHAOS_WP, REW_GOOD_OBJ, REW_GAIN_EXP, REW_GREA_OBJ, REW_AUGM_ABL
	},

	/* Eequor, Blue Lady of Dismay: */
	{
		REW_WRATH, REW_TY_CURSE, REW_PISS_OFF, REW_CURSE_WP, REW_RUIN_ABL,
		REW_IGNORE, REW_IGNORE, REW_POLY_SLF, REW_POLY_SLF, REW_POLY_WND,
		REW_GOOD_OBJ, REW_GOOD_OBJ, REW_SER_MONS, REW_HEAL_FUL, REW_GAIN_EXP,
		REW_GAIN_ABL, REW_CHAOS_WP, REW_GOOD_OBS, REW_GREA_OBJ, REW_AUGM_ABL
	},

	/* Narjhan, Lord of Beggars: */
	{
		REW_WRATH, REW_CURSE_AR, REW_CURSE_WP, REW_CURSE_WP, REW_CURSE_AR,
		REW_IGNORE, REW_IGNORE, REW_IGNORE, REW_POLY_SLF, REW_POLY_SLF,
		REW_POLY_WND, REW_HEAL_FUL, REW_HEAL_FUL, REW_GAIN_EXP, REW_AUGM_ABL,
		REW_GOOD_OBJ, REW_GOOD_OBJ, REW_CHAOS_WP, REW_GREA_OBJ, REW_GREA_OBS
	},

	/* Balo the Jester: */
	{
		REW_WRATH, REW_SER_DEMO, REW_CURSE_WP, REW_CURSE_AR, REW_LOSE_EXP,
		REW_GAIN_ABL, REW_LOSE_ABL, REW_POLY_WND, REW_POLY_SLF, REW_IGNORE,
		REW_DESTRUCT, REW_MASS_GEN, REW_CHAOS_WP, REW_GREA_OBJ, REW_HURT_LOT,
		REW_AUGM_ABL, REW_RUIN_ABL, REW_H_SUMMON, REW_GREA_OBS, REW_AUGM_ABL
	},

	/* Khorne the Bloodgod: */
	{
		REW_WRATH, REW_HURT_LOT, REW_HURT_LOT, REW_H_SUMMON, REW_H_SUMMON,
		REW_IGNORE, REW_IGNORE, REW_IGNORE, REW_SER_MONS, REW_SER_DEMO,
		REW_POLY_SLF, REW_POLY_WND, REW_HEAL_FUL, REW_GOOD_OBJ, REW_GOOD_OBJ,
		REW_CHAOS_WP, REW_GOOD_OBS, REW_GOOD_OBS, REW_GREA_OBJ, REW_GREA_OBS
	},

	/* Slaanesh: */
	{
		REW_WRATH, REW_PISS_OFF, REW_PISS_OFF, REW_RUIN_ABL, REW_LOSE_ABL,
		REW_LOSE_EXP, REW_IGNORE, REW_IGNORE, REW_POLY_WND, REW_SER_DEMO,
		REW_POLY_SLF, REW_HEAL_FUL, REW_HEAL_FUL, REW_GOOD_OBJ, REW_GAIN_EXP,
		REW_GAIN_EXP, REW_CHAOS_WP, REW_GAIN_ABL, REW_GREA_OBJ, REW_AUGM_ABL
	},

	/* Nurgle: */
	{
		REW_WRATH, REW_PISS_OFF, REW_HURT_LOT, REW_RUIN_ABL, REW_LOSE_ABL,
		REW_LOSE_EXP, REW_IGNORE, REW_IGNORE, REW_IGNORE, REW_POLY_SLF,
		REW_POLY_SLF, REW_POLY_WND, REW_HEAL_FUL, REW_GOOD_OBJ, REW_GAIN_ABL,
		REW_GAIN_ABL, REW_SER_UNDE, REW_CHAOS_WP, REW_GREA_OBJ, REW_AUGM_ABL
	},

	/* Tzeentch: */
	{
		REW_WRATH, REW_CURSE_WP, REW_CURSE_AR, REW_RUIN_ABL, REW_LOSE_ABL,
		REW_LOSE_EXP, REW_IGNORE, REW_POLY_SLF, REW_POLY_SLF, REW_POLY_SLF,
		REW_POLY_SLF, REW_POLY_WND, REW_HEAL_FUL, REW_CHAOS_WP, REW_GREA_OBJ,
		REW_GAIN_ABL, REW_GAIN_ABL, REW_GAIN_EXP, REW_GAIN_EXP, REW_AUGM_ABL
	},

	/* Khaine: */
	{
		REW_WRATH, REW_HURT_LOT, REW_PISS_OFF, REW_LOSE_ABL, REW_LOSE_EXP,
		REW_IGNORE,   REW_IGNORE,   REW_DISPEL_C, REW_DO_HAVOC, REW_DO_HAVOC,
		REW_POLY_SLF, REW_POLY_SLF, REW_GAIN_EXP, REW_GAIN_ABL, REW_GAIN_ABL,
		REW_SER_MONS, REW_GOOD_OBJ, REW_CHAOS_WP, REW_GREA_OBJ, REW_GOOD_OBS
	}
};

/* Names used for random artifact name generation */
cptr artifact_names_list =
"adanedhel\n"
"adurant\n"
"aeglos\n"
"aegnor\n"
"aelin\n"
"aeluin\n"
"aerandir\n"
"aerin\n"
"agarwaen\n"
"aglareb\n"
"aglarond\n"
"aglon\n"
"ainulindale\n"
"ainur\n"
"alcarinque\n"
"aldaron\n"
"aldudenie\n"
"almaren\n"
"alqualonde\n"
"aman\n"
"amandil\n"
"amarie\n"
"amarth\n"
"amlach\n"
"amon\n"
"amras\n"
"amrod\n"
"anach\n"
"anar\n"
"anarion\n"
"ancalagon\n"
"ancalimon\n"
"anarrima\n"
"andor\n"
"andram\n"
"androth\n"
"anduin\n"
"andunie\n"
"anfauglir\n"
"anfauglith\n"
"angainor\n"
"angband\n"
"anghabar\n"
"anglachel\n"
"angrenost\n"
"angrim\n"
"angrist\n"
"angrod\n"
"anguirel\n"
"annael\n"
"annatar\n"
"annon\n"
"annuminas\n"
"apanonar\n"
"aradan\n"
"aragorn\n"
"araman\n"
"aranel\n"
"aranruth\n"
"aranwe\n"
"aras\n"
"aratan\n"
"aratar\n"
"arathorn\n"
"arda\n"
"ard-galen\n"
"aredhel\n"
"ar-feiniel\n"
"argonath\n"
"arien\n"
"armenelos\n"
"arminas\n"
"arnor\n"
"aros\n"
"arossiach\n"
"arthad\n"
"arvernien\n"
"arwen\n"
"ascar\n"
"astaldo\n"
"atalante\n"
"atanamir\n"
"atanatari\n"
"atani\n"
"aule\n"
"avallone\n"
"avari\n"
"avathar\n"
"balan\n"
"balar\n"
"balrog\n"
"barad\n"
"baragund\n"
"barahir\n"
"baran\n"
"baranduin\n"
"bar\n"
"bauglir\n"
"beleg\n"
"belegaer\n"
"belegost\n"
"belegund\n"
"beleriand\n"
"belfalas\n"
"belthil\n"
"belthronding\n"
"beor\n"
"beraid\n"
"bereg\n"
"beren\n"
"boromir\n"
"boron\n"
"bragollach\n"
"brandir\n"
"bregolas\n"
"bregor\n"
"brethil\n"
"brilthor\n"
"brithiach\n"
"brithombar\n"
"brithon\n"
"cabed\n"
"calacirya\n"
"calaquendi\n"
"calenardhon\n"
"calion\n"
"camlost\n"
"caragdur\n"
"caranthir\n"
"carcharoth\n"
"cardolan\n"
"carnil\n"
"celeborn\n"
"celebrant\n"
"celebrimbor\n"
"celebrindal\n"
"celebros\n"
"celegorm\n"
"celon\n"
"cirdan\n"
"cirith\n"
"cirth\n"
"ciryatan\n"
"ciryon\n"
"coimas\n"
"corollaire\n"
"crissaegrim\n"
"cuarthal\n"
"cuivienen\n"
"culurien\n"
"curufin\n"
"curufinwe\n"
"curunir\n"
"cuthalion\n"
"daedeloth\n"
"daeron\n"
"dagnir\n"
"dagor\n"
"dagorlad\n"
"dairuin\n"
"danwedh\n"
"delduwath\n"
"denethor\n"
"dimbar\n"
"dimrost\n"
"dinen\n"
"dior\n"
"dirnen\n"
"dolmed\n"
"doriath\n"
"dorlas\n"
"dorthonion\n"
"draugluin\n"
"drengist\n"
"duath\n"
"duinath\n"
"duilwen\n"
"dunedain\n"
"dungortheb\n"
"earendil\n"
"earendur\n"
"earnil\n"
"earnur\n"
"earrame\n"
"earwen\n"
"echor\n"
"echoriath\n"
"ecthelion\n"
"edain\n"
"edrahil\n"
"eglador\n"
"eglarest\n"
"eglath\n"
"eilinel\n"
"eithel\n"
"ekkaia\n"
"elbereth\n"
"eldalie\n"
"eldalieva\n"
"eldamar\n"
"eldar\n"
"eledhwen\n"
"elemmire\n"
"elende\n"
"elendil\n"
"elendur\n"
"elenna\n"
"elentari\n"
"elenwe\n"
"elerrina\n"
"elleth\n"
"elmoth\n"
"elostirion\n"
"elrond\n"
"elros\n"
"elu\n"
"eluchil\n"
"elured\n"
"elurin\n"
"elwe\n"
"elwing\n"
"emeldir\n"
"endor\n"
"engrin\n"
"engwar\n"
"eol\n"
"eonwe\n"
"ephel\n"
"erchamion\n"
"ereb\n"
"ered\n"
"erech\n"
"eregion\n"
"ereinion\n"
"erellont\n"
"eressea\n"
"eriador\n"
"eru\n"
"esgalduin\n"
"este\n"
"estel\n"
"estolad\n"
"ethir\n"
"ezellohar\n"
"faelivrin\n"
"falas\n"
"falathar\n"
"falathrim\n"
"falmari\n"
"faroth\n"
"fauglith\n"
"feanor\n"
"feanturi\n"
"felagund\n"
"finarfin\n"
"finduilas\n"
"fingolfin\n"
"fingon\n"
"finwe\n"
"firimar\n"
"formenos\n"
"fornost\n"
"frodo\n"
"fuin\n"
"fuinur\n"
"gabilgathol\n"
"galad\n"
"galadriel\n"
"galathilion\n"
"galdor\n"
"galen\n"
"galvorn\n"
"gandalf\n"
"gaurhoth\n"
"gelion\n"
"gelmir\n"
"gelydh\n"
"gil\n"
"gildor\n"
"giliath\n"
"ginglith\n"
"girith\n"
"glaurung\n"
"glingal\n"
"glirhuin\n"
"gloredhel\n"
"glorfindel\n"
"golodhrim\n"
"gondolin\n"
"gondor\n"
"gonnhirrim\n"
"gorgoroth\n"
"gorlim\n"
"gorthaur\n"
"gorthol\n"
"gothmog\n"
"guilin\n"
"guinar\n"
"guldur\n"
"gundor\n"
"gurthang\n"
"gwaith\n"
"gwareth\n"
"gwindor\n"
"hadhodrond\n"
"hador\n"
"haladin\n"
"haldad\n"
"haldan\n"
"haldar\n"
"haldir\n"
"haleth\n"
"halmir\n"
"handir\n"
"harad\n"
"hareth\n"
"hathaldir\n"
"hathol\n"
"haudh\n"
"helcar\n"
"helcaraxe\n"
"helevorn\n"
"helluin\n"
"herumor\n"
"herunumen\n"
"hildorien\n"
"himlad\n"
"himring\n"
"hirilorn\n"
"hisilome\n"
"hithaeglir\n"
"hithlum\n"
"hollin\n"
"huan\n"
"hunthor\n"
"huor\n"
"hurin\n"
"hyarmendacil\n"
"hyarmentir\n"
"iant\n"
"iaur\n"
"ibun\n"
"idril\n"
"illuin\n"
"ilmare\n"
"ilmen\n"
"iluvatar\n"
"imlach\n"
"imladris\n"
"indis\n"
"ingwe\n"
"irmo\n"
"isil\n"
"isildur\n"
"istari\n"
"ithil\n"
"ivrin\n"
"kelvar\n"
"kementari\n"
"ladros\n"
"laiquendi\n"
"lalaith\n"
"lamath\n"
"lammoth\n"
"lanthir\n"
"laurelin\n"
"leithian\n"
"legolin\n"
"lembas\n"
"lenwe\n"
"linaewen\n"
"lindon\n"
"lindorie\n"
"loeg\n"
"lomelindi\n"
"lomin\n"
"lomion\n"
"lorellin\n"
"lorien\n"
"lorindol\n"
"losgar\n"
"lothlann\n"
"lothlorien\n"
"luin\n"
"luinil\n"
"lumbar\n"
"luthien\n"
"mablung\n"
"maedhros\n"
"maeglin\n"
"maglor\n"
"magor\n"
"mahanaxar\n"
"mahtan\n"
"maiar\n"
"malduin\n"
"malinalda\n"
"mandos\n"
"manwe\n"
"mardil\n"
"melian\n"
"melkor\n"
"menegroth\n"
"meneldil\n"
"menelmacar\n"
"meneltarma\n"
"minas\n"
"minastir\n"
"mindeb\n"
"mindolluin\n"
"mindon\n"
"minyatur\n"
"mirdain\n"
"miriel\n"
"mithlond\n"
"mithrandir\n"
"mithrim\n"
"mordor\n"
"morgoth\n"
"morgul\n"
"moria\n"
"moriquendi\n"
"mormegil\n"
"morwen\n"
"nahar\n"
"naeramarth\n"
"namo\n"
"nandor\n"
"nargothrond\n"
"narog\n"
"narsil\n"
"narsilion\n"
"narya\n"
"nauglamir\n"
"naugrim\n"
"ndengin\n"
"neithan\n"
"neldoreth\n"
"nenar\n"
"nenning\n"
"nenuial\n"
"nenya\n"
"nerdanel\n"
"nessa\n"
"nevrast\n"
"nibin\n"
"nienna\n"
"nienor\n"
"nimbrethil\n"
"nimloth\n"
"nimphelos\n"
"nimrais\n"
"nimras\n"
"ningloron\n"
"niniel\n"
"ninniach\n"
"ninquelote\n"
"niphredil\n"
"nirnaeth\n"
"nivrim\n"
"noegyth\n"
"nogrod\n"
"noldolante\n"
"noldor\n"
"numenor\n"
"nurtale\n"
"obel\n"
"ohtar\n"
"oiolosse\n"
"oiomure\n"
"olorin\n"
"olvar\n"
"olwe\n"
"ondolinde\n"
"orfalch\n"
"ormal\n"
"orocarni\n"
"orodreth\n"
"orodruin\n"
"orome\n"
"oromet\n"
"orthanc\n"
"osgiliath\n"
"osse\n"
"ossiriand\n"
"palantir\n"
"pelargir\n"
"pelori\n"
"periannath\n"
"quendi\n"
"quenta\n"
"quenya\n"
"radagast\n"
"radhruin\n"
"ragnor\n"
"ramdal\n"
"rana\n"
"rathloriel\n"
"rauros\n"
"region\n"
"rerir\n"
"rhovanion\n"
"rhudaur\n"
"rhun\n"
"rhunen\n"
"rian\n"
"ringil\n"
"ringwil\n"
"romenna\n"
"rudh\n"
"rumil\n"
"saeros\n"
"salmar\n"
"saruman\n"
"sauron\n"
"serech\n"
"seregon\n"
"serinde\n"
"shelob\n"
"silmarien\n"
"silmaril\n"
"silpion\n"
"sindar\n"
"singollo\n"
"sirion\n"
"soronume\n"
"sul\n"
"sulimo\n"
"talath\n"
"taniquetil\n"
"tar\n"
"taras\n"
"tarn\n"
"tathren\n"
"taur\n"
"tauron\n"
"teiglin\n"
"telchar\n"
"telemnar\n"
"teleri\n"
"telperion\n"
"telumendil\n"
"thalion\n"
"thalos\n"
"thangorodrim\n"
"thargelion\n"
"thingol\n"
"thoronath\n"
"thorondor\n"
"thranduil\n"
"thuringwethil\n"
"tilion\n"
"tintalle\n"
"tinuviel\n"
"tirion\n"
"tirith\n"
"tol\n"
"tulkas\n"
"tumhalad\n"
"tumladen\n"
"tuna\n"
"tuor\n"
"turambar\n"
"turgon\n"
"turin\n"
"uial\n"
"uilos\n"
"uinen\n"
"ulairi\n"
"ulmo\n"
"ulumuri\n"
"umanyar\n"
"umarth\n"
"umbar\n"
"ungoliant\n"
"urthel\n"
"uruloki\n"
"utumno\n"
"vaire\n"
"valacirca\n"
"valandil\n"
"valaquenta\n"
"valar\n"
"valaraukar\n"
"valaroma\n"
"valier\n"
"valimar\n"
"valinor\n"
"valinoreva\n"
"valmar\n"
"vana\n"
"vanyar\n"
"varda\n"
"vasa\n"
"vilya\n"
"vingilot\n"
"vinyamar\n"
"voronwe\n"
"wethrin\n"
"wilwarin\n"
"yavanna\n"
;


martial_arts ma_blows[MAX_MA] =
{
	{ "You punch %s.",                          1, 0, 1, 4, 0, 0 },
	{ "You kick %s.",                           2, 0, 1, 6, 0, 0 },
	{ "You strike %s.",                         3, 0, 1, 7, 0, 0 },
	{ "You hit %s with your knee.",             5, 5, 2, 3, MA_KNEE, 0 },
	{ "You hit %s with your elbow.",            7, 5, 1, 8, 0, 0 },
	{ "You butt %s.",                           9, 10, 2, 5, 0, 0 },
	{ "You kick %s.",                           11, 10, 3, 4, MA_SLOW, 0 },
	{ "You uppercut %s.",                       13, 12, 4, 4, MA_STUN, 6 },
	{ "You double-kick %s.",                    16, 15, 5, 4, MA_STUN, 8 },
	{ "You hit %s with a Cat's Claw.",          20, 20, 5, 5, 0, 0 },
	{ "You hit %s with a jump kick.",           25, 25, 5, 6, MA_STUN, 10 },
	{ "You hit %s with an Eagle's Claw.",       29, 25, 6, 6, 0, 0 },
	{ "You hit %s with a circle kick.",         33, 30, 6, 8, MA_STUN, 10 },
	{ "You hit %s with an Iron Fist.",          37, 35, 8, 8, MA_STUN, 10 },
	{ "You hit %s with a flying kick.",         41, 35, 8, 10, MA_STUN, 12 },
	{ "You hit %s with a Dragon Fist.",         45, 35, 10, 10, MA_STUN, 16 },
	{ "You hit %s with a Crushing Blow.",       48, 35, 10, 12, MA_STUN, 18 },
};

/*
 *   cptr    desc;      A verbose attack description
 *   int     min_level; Minimum level to use
 *   int     chance;    Chance of 'success
 *   int     dd;        Damage dice
 *   int     ds;        Damage sides
 *   s16b    effect;    Special effects
 *   s16b    power;     Special effects power
 */
martial_arts bear_blows[MAX_BEAR] =
{
	{ "You claw %s.",                       1, 0, 3, 4, MA_STUN, 4 },
	{ "You swat %s.",                       4, 0, 4, 4, MA_WOUND, 20 },
	{ "You bite %s.",                       9, 2, 4, 4, MA_WOUND, 30 },
	{ "You hug %s.",                       15, 5, 6, 4, MA_FULL_SLOW, 0 },
	{ "You swat and rake %s.",             25,10, 6, 5, MA_STUN | MA_WOUND, 10 },
	{ "You hug and claw %s.",              30,15, 6, 6, MA_FULL_SLOW | MA_WOUND, 60 },
	{ "You double swat %s.",               35,20, 9, 7, MA_STUN | MA_WOUND, 20 },
	{ "You double swat and rake %s.",      40,25,10,10, MA_STUN | MA_WOUND, 25 },
};


magic_power mindcraft_powers[MAX_MINDCRAFT_POWERS] =
{
	/* Level gained,  cost,  %fail,  name,  desc */
	{
		/* Det. monsters/traps */
		1,   1,  15,
		"Precognition",
		"Detect monsters, traps and level layout and lights up at higher levels."
	},
	{
		/* ~MM */
		2,   1,  20,
		"Neural Blast",
		"Blast the minds of your foes."
	},
	{
		/* Phase/Between gate */
		3,   2,  25,
		"Minor Displacement",
		"Short distance teleportation"
	},
	{
		/* Tele. Self / All */
		7,   6,  35,
		"Major Displacement",
		"Teleport you and others at high levels."
	},
	{
		9,   7,  50,
		"Domination",
		"Charm monsters"
	},
	{
		/* Telekinetic "bolt" */
		11,  7,  30,
		"Pulverise",
		"Fires a bolt of pure sound."
	},
	{
		/* Psychic/physical defences */
		13, 12,  50,
		"Character Armour",
		"Sets up physical/elemental shield."
	},
	{
		15, 12,  60,
		"Psychometry",
		"Senses/identifies objects."
	},
	{
		/* Ball -> LOS */
		18, 10,  45,
		"Mind Wave",
		"Projects psi waves to crush the minds of your foes."
	},
	{
		23, 15,  50,
		"Adrenaline Channeling",
		"Heals you, cures you and speeds you."
	},
	{
		/* Convert enemy HP to mana */
		25, 10,  40,
		"Psychic Drain",
		"Drain your foes life into your mana reserves"
	},
	{
		/* Ball -> LOS */
		28, 20,  45,
		"Telekinetic Wave",
		"Powerful wave of pure telekinetic forces."
	},
};

magic_power necro_powers[MAX_NECRO_POWERS] =
{
	/* Level gained,  cost,  %fail,  name,  desc */
	{
		/* Bolt/beam/ball/LOS of stun/scare */
		1,   2, 10,
		"Horrify",
		"Call upon the darkness to stun and scare your foes."
	},
	{
		/* Ball */
		5,   6, 20,
		"Raise Dead",
		"Brings back your foes in the form of various undead, also can heal monsters."
	},
	{
		/* Heals when killing a monster */
		20,  10, 25,
		"Absorb Soul",
		"Gives back some life for each kill."
	},
	{
		/* Bolt */
		30,  15, 20,
		"Vampirism",
		"Drain the life of your foes into your own."
	},
	{
		/* The Death word, always bolt put your HP to 1 */
		35, 100, 25,
		"Death",
		"Instantly kills your opponent and you, turning yourself into an undead."
	},
};

magic_power mimic_powers[MAX_MIMIC_POWERS] =
{
	/* Level gained,  cost,  %fail,  name */
	{
		/* Use a book of lore */
		1,   2,  0,
		"Mimic",
		"Lets you use the powers of a Cloak of Mimicry."
	},
	{
		/* Invisibility */
		10,   6, 20,
		"Invisibility",
		"Hides you from the sight of mortals."
	},
	{
		/* +1 pair of legs */
		25,  20, 25,
		"Legs Mimicry",
		"Temporarily provides a new pair of legs."
	},
	{
		/* wall form */
		30,  40, 30,
		"Wall Mimicry",
		"Temporarily lets you walk in walls, and ONLY in walls."
	},
	{
		/* +1 pair of arms, +1 weapon */
		35, 100, 40,
		"Arms Mimicry",
		"Temporarily provides a new pair of arms."
	},
};

alchemist_recipe alchemist_recipes[MAX_ALCHEMIST_RECIPES] = {
	{SV_BATERIE_POISON,
		{
			{EGO_BRAND_POIS,TV_SWORD,0,8},
			{EGO_BRAND_POIS,TV_HAFTED,0,8},
			{EGO_BRAND_POIS,TV_POLEARM,0,8},
			{EGO_BRAND_POIS,TV_AXE,0,8},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
		},
		{
			{TV_BOTTLE,1,1,
			 TV_POTION,SV_POTION_POISON},
			{TV_WAND,SV_WAND_NOTHING,1,
			 TV_WAND,SV_WAND_STINKING_CLOUD},
			{TV_WAND,SV_WAND_STINKING_CLOUD,1,
			 TV_WAND,SV_WAND_STINKING_CLOUD},
			{TV_RING,SV_RING_NOTHING,4,
			 TV_RING,SV_RING_RESIST_POIS},
			{0,0,1,
			 0,0},
			{0,0,1,
			 0,0},
			{0,0,1,
			 0,0},
			{0,0,1,
			 0,0},
			{0,0,1,
			 0,0},
		}
	},
	{SV_BATERIE_EXPLOSION,
		{
			{EGO_SLAYING_WEAPON,TV_SWORD,ALCHEMIST_ENCHANT_DAM,8},
			{EGO_SLAYING_WEAPON,TV_HAFTED,ALCHEMIST_ENCHANT_DAM,8},
			{EGO_SLAYING_WEAPON,TV_POLEARM,ALCHEMIST_ENCHANT_DAM,8},
			{EGO_SLAYING_WEAPON,TV_AXE,ALCHEMIST_ENCHANT_DAM,8},
			{EGO_SLAYING,TV_GLOVES,ALCHEMIST_ENCHANT_DAM,8},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
		},
		{
			{TV_WAND,SV_WAND_NOTHING,1,
			 TV_WAND,SV_WAND_MAGIC_MISSILE},
			{TV_BOTTLE,1,1,
			 TV_POTION,SV_POTION_DETONATIONS},
			{TV_STAFF,SV_STAFF_NOTHING,2,
			 TV_STAFF,SV_STAFF_EARTHQUAKES},
			{TV_SCROLL,SV_SCROLL_NOTHING,1,
			 TV_SCROLL,SV_SCROLL_DARKNESS},
			{TV_WAND,SV_WAND_MAGIC_MISSILE,1,
			 TV_WAND,SV_WAND_MAGIC_MISSILE},
			{TV_STAFF,SV_STAFF_EARTHQUAKES,2,
			 TV_STAFF,SV_STAFF_EARTHQUAKES},
			{0,0,1,
			 0,0},
			{0,0,1,
			 0,0},
			{0,0,1,
			 0,0},
		}
	},
	{SV_BATERIE_TELEPORT,
		{
			{EGO_TELEPORTATION,TV_CROWN,0,7},
			{EGO_TELEPORTATION,TV_HELM,0,7},
			{EGO_DRAGON, TV_SWORD,ALCHEMIST_ENCHANT_PVAL,9},
			{EGO_DRAGON, TV_HAFTED,ALCHEMIST_ENCHANT_PVAL,9},
			{EGO_DRAGON, TV_POLEARM,ALCHEMIST_ENCHANT_PVAL,9},
			{EGO_DRAGON, TV_AXE,ALCHEMIST_ENCHANT_PVAL,9},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
		},
		{
			{TV_SCROLL,SV_SCROLL_NOTHING,1,
			 TV_SCROLL,SV_SCROLL_TELEPORT},
			{TV_ROD,SV_ROD_NOTHING,3,
			 TV_ROD,SV_ROD_TELEPORT_AWAY},
			{TV_WAND,SV_WAND_NOTHING,1,
			 TV_WAND,SV_WAND_TELEPORT_AWAY},
			{TV_WAND,SV_WAND_TELEPORT_AWAY,1,
			 TV_WAND,SV_WAND_TELEPORT_AWAY},
			{TV_STAFF,SV_STAFF_NOTHING,1,
			 TV_STAFF,SV_STAFF_TELEPORTATION},
			{TV_STAFF,SV_STAFF_TELEPORTATION,1,
			 TV_STAFF,SV_STAFF_TELEPORTATION},
			{TV_RING,SV_RING_NOTHING,3,
			 TV_RING,SV_RING_TELEPORTATION},
			{TV_AMULET,SV_AMULET_NOTHING,4,
			 TV_AMULET,SV_AMULET_TELEPORT},
			{0,0,1,
			 0,0},
		}
	},
	{SV_BATERIE_ACID,
		{
			{EGO_BRAND_ACID,TV_SWORD,0,8},
			{EGO_BRAND_ACID,TV_HAFTED,0,8},
			{EGO_BRAND_ACID,TV_POLEARM,0,8},
			{EGO_BRAND_ACID,TV_AXE,0,8},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
		},
		{
			{TV_RING,SV_RING_NOTHING,8,
			 TV_RING,SV_RING_ACID},
			{TV_WAND,SV_WAND_NOTHING,2,
			 TV_WAND,SV_WAND_ACID_BOLT},
			{TV_WAND,SV_WAND_ACID_BOLT,2,
			 TV_WAND,SV_WAND_ACID_BOLT},
			{TV_ROD,SV_ROD_NOTHING,5,
			 TV_ROD,SV_ROD_ACID_BOLT},
			{TV_SCROLL,SV_SCROLL_NOTHING,1,
			 TV_SCROLL,SV_SCROLL_CURSE_ARMOR},
			{TV_AMULET,SV_AMULET_NOTHING,2,
			 TV_AMULET,SV_AMULET_RESIST_ACID},
			{0,0,1,
			 0,0},
			{0,0,1,
			 0,0},
			{0,0,1,
			 0,0},
		}
	},
	{SV_BATERIE_COLD,
		{
			{EGO_BRAND_COLD,TV_SWORD,0,8},
			{EGO_BRAND_COLD,TV_HAFTED,0,8},
			{EGO_BRAND_COLD,TV_POLEARM,0,8},
			{EGO_BRAND_COLD,TV_AXE,0,8},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
		},
		{
			{TV_RING,SV_RING_NOTHING,8,
			 TV_RING,SV_RING_ICE},
			{TV_WAND,SV_WAND_NOTHING,1,
			 TV_WAND,SV_WAND_COLD_BOLT},
			{TV_WAND,SV_WAND_COLD_BOLT,1,
			 TV_WAND,SV_WAND_COLD_BOLT},
			{TV_ROD,SV_ROD_NOTHING,4,
			 TV_ROD,SV_ROD_COLD_BOLT},
			{TV_SCROLL,SV_SCROLL_NOTHING,1,
			 TV_SCROLL,SV_SCROLL_ICE},
			{TV_BOTTLE,1,1,
			 TV_POTION,SV_POTION_RESIST_COLD},
			{0,0,1,
			 0,0},
			{0,0,1,
			 0,0},
			{0,0,1,
			 0,0},
		}
	},
	{SV_BATERIE_FIRE,
		{
			{EGO_BRAND_FIRE,TV_SWORD,0,8},
			{EGO_BRAND_FIRE,TV_HAFTED,0,8},
			{EGO_AURA_FIRE,TV_CLOAK,0,9},
			{EGO_BRAND_FIRE,TV_POLEARM,0,8},
			{EGO_BRAND_FIRE,TV_AXE,0,8},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
		},
		{
			{TV_RING,SV_RING_NOTHING,8,
			 TV_RING,SV_RING_FLAMES},
			{TV_WAND,SV_WAND_NOTHING,2,
			 TV_WAND,SV_WAND_FIRE_BOLT},
			{TV_WAND,SV_WAND_FIRE_BOLT,2,
			 TV_WAND,SV_WAND_FIRE_BOLT},
			{TV_ROD,SV_ROD_NOTHING,4,
			 TV_ROD,SV_ROD_FIRE_BOLT},
			{TV_SCROLL,SV_SCROLL_NOTHING,1,
			 TV_SCROLL,SV_SCROLL_FIRE},
			{TV_BOTTLE,1,1,
			 TV_POTION,SV_POTION_RESIST_HEAT},
			{0,0,1,
			 0,0},
			{0,0,1,
			 0,0},
			{0,0,1,
			 0,0},
		}
	},
	{SV_BATERIE_LIFE,
		{
			{EGO_VAMPIRIC,TV_SWORD,0,12},
			{EGO_VAMPIRIC,TV_HAFTED,0,12},
			{EGO_VAMPIRIC,TV_POLEARM,0,12},
			{EGO_VAMPIRIC,TV_AXE,0,12},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
		},
		{
			{TV_RING,SV_RING_NOTHING,5,
			 TV_RING,SV_RING_PROTECTION},
			{TV_STAFF,SV_STAFF_NOTHING,2,
			 TV_STAFF,SV_STAFF_CURING},
			{TV_STAFF,SV_STAFF_CURING,2,
			 TV_STAFF,SV_STAFF_CURING},
			{TV_WAND,SV_WAND_NOTHING,2,
			 TV_WAND,SV_WAND_DRAIN_LIFE},
			{TV_WAND,SV_WAND_DRAIN_LIFE,2,
			 TV_WAND,SV_WAND_DRAIN_LIFE},
			{TV_ROD,SV_ROD_NOTHING,5,
			 TV_ROD,SV_ROD_CURING},
			{TV_SCROLL,SV_SCROLL_NOTHING,1,
			 TV_SCROLL,SV_SCROLL_SATISFY_HUNGER},
			{0,0,1,
			 0,0},
			{0,0,1,
			 0,0},
		}
	},
	{SV_BATERIE_CONFUSION,
		{
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
		},
		{
			{TV_RING,SV_RING_NOTHING,2,
			 TV_RING,SV_RING_RES_CONFUSION},
			{TV_WAND,SV_WAND_NOTHING,1,
			 TV_WAND,SV_WAND_CONFUSE_MONSTER},
			{TV_WAND,SV_WAND_CONFUSE_MONSTER,1,
			 TV_WAND,SV_WAND_CONFUSE_MONSTER},
			{TV_SCROLL,SV_SCROLL_NOTHING,1,
			 TV_SCROLL,SV_SCROLL_MONSTER_CONFUSION},
			{TV_BOTTLE,1,1,
			 TV_POTION,SV_POTION_CONFUSION},
			{0,0,1,
			 0,0},
			{0,0,1,
			 0,0},
			{0,0,1,
			 0,0},
			{0,0,1,
			 0,0},
		}
	},
	{SV_BATERIE_LITE,
		{
			{EGO_LITE,TV_CROWN,0,5},
			{EGO_LITE,TV_HELM,0,5},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
		},
		{
			{TV_RING,SV_RING_NOTHING,6,
			 TV_RING,SV_RING_RES_LD},
			{TV_STAFF,SV_STAFF_NOTHING,1,
			 TV_STAFF,SV_STAFF_LITE},
			{TV_STAFF,SV_STAFF_LITE,1,
			 TV_STAFF,SV_STAFF_LITE},
			{TV_WAND,SV_WAND_NOTHING,1,
			 TV_WAND,SV_WAND_LITE},
			{TV_WAND,SV_WAND_LITE,1,
			 TV_WAND,SV_WAND_LITE},
			{TV_ROD,SV_ROD_NOTHING,3,
			 TV_ROD,SV_ROD_ILLUMINATION},
			{TV_SCROLL,SV_SCROLL_NOTHING,1,
			 TV_SCROLL,SV_SCROLL_LIGHT},
			{TV_BOTTLE,1,2,
			 TV_POTION,SV_POTION_INFRAVISION},
			{0,0,1,
			 0,0},
		}
	},
	{SV_BATERIE_CHAOS,
		{
			{EGO_CHAOTIC,TV_SWORD,0,10},
			{EGO_CHAOTIC,TV_HAFTED,0,10},
			{EGO_CHAOTIC,TV_POLEARM,0,10},
			{EGO_CHAOTIC,TV_AXE,0,10},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
		},
		{
			{TV_RING,SV_RING_NOTHING,5,
			 TV_RING,SV_RING_RES_CHAOS},
			{TV_WAND,SV_WAND_NOTHING,4,
			 TV_WAND,SV_WAND_ANNIHILATION},
			{TV_WAND,SV_WAND_ANNIHILATION,4,
			 TV_WAND,SV_WAND_ANNIHILATION},
			{TV_ROD,SV_ROD_NOTHING,5,
			 TV_ROD,SV_ROD_POLYMORPH},
			{TV_SCROLL,SV_SCROLL_NOTHING,2,
			 TV_SCROLL,SV_SCROLL_CHAOS},
			{TV_BOTTLE,1,3,
			 TV_POTION,SV_POTION_MUTATION},
			{0,0,1,
			 0,0},
			{0,0,1,
			 0,0},
			{0,0,1,
			 0,0},
		}
	},
	{SV_BATERIE_TIME,
		{
			{EGO_SPEED,TV_BOOTS,ALCHEMIST_ENCHANT_PVAL,13},
			{EGO_RESISTANCE,TV_SOFT_ARMOR,0,10},
			{EGO_RESISTANCE,TV_HARD_ARMOR,0,10},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
		},
		{
			{TV_RING,SV_RING_NOTHING,6,
			 TV_RING,SV_RING_SPEED},
			{TV_STAFF,SV_STAFF_NOTHING,2,
			 TV_STAFF,SV_STAFF_SPEED},
			{TV_STAFF,SV_STAFF_SPEED,2,
			 TV_STAFF,SV_STAFF_SPEED},
			{TV_ROD,SV_ROD_NOTHING,9,
			 TV_ROD,SV_ROD_SPEED},
			{TV_BOTTLE,1,33,
			 TV_POTION,SV_POTION_EXPERIENCE},
			{0,0,1,
			 0,0},
			{0,0,1,
			 0,0},
			{0,0,1,
			 0,0},
			{0,0,1,
			 0,0},
		}
	},
	{SV_BATERIE_MAGIC,
		{
			{EGO_MAGI,TV_HELM,ALCHEMIST_ENCHANT_PVAL,10},
			{EGO_MAGI,TV_CROWN,ALCHEMIST_ENCHANT_PVAL,10},
			{EGO_JUMP,TV_BOOTS,0,6},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
		},
		{
			{TV_AMULET,SV_AMULET_NOTHING,6,
			 TV_AMULET,SV_AMULET_THE_MAGI},
			{TV_RING,SV_RING_NOTHING,5,
			 TV_RING,SV_RING_INT},
			{TV_STAFF,SV_STAFF_NOTHING,3,
			 TV_STAFF,SV_STAFF_THE_MAGI},
			{TV_STAFF,SV_STAFF_THE_MAGI,3,
			 TV_STAFF,SV_STAFF_THE_MAGI},
			{TV_ROD,SV_ROD_NOTHING,9,
			 TV_ROD,SV_ROD_IDENTIFY},
			{TV_SCROLL,SV_SCROLL_NOTHING,99,
			 TV_SCROLL,SV_SCROLL_ARTIFACT},
			{TV_BOTTLE,1,2,
			 TV_POTION,SV_POTION_RESTORE_MANA},
			{0,0,1,
			 0,0},
			{0,0,1,
			 0,0},
		}
	},
	{SV_BATERIE_XTRA_LIFE,
		{
			{EGO_LIFE,TV_SWORD,ALCHEMIST_ENCHANT_PVAL,15},
			{EGO_LIFE,TV_HAFTED,ALCHEMIST_ENCHANT_PVAL,15},
			{EGO_LIFE,TV_POLEARM,ALCHEMIST_ENCHANT_PVAL,15},
			{EGO_LIFE,TV_AXE,ALCHEMIST_ENCHANT_PVAL,15},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
		},
		{
			{TV_BOTTLE,1,2,
			 TV_POTION,SV_POTION_NEW_LIFE},
			{TV_STAFF,SV_STAFF_NOTHING,2,
			 TV_STAFF,SV_STAFF_HEALING},
			{TV_STAFF,SV_STAFF_HEALING,2,
			 TV_STAFF,SV_STAFF_HEALING},
			{TV_WAND,SV_WAND_NOTHING,2,
			 TV_WAND,SV_WAND_CHARM_MONSTER},
			{TV_WAND,SV_WAND_CHARM_MONSTER,2,
			 TV_WAND,SV_WAND_CHARM_MONSTER},
			{TV_ROD,SV_ROD_NOTHING,5,
			 TV_ROD,SV_ROD_HEALING},
			{TV_SCROLL,SV_SCROLL_NOTHING,3,
			 TV_SCROLL,SV_SCROLL_MASS_GENOCIDE},
			{0,0,1,
			 0,0},
			{0,0,1,
			 0,0},
		}
	},
	{SV_BATERIE_DARKNESS,
		{
			{EGO_STEALTH,TV_CLOAK,ALCHEMIST_ENCHANT_PVAL,12},
			{EGO_QUIET,TV_BOOTS,ALCHEMIST_ENCHANT_PVAL,12},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
		},

		{
			{TV_BOTTLE,1,2,
			 TV_POTION,SV_POTION_INVIS},
			{TV_WAND,SV_WAND_NOTHING,1,
			 TV_WAND,SV_WAND_FEAR_MONSTER},
			{TV_WAND,SV_WAND_FEAR_MONSTER,2,
			 TV_WAND,SV_WAND_FEAR_MONSTER},
			{TV_STAFF,SV_STAFF_NOTHING,1,
			 TV_STAFF,SV_STAFF_DARKNESS},
			{TV_STAFF,SV_STAFF_DARKNESS,1,
			 TV_STAFF,SV_STAFF_DARKNESS},
			{TV_ROD,SV_ROD_NOTHING,6,
			 TV_ROD,SV_ROD_SLEEP_MONSTER},
			{TV_RING,SV_RING_NOTHING,8,
			 TV_RING,SV_RING_INVIS},
			{TV_AMULET,SV_AMULET_NOTHING,3,
			 TV_AMULET,SV_AMULET_DOOM},
			{0,0,0,
			 0,0},
		},
	},
	{SV_BATERIE_KNOWLEDGE,
		{
			{EGO_WISDOM,TV_CROWN,ALCHEMIST_ENCHANT_PVAL,8},
			{EGO_WISDOM,TV_HELM,ALCHEMIST_ENCHANT_PVAL,8},
			{EGO_BLESS_BLADE,TV_SWORD,ALCHEMIST_ENCHANT_PVAL,7},
			{EGO_BLESS_BLADE,TV_HAFTED,ALCHEMIST_ENCHANT_PVAL,7},
			{EGO_BLESS_BLADE,TV_POLEARM,ALCHEMIST_ENCHANT_PVAL,7},
			{EGO_BLESS_BLADE,TV_AXE,ALCHEMIST_ENCHANT_PVAL,7},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
		},

		{
			{TV_BOTTLE,1,7,
			 TV_POTION,SV_POTION_SELF_KNOWLEDGE},
			{TV_WAND,SV_WAND_NOTHING,2,
			 TV_WAND,SV_WAND_DISARMING},
			{TV_WAND,SV_WAND_DISARMING,2,
			 TV_WAND,SV_WAND_DISARMING},
			{TV_STAFF,SV_STAFF_NOTHING,3,
			 TV_STAFF,SV_STAFF_IDENTIFY},
			{TV_STAFF,SV_STAFF_IDENTIFY,3,
			 TV_STAFF,SV_STAFF_IDENTIFY},
			{TV_ROD,SV_ROD_NOTHING,7,
			 TV_ROD,SV_ROD_PROBING},
			{TV_RING,SV_RING_NOTHING,5,
			 TV_RING,SV_RING_SEE_INVIS},
			{TV_AMULET,SV_AMULET_NOTHING,6,
			 TV_AMULET,SV_AMULET_SEARCHING},
			{TV_SCROLL,SV_SCROLL_NOTHING,2,
			 TV_SCROLL,SV_SCROLL_IDENTIFY},
		},
	},
	{SV_BATERIE_FORCE,
		{
			{EGO_PROTECTION,TV_CLOAK,ALCHEMIST_ENCHANT_AC,6},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
		},

		{
			{TV_BOTTLE,1,7,
			 TV_POTION,SV_POTION_RESISTANCE},
			{TV_WAND,SV_WAND_NOTHING,1,
			 TV_WAND,SV_WAND_STONE_TO_MUD},
			{TV_WAND,SV_WAND_STONE_TO_MUD,1,
			 TV_WAND,SV_WAND_STONE_TO_MUD},
			{TV_STAFF,SV_STAFF_NOTHING,2,
			 TV_STAFF,SV_STAFF_DESTRUCTION},
			{TV_STAFF,SV_STAFF_DESTRUCTION,2,
			 TV_STAFF,SV_STAFF_DESTRUCTION},
			{TV_ROD,SV_ROD_NOTHING,20,
			 TV_ROD,SV_ROD_HAVOC},
			{TV_RING,SV_RING_NOTHING,8,
			 TV_RING,SV_RING_RES_SHARDS},
			{TV_AMULET,SV_AMULET_NOTHING,2,
			 TV_AMULET,SV_AMULET_SLOW_DIGEST},
			{TV_SCROLL,SV_SCROLL_NOTHING,2,
			 TV_SCROLL,SV_SCROLL_ENCHANT_ARMOR},
		},
	},
	{SV_BATERIE_LIGHTNING,
		{
			{EGO_RESIST_ELEC,TV_SOFT_ARMOR,0,4},
			{EGO_RESIST_ELEC,TV_HARD_ARMOR,0,4},
			{EGO_ENDURE_ELEC,TV_SHIELD,0,4},
			{EGO_BRAND_ELEC,TV_SWORD,0,9},
			{EGO_BRAND_ELEC,TV_POLEARM,0,9},
			{EGO_BRAND_ELEC,TV_HAFTED,0,9},
			{EGO_LIGHTNING_BOLT,TV_SHOT,0,2},
			{EGO_LIGHTNING_BOLT,TV_ARROW,0,2},
			{EGO_LIGHTNING_BOLT,TV_BOLT,0,2},
		},

		{
			{TV_BOTTLE,1,10,
			 TV_POTION,SV_POTION_ENLIGHTENMENT},
			{TV_WAND,SV_WAND_NOTHING,2,
			 TV_WAND,SV_WAND_ELEC_BALL},
			{TV_WAND,SV_WAND_ELEC_BALL,2,
			 TV_WAND,SV_WAND_ELEC_BALL},
			{TV_STAFF,SV_STAFF_NOTHING,1,
			 TV_STAFF,SV_STAFF_STARLITE},
			{TV_STAFF,SV_STAFF_STARLITE,1,
			 TV_STAFF,SV_STAFF_STARLITE},
			{TV_ROD,SV_ROD_NOTHING,15,
			 TV_ROD,SV_ROD_ELEC_BALL},
			{TV_RING,SV_RING_NOTHING,6,
			 TV_RING,SV_RING_RES_BLINDNESS},
			{TV_AMULET,SV_AMULET_NOTHING,40,
			 TV_AMULET,SV_AMULET_REFLECTION},
			{TV_SCROLL,SV_SCROLL_NOTHING,3,
			 TV_SCROLL,SV_SCROLL_RECHARGING},
		},
	},
	{SV_BATERIE_MANA,
		{
			{EGO_FREE_ACTION,TV_GLOVES,0,15},
			{EGO_MOTION,TV_BOOTS,0,15},
			{EGO_DF,TV_SWORD,ALCHEMIST_ENCHANT_DAM | ALCHEMIST_ENCHANT_PVAL | ALCHEMIST_ENCHANT_AC,23},
			{EGO_DF,TV_POLEARM,ALCHEMIST_ENCHANT_DAM | ALCHEMIST_ENCHANT_PVAL | ALCHEMIST_ENCHANT_AC,23},
			{EGO_DF,TV_HAFTED,ALCHEMIST_ENCHANT_DAM | ALCHEMIST_ENCHANT_PVAL | ALCHEMIST_ENCHANT_AC,23},
			{EGO_DF,TV_AXE,ALCHEMIST_ENCHANT_DAM | ALCHEMIST_ENCHANT_PVAL | ALCHEMIST_ENCHANT_AC,23},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
		},

		{
			{TV_BOTTLE,1,1,
			 TV_POTION,SV_POTION_SLEEP},
			{TV_WAND,SV_WAND_NOTHING,1,
			 TV_WAND,SV_WAND_SLEEP_MONSTER},
			{TV_WAND,SV_WAND_SLEEP_MONSTER,1,
			 TV_WAND,SV_WAND_SLEEP_MONSTER},
			{TV_STAFF,SV_STAFF_NOTHING,2,
			 TV_STAFF,SV_STAFF_SLEEP_MONSTERS},
			{TV_STAFF,SV_STAFF_SLEEP_MONSTERS,2,
			 TV_STAFF,SV_STAFF_SLEEP_MONSTERS},
			{TV_ROD,SV_ROD_NOTHING,10,
			 TV_ROD,SV_ROD_MAPPING},
			{TV_RING,SV_RING_NOTHING,6,
			 TV_RING,SV_RING_FREE_ACTION},
			{TV_AMULET,SV_AMULET_NOTHING,15,
			 TV_AMULET,SV_AMULET_NO_MAGIC},
			{TV_SCROLL,SV_SCROLL_NOTHING,3,
			 TV_SCROLL,SV_SCROLL_STAR_ENCHANT_WEAPON},
		},
	},
};
/*
	{SV_BATERIE_,
		{
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
			{0,0,0,1},
		},

		{
			{0,0,0,
			 0,0},
			{0,0,0,
			 0,0},
			{0,0,0,
			 0,0},
			{0,0,0,
			 0,0},
			{0,0,0,
			 0,0},
			{0,0,0,
			 0,0},
			{0,0,0,
			 0,0},
			{0,0,0,
			 0,0},
			{0,0,0,
			 0,0},
		},
	},
*/

/*
 * Textual translation of your god's "niceness".
 */

cptr deity_niceness[10] =
{
	"a lovable deity",
	"a friendly deity",
	"an easygoing deity",
	"a forgiving deity",
	"an uncaring deity",
	"a wary deity",
	"an unforgiving deity",
	"an impatient deity",
	"a wrathful deity",
	"an easily angered deity"
};

/*
 * Textual translation of your standing with your god.
 */

cptr deity_standing[11] =
{
	"cursed",
	"persecuted",
	"punished",
	"despised",
	"disliked",
	"watched",
	"unnoticed",
	"noticed",
	"rewarded",
	"favored",
	"championed"
};

deity_type deity_info[MAX_GODS] =
{
	{
		"Nobody",
		{
			"Atheist",
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
		},
	},
	{
		"Eru Iluvatar",
		{
			"He is the supreme god, he created the world, and most of its inhabitants.",
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
		},
	},
	{
		"Manwe Sulimo",
		{
			"He is the king of the Valar, most powerful of them after Melkor.",
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
		},
	},
	{
		"Tulkas",
		{
			"He is the last of the Valar that came to the world, and the fiercest fighter.",
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
		},
	},
	{
		"Melkor Bauglir",
		{
			"He is the most powerful of the Valar. He became corrupted and he's now \n"
			"the greatest threat of Arda, he is also known as Morgoth, the dark enemy.",
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
		},
	},
};

/* jk - to hit, to dam, to ac, to stealth, to disarm, to saving throw */
/* this concept is taken from Adom, where Thomas Biskup thought it out, */
/* as far as I know. */
tactic_info_type tactic_info[9] =
{
/*        hit  dam   ac stl  dis  sav */
	{ -10, -10, +15, +3, +15, +14, "coward"},           /* 4-4 */
	{  -8,  -8, +10, +2,  +9,  +9, "meek"},             /* 4-3 */
	{  -4,  -4,  +5, +1,  +5,  +5, "wary"},             /* 4-2 */
	{  -2,  -2,  +2, +1,  +2,  +2, "careful"},          /* 4-1 */
	{   0,   0,   0,  0,   0,   0, "normal"},           /* 4+0 */
	{   2,   2,  -2, -1,  -2,  -3, "confident"},        /* 4+1 */
	{   4,   4,  -5, -2,  -5,  -7, "aggressive"},       /* 4+2 */
	{   6,   6, -10, -3, -11, -12, "furious"},          /* 4+3 */
	{   8,  12, -25, -5, -18, -18, "berserker"}         /* 4+4 */
};

/*
 * Random artifact activations.
 */
activation activation_info[MAX_T_ACT] =
{
	{ "death", 0, ACT_DEATH },
	{ "ruination", 0, ACT_RUINATION },
	{ "destruction", 0, ACT_DESTRUC },
	{ "stupidity", 0, ACT_UNINT },
	{ "weakness", 0, ACT_UNSTR },
	{ "unhealth", 0, ACT_UNCON },
	{ "ugliness", 0, ACT_UNCHR },
	{ "clumsiness", 0, ACT_UNDEX },
	{ "naivete", 0, ACT_UNWIS },
	{ "stat loss", 0, ACT_STATLOSS },
	{ "huge stat loss", 0, ACT_HISTATLOSS },
	{ "experience loss", 0, ACT_EXPLOSS },
	{ "huge experience loss", 0, ACT_HIEXPLOSS },
	{ "teleportation", 1000, ACT_TELEPORT },
	{ "monster summoning", 5, ACT_SUMMON_MONST },
	{ "paralyzation", 0, ACT_PARALYZE },
	{ "hallucination", 100, ACT_HALLU },
	{ "poisoning", 0, ACT_POISON },
	{ "hunger", 0, ACT_HUNGER },
	{ "stun", 0, ACT_STUN },
	{ "cuts", 0, ACT_CUTS },
	{ "paranoia", 0, ACT_PARANO },
	{ "confusion", 0, ACT_CONFUSION },
	{ "blindness", 0, ACT_BLIND },
	{ "pet summoning", 1010, ACT_PET_SUMMON },
	{ "cure paralyzation", 5000, ACT_CURE_PARA },
	{ "cure hallucination", 1000, ACT_CURE_HALLU },
	{ "cure poison", 1000, ACT_CURE_POISON },
	{ "cure hunger", 1000, ACT_CURE_HUNGER },
	{ "cure stun", 1000, ACT_CURE_STUN },
	{ "cure cut", 1000, ACT_CURE_CUTS },
	{ "cure fear", 1000, ACT_CURE_FEAR },
	{ "cure confusion", 1000, ACT_CURE_CONF },
	{ "cure blindness", 1000, ACT_CURE_BLIND },
	{ "cure light wounds", 500, ACT_CURE_LW },
	{ "cure serious wounds", 750, ACT_CURE_MW },
	{ "cure critical wounds", 1000, ACT_CURE_700 },
	{ "curing", 1100, ACT_CURING },
	{ "genocide", 5000, ACT_GENOCIDE },
	{ "mass genocide", 10000, ACT_MASS_GENO },
	{ "restoration", 2000, ACT_REST_ALL },
	{ "light", 1000, ACT_LIGHT },
	{ "darkness", 0, ACT_DARKNESS },
	{ "teleportation", 1000, ACT_TELEPORT },
	{ "level teleportation", 500, ACT_LEV_TELE },
	{ "acquirement", 30000, ACT_ACQUIREMENT },
	{ "something weird", 50, ACT_WEIRD },
	{ "aggravation", 0, ACT_AGGRAVATE },
	{ "corruption", 100, ACT_MUT },
	{ "cure insanity", 2000, ACT_CURE_INSANITY },
	{ "cure corruption", 2000, ACT_CURE_MUT },
	{ "light absortion", 800, ACT_LIGHT_ABSORBTION },
};

/*
 * Possible Musics.
 */
music music_info[MAX_MUSICS] =
{
	{"singing a song of freedom",MUSIC_BETWEEN,10,40,10,1,10},
	{"singing a charming song",MUSIC_CHARME,6,60,20,1,15},
	{"singing a knowledge song",MUSIC_ID,6,50,5,2,10},
	{"singing a *knowledge* song",MUSIC_STAR_ID,2,100,50,4,100}, /* Never random, only for the Harp of Master Robinton */
	{"singing a beautiful song",MUSIC_NONE,0,0,0,1,1},
	{"singing a hiding song",MUSIC_HIDE,20,50,8,3,6},
	{"singing a song of brightness",MUSIC_LITE,60,20,4,1,5},
	{"singing a immaterial song",MUSIC_SHADOW,30,50,2,5,15},
	{"singing a godness song",MUSIC_HOLY,20,100,20,6,15},
	{"singing a *godness* song",MUSIC_HOLY,30,100,20,5,100}, /* Never random, only for the Drum of Piemur */
	{"singing a *charming* song",MUSIC_CHARME,20,100,20,1,100}, /* Never random, only for the Flute of Menolly */
};

/*
 * Possible movement type.
 */
move_info_type move_info[9] =
{
/*        speed, searching, stealth, perception */
	{ -10,     17,        4,      20, "slug-like"},
	{  -8,     12,        4,      16, "very slow"},
	{  -6,      8,        3,      10, "slow"},
	{  -3,      4,        2,       6, "leisurely"},
	{   0,      0,        0,       0, "normal"},
	{   1,     -4,       -1,      -4, "brisk"},
	{   2,     -6,       -4,      -8, "fast"},
	{   3,    -10,       -7,     -14, "very fast"},
	{   4,    -16,      -10,     -20, "running"}
};

/*
 * Possible inscriptions type.
 */
inscription_info_type inscription_info[MAX_INSCRIPTIONS] =
{
	{       /* Nothing */
		"",
		0,
		TRUE,
		0,
	},
	{       /* Light up the room(Adunaic) */
		"ure nimir", /* sun shine */
		INSCRIP_EXEC_ENGRAVE | INSCRIP_EXEC_WALK | INSCRIP_EXEC_MONST_WALK,
		FALSE,
		30,
	},
	{       /* Darkness in room(Adunaic) */
		"lomi gimli", /* night stars */
		INSCRIP_EXEC_ENGRAVE | INSCRIP_EXEC_WALK | INSCRIP_EXEC_MONST_WALK,
		FALSE,
		10,
	},
	{       /* Storm(Adunaic) */
		"dulgi bawiba", /* black winds */
		INSCRIP_EXEC_ENGRAVE | INSCRIP_EXEC_WALK | INSCRIP_EXEC_MONST_WALK,
		FALSE,
		40,
	},
	{       /* Protection(Sindarin) */
		"pedo mellon a minno", /* say friend and enter */
		INSCRIP_EXEC_MONST_WALK,
		FALSE,
		8,
	},
	{       /* Dwarves summoning(Khuzdul) */
		"Baruk Khazad! Khazad aimenu!", /* Axes of the Dwarves, the Dwarves are upon you! */
		INSCRIP_EXEC_ENGRAVE,
		FALSE,
		100,
	},
	{       /* Open Chasm(Nandorin) */
		"dunna hrassa", /* black precipice */
		INSCRIP_EXEC_MONST_WALK,
		FALSE,
		50,
	},
	{       /* Blast of Black Fire(Orkish) */
		"burz ghash ronk", /* black fire pool */
		INSCRIP_EXEC_ENGRAVE | INSCRIP_EXEC_WALK | INSCRIP_EXEC_MONST_WALK,
		FALSE,
		60,
	},
};

/*
 * Inscriptions for pseudo-id
 */
cptr sense_desc[] =
{
	"whoops",
	"cursed",
	"average",
	"good",
	"good",
	"excellent",
	"worthless",
	"terrible",
	"special",
	"broken",
	""
};

/*
 * Flag groups used for art creation, level gaining weapons, ...
 * -----
 * Name,
 * Price,
 * Flags 1,
 * Flags 2,
 * Flags 3,
 * Flags 4,
 * ESP,
 */
flags_group flags_groups[MAX_FLAG_GROUP] =
{
	{
		"Fire",
		TERM_L_RED,
		1,
		TR1_SLAY_UNDEAD | TR1_BRAND_FIRE,
		TR2_RES_FIRE,
		TR3_SH_FIRE | TR3_LITE1 | TR3_IGNORE_FIRE,
		0,
		0,
	},
	{
		"Cold",
		TERM_WHITE,
		1,
		TR1_SLAY_DRAGON | TR1_SLAY_DEMON | TR1_BRAND_COLD,
		TR2_RES_COLD | TR2_INVIS,
		TR3_SLOW_DIGEST | TR3_IGNORE_COLD,
		0,
		0,
	},
	{
		"Acid",
		TERM_GREEN,
		1,
		TR1_SLAY_ANIMAL | TR1_IMPACT | TR1_TUNNEL | TR1_BRAND_ACID,
		TR2_RES_ACID,
		TR3_IGNORE_ACID,
		0,
		0,
	},
	{
		"Lightning",
		TERM_L_BLUE,
		1,
		TR1_SLAY_EVIL | TR1_BRAND_ELEC,
		TR2_RES_ELEC,
		TR3_IGNORE_ELEC | TR3_SH_ELEC | TR3_TELEPORT,
		0,
		0,
	},
	{
		"Poison",
		TERM_L_GREEN,
		2,
		TR1_CHR | TR1_VAMPIRIC | TR1_SLAY_ANIMAL | TR1_BRAND_POIS,
		TR2_SUST_CHR | TR2_RES_POIS,
		TR3_DRAIN_EXP,
		0,
		ESP_TROLL | ESP_GIANT,
	},
	{
		"Air",
		TERM_BLUE,
		5,
		TR1_WIS | TR1_STEALTH | TR1_INFRA | TR1_SPEED,
		TR2_RES_LITE | TR2_RES_DARK | TR2_RES_BLIND | TR2_SUST_WIS,
		TR3_FEATHER | TR3_SEE_INVIS | TR3_BLESSED,
		0,
		ESP_GOOD,
	},
	{
		"Earth",
		TERM_L_UMBER,
		5,
		TR1_STR | TR1_CON | TR1_TUNNEL | TR1_BLOWS | TR1_SLAY_TROLL | TR1_SLAY_GIANT | TR1_IMPACT,
		TR2_SUST_STR | TR2_SUST_CON | TR2_FREE_ACT | TR2_RES_FEAR | TR2_RES_SHARDS,
		TR3_REGEN,
		0,
		ESP_TROLL | ESP_GIANT,
	},
	{
		"Mind",
		TERM_YELLOW,
		7,
		TR1_INT | TR1_SEARCH,
		TR2_SUST_INT | TR2_RES_CONF | TR2_RES_FEAR,
		0,
		0,
		ESP_ORC | ESP_TROLL | ESP_GIANT | ESP_ANIMAL | ESP_UNIQUE | ESP_SPIDER | ESP_DEMON,
	},
	{
		"Shield",
		TERM_RED,
		7,
		TR1_DEX,
		TR2_SUST_DEX | TR2_INVIS | TR2_REFLECT | TR2_HOLD_LIFE | TR2_RES_SOUND | TR2_RES_NEXUS,
		TR3_REGEN,
		0,
		0,
	},
	{
		"Chaos",
		TERM_VIOLET,
		7,
		TR1_CHAOTIC | TR1_IMPACT,
		TR2_RES_CHAOS | TR2_RES_DISEN,
		TR3_REGEN,
		0,
		ESP_ALL,
	},
	{
		"Magic",
		TERM_L_BLUE,
		10,
		TR1_MANA | TR1_SPELL,
		TR2_RES_CHAOS | TR2_RES_DISEN,
		TR3_WRAITH,
		TR4_PRECOGNITION | TR4_FLY | TR4_CLONE,
		0,
	},
	{
		"Antimagic",
		TERM_L_DARK,
		10,
		TR1_VAMPIRIC | TR1_CHAOTIC | TR1_BLOWS | TR1_SPEED,
		TR2_LIFE | TR2_REFLECT | TR2_FREE_ACT | TR2_HOLD_LIFE,
		TR3_NO_MAGIC | TR3_NO_TELE | TR3_SEE_INVIS,
		TR4_ANTIMAGIC_10 | TR4_ANTIMAGIC_20,
		0,
	},
};

/* Power, the first 32 are the corruptions */
power_type powers_type_init[POWER_MAX_INIT] =
{
	{
		"spit acid",
		"You can spit acid.",
		"You gain the ability to spit acid.",
		"You lose the ability to spit acid.",
		9, 9, A_DEX, 15,
	},
	{
		"fire breath",
		"You can breath fire.",
		"You gain the ability to breathe fire.",
		"You lose the ability to breathe fire.",
		20, 10, A_CON, 18,
	},
	{
		"hypnotic gaze",
		"Your gaze is hypnotic.",
		"Your eyes look mesmerizing...",
		"Your eyes look uninteresting.",
		12, 12, A_CHR, 18,
	},
	{
		"telekinesis",
		"You are telekinetic.",
		"You gain the ability to move objects telekinetically.",
		"You lose the ability to move objects telekinetically.",
		9, 9, A_WIS, 14,
	},
	{
		"teleport",
		"You can teleport at will.",
		"You gain the power of teleportation at will.",
		"You lose the power of teleportation at will.",
		7, 7, A_WIS, 15,
	},
	{
		"mind blast",
		"You can mind blast your enemies.",
		"You gain the power of Mind Blast.",
		"You lose the power of Mind Blast.",
		5, 3, A_WIS, 15,
	},
	{
		"emit radiation",
		"You can emit hard radiation at will.",
		"You start emitting hard radiation.",
		"You stop emitting hard radiation.",
		15, 15, A_CON, 14,
	},
	{
		"vampiric drain",
		"You can drain life from a foe.",
		"You become vampiric.",
		"You are no longer vampiric.",
		4, 5, A_CON, 9,
	},
	{
		"smell metal",
		"You can smell nearby precious metal.",
		"You smell a metallic odour.",
		"You no longer smell a metallic odour.",
		3, 2, A_INT, 12,
	},
	{
		"smell monsters",
		"You can smell nearby monsters.",
		"You smell filthy monsters.",
		"You no longer smell filthy monsters.",
		5, 4, A_INT, 15,
	},
	{
		"blink",
		"You can teleport yourself short distances.",
		"You gain the power of minor teleportation.",
		"You lose the power of minor teleportation.",
		3, 3, A_WIS, 12,
	},
	{
		"eat rock",
		"You can consume solid rock.",
		"The walls look delicious.",
		"The walls look unappetizing.",
		8, 12, A_CON, 18,
	},
	{
		"swap position",
		"You can switch locations with another being.",
		"You feel like walking a mile in someone else's shoes.",
		"You feel like staying in your own shoes.",
		15, 12, A_DEX, 16,
	},
	{
		"shriek",
		"You can emit a horrible shriek.",
		"Your vocal cords get much tougher.",
		"Your vocal cords get much weaker.",
		4, 4, A_CON, 6,
	},
	{
		"illuminate",
		"You can emit bright light.",
		"You can light up rooms with your presence.",
		"You can no longer light up rooms with your presence.",
		3, 2, A_INT, 10,
	},
	{
		"detect curses",
		"You can feel the danger of evil magic.",
		"You can feel evil magics.",
		"You can no longer feel evil magics.",
		7, 14, A_WIS, 14,
	},
	{
		"berserk",
		"You can drive yourself into a berserk frenzy.",
		"You feel a controlled rage.",
		"You no longer feel a controlled rage.",
		8, 8, A_STR, 14,
	},
	{
		"polymorph",
		"You can polymorph yourself at will.",
		"Your body seems mutable.",
		"Your body seems stable.",
		18, 20, A_CON, 18,
	},
	{
		"midas touch",
		"You can turn ordinary items to gold.",
		"You gain the Midas touch.",
		"You lose the Midas touch.",
		10, 5, A_INT, 12,
	},
	{
		"grow mold",
		"You can cause mold to grow near you.",
		"You feel a sudden affinity for mold.",
		"You feel a sudden dislike for mold.",
		1, 6, A_CON, 14,
	},
	{
		"resist elements",
		"You can harden yourself to the ravages of the elements.",
		"You feel like you can protect yourself.",
		"You feel like you might be vulnerable.",
		10, 12, A_CON, 12,
	},
	{
		"earthquake",
		"You can bring down the dungeon around your ears.",
		"You gain the ability to wreck the dungeon.",
		"You lose the ability to wreck the dungeon.",
		12, 12, A_STR, 16,
	},
	{
		"eat magic",
		"You can consume magic energy for your own use.",
		"Your magic items look delicious.",
		"Your magic items no longer look delicious.",
		17, 1, A_WIS, 15,
	},
	{
		"weigh magic",
		"You can feel the strength of the magics affecting you.",
		"You feel you can better understand the magic around you.",
		"You no longer sense magic.",
		6, 6, A_INT, 10,
	},
	{
		"sterilize",
		"You can cause mass impotence.",
		"You can give everything around you a headache.",
		"You hear a massed sigh of relief.",
		20, 40, A_CHR, 18,
	},
	{
		"panic hit",
		"You can run for your life after hitting something.",
		"You suddenly understand how thieves feel.",
		"You no longer feel jumpy.",
		10, 12, A_DEX, 14,
	},
	{
		"dazzle",
		"You can emit confusing, blinding radiation.",
		"You gain the ability to emit dazzling lights.",
		"You lose the ability to emit dazzling lights.",
		7, 15, A_CHR, 8,
	},
	{
		"spear of darkness",
		"You can create a spear of darkness.",
		"An illusory spear of darkness appears in your hand.",
		"The spear of darkness disappear.",
		7, 10, A_WIS, 9,
	},
	{
		"recall",
		"You can travel between towns and the depths.",
		"You feel briefly homesick, but it passes.",
		"You feel briefly homesick.",
		17, 50, A_INT, 16,
	},
	{
		"banish evil",
		"You can send evil creatures directly to the Nether Realm.",
		"You feel a holy wrath fill you.",
		"You no longer feel a holy wrath.",
		25, 25, A_WIS, 18,
	},
	{
		"cold touch",
		"You can freeze things with a touch.",
		"Your hands get very cold.",
		"Your hands warm up.",
		2, 2, A_CON, 11,
	},
	{
		"throw object",
		"You can hurl objects with great force.",
		"Your throwing arm feels much stronger.",
		"Your throwing arm feels much weaker.",
		1, 10, A_STR, 6,
	},

	/* Others */
	{
		"find secret passages",
		"You can use secret passages.",
		"You suddenly notice lots of hidden ways.",
		"You no longer can use hidden ways.",
		15, 15, A_DEX, 12,
	},
	{
		"detect doors and traps",
		"You can detect hidden doors and traps.",
		"You grow an affinity for traps.",
		"You no longer can detect hidden doors and traps.",
		5, 5, A_WIS, 10,
	},
	{
		"create food",
		"You can create food.",
		"Your cooking skills greatly improves.",
		"Your cooking skills returns to a normal level.",
		15, 10, A_INT, 10,
	},
	{
		"remove fear",
		"You can embolden yourself.",
		"You feel your fears lessening.",
		"You feel your fears growing again.",
		3, 5, A_WIS, 8,
	},
	{
		"set explosive rune",
		"You can set explosive runes.",
		"You suddenly understand how explosive runes work.",
		"You suddenly forget how explosive runes work.",
		25, 35, A_INT, 15,
	},
	{
		"stone to mud",
		"You can destroy walls.",
		"You could destroy walls.",
		"You cannot destroy walls anymore.",
		20, 10, A_STR, 12,
	},
	{
		"poison dart",
		"You can throw poisoned darts.",
		"You get an infinite supply of poisoned darts.",
		"You lose your infinite supply of poisoned darts.",
		12, 8, A_DEX, 14,
	},
	{
		"magic missile",
		"You can cast magic missiles.",
		"You suddenly understand the basics of magic.",
		"You forgot the basics of magic.",
		2, 2, A_INT, 9,
	},
	{
		"grow trees",
		"You can grow trees.",
		"You feel an affinity for trees.",
		"You no longer feel an affinity for trees.",
		2, 6, A_CHR, 3,
	},
	{
		"cold breath",
		"You can breath cold.",
		"You gain the ability to breathe cold.",
		"You lose the ability to breathe cold.",
		20, 10, A_CON, 18,
	},
	{
		"chaos breath",
		"You can breath chaos.",
		"You gain the ability to breathe chaos.",
		"You lose the ability to breathe chaos.",
		20, 10, A_CON, 18,
	},
	{
		"elemental breath",
		"You can breath the elements.",
		"You gain the ability to breathe the elements.",
		"You lose the ability to breathe the elements.",
		20, 10, A_CON, 18,
	},
	{
		"change the world",
		"You can wreck the world around you.",
		"You gain the ability to wreck the world.",
		"You lose the ability to wreck the world.",
		1, 30, A_CHR, 6,
	},
	{
		"scare monster",
		"You can scare monsters.",
		"You gain the ability to scare monsters.",
		"You lose the ability to scare monsters.",
		4, 3, A_INT, 3,
	},
	{
		"restore life",
		"You can restore lost life forces.",
		"You gain the ability to restore your life force.",
		"You lose the ability to restore your life force.",
		30, 30, A_WIS, 18,
	},
	{
		"summon monsters",
		"You can call upon monsters.",
		"You gain the ability to call upon monsters.",
		"You lose the ability to call upon monsters.",
		0, 0, 0, 0,
	},
	{
		"necromantic powers",
		"You can use the foul necromantic magic.",
		"You gain the ability to use the foul necromantic magic.",
		"You lose the ability to use the foul necromantic magic.",
		0, 0, 0, 0,
	},
	{
		"Rohan Knight's Powers",
		"You can use rohir powers.",
		"You gain the ability to use rohir powers.",
		"You lose the ability to use rohir powers.",
		0, 0, 0, 0,
	},
	{
		"Thunderlord's Powers",
		"You can use thunderlords powers.",
		"You gain the ability to use thunderlords powers.",
		"You lose the ability to use thunderlords powers.",
		0, 0, 0, 0,
	},
	{
		"Death Mold's Powers",
		"You can use the foul deathmold magic.",
		"You gain the ability to use the foul deathmold magic.",
		"You lose the ability to use the foul deathmold magic.",
		0, 0, 0, 0,
	},
	{
		"Hypnotize Pet",
		"You can mystify pets.",
		"You gain the ability to mystify pets.",
		"You lose the ability to mystify pets.",
		0, 0, 0, 0,
	},
	{
		"Awaken hypnotized pet",
		"You can wake up a pet.",
		"You gain the ability to wake up a pet.",
		"You lose the ability to wake up a pet.",
		0, 0, 0, 0,
	},
	{
		"Incarnate",
		"You can incarnate into a body.",
		"You feel the need to get a body.",
		"You no longer feel the need for a new body.",
		0, 0, 0, 0,
	},
	{
		"magic map",
		"You can sense what is beyond walls.",
		"You feel you can sense what is beyond walls.",
		"You no longer can sense what is beyond walls.",
		7, 10, A_WIS, 15,
	},
	{
		"lay trap",
		"You can lay monster traps.",
		"You suddenly understand how rogues work.",
		"You no longer suddenly understand how rogues work.",
		1, 1, A_DEX, 1,
	},
	{
		"Merchant abilities",
		"You can request items and get loans.",
		"From now on you can use the merchant abilities.",
		"You can no longer use the merchant abilities.",
		0, 0, 0, 0,
	},
	{
		"turn pet into companion",
		"You can turn a pet into a companion.",
		"You suddenly gain authority over your pets.",
		"You can no longer convert pets into companions.",
		2, 10, A_CHR, 10,
	},
	{
		"turn into a bear",
		"You can turn into a bear.",
		"You suddenly gain beornings powers.",
		"You can no longer shapeshift into a bear.",
		2, 5, A_CON, 5,
	},
	{
		"sense dodge success",
		"You can sense your dodging success chance.",
		"You suddenly can sense your dodging success chance.",
		"You can no longer sense your dodging success chance.",
		0, 0, 0, 0,
	},
};

/*
 * The Quests
 */
quest_type quest_init[MAX_Q_IDX_INIT] =
{
	{
		FALSE,
		FALSE,
		"",
		{
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
		},
		QUEST_STATUS_UNTAKEN,
		0,

		NULL,
		HOOK_TYPE_C,
		quest_null_hook,
		{0, 0},
	},
	{
		FALSE,
		FALSE,
		"Dol Guldur",
		{
			"The forest of Mirkwood is a very dangerous place to go, mainly due to",
			"the activities of the Necromancer that lurks in Dol Guldur.",
			"Find him, and free Mirkwood from his spells.",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
		},
		QUEST_STATUS_TAKEN,
		70,

		&plots[PLOT_MAIN],
		HOOK_TYPE_C,
		quest_necro_init_hook,
		{0, 0},
	},
	{
		FALSE,
		FALSE,
		"Sauron",
		{
			"It is time to take the battle to Morgoth. But, before you can",
			"reach it, you must find and kill Sauron.  Only after defeating",
			"the powerful sorcerer will the stairs leading to the Morgoth's",
			"room be opened.",
			"",
			"",
			"",
			"",
			"",
			"",
		},
		QUEST_STATUS_UNTAKEN,
		99,

		&plots[PLOT_MAIN],
		HOOK_TYPE_C,
		quest_sauron_init_hook,
		{0, 0},
	},
	{
		FALSE,
		FALSE,
		"Morgoth",
		{
			"Your final quest is the ultimate quest that has always been",
			"required of you. You must enter the last level of Angband, where",
			"Morgoth is waiting. Travel deep, and defeat this source of all our",
			"problems.Be prepared, be patient, and good luck. May the light",
			"shine on you.",
			"",
			"",
			"",
			"",
			"",
		},
		QUEST_STATUS_UNTAKEN,
		100,

		&plots[PLOT_MAIN],
		HOOK_TYPE_C,
		quest_morgoth_init_hook,
		{0, 0},
	},

	/* Bree plot */
	{
		FALSE,
		FALSE,
		"Thieves!!!",
		{
			"There are thieves robbing my people! They live in a small",
			"burrow outside the city walls robbing people as they pass",
			"on the roads. They even sneak inside the town to mug and",
			"pillage! Your task is to go to the burrow and kill these",
			"ruffians.",
			"",
			"",
			"",
			"",
			"",
		},
		QUEST_STATUS_UNTAKEN,
		5,

		&plots[PLOT_BREE],
		HOOK_TYPE_C,
		quest_thieves_init_hook,
		{0, 0},
	},

	{
		FALSE,
		TRUE,
		"Random Quest",
		{
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
		},
		QUEST_STATUS_UNTAKEN,
		5,

		NULL,
		HOOK_TYPE_C,
		quest_random_init_hook,
		{0, 0},
	},

	{
		FALSE,
		FALSE,
		"Lost Hobbit",
		{
			"Merton Proundfoot, a young hobbit, seems to have disappeared.",
			"Last time someone saw him was near the horrible maze to the south...",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
		},
		QUEST_STATUS_UNTAKEN,
		25,

		&plots[PLOT_OTHER],
		HOOK_TYPE_C,
		quest_hobbit_init_hook,
		{0, 0},
	},

	{
		FALSE,
		FALSE,
		"The Dark Horseman",
		{
			"A dark cloaked horseman has been spotted several time in town.",
			"He carries an aura of fear with him and people seems to get sick",
			"where he goes. Please do something, but beware...",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
		},
		QUEST_STATUS_UNTAKEN,
		40,

		&plots[PLOT_BREE],
		HOOK_TYPE_C,
		quest_nazgul_init_hook,
		{0, 0},
	},

	{
		FALSE,
		FALSE,
		"The Trolls Glade",
		{
			"A group of Forest Trolls settled in an abandoned forest in the",
			"south east of our town. They are killing our people, so it's",
			"your mission to put an end to this! You may only find them",
			"when night comes... Local hobbits claim that the mighty swords",
			"Orcrist and Glamdring can be found there! Bring back one of",
			"them as a proof! But you will have to wait for the nightfall to",
			"find them.",
			"",
			"",
			"",
		},
		QUEST_STATUS_UNTAKEN,
		30,

		&plots[PLOT_BREE],
		HOOK_TYPE_C,
		quest_troll_init_hook,
		{FALSE, 0},
	},

	{
		FALSE,
		FALSE,
		"The Wight Grave",
		{
			"The Barrow-Downs hides lots of mysteries and dangers...",
			"Lately lots of people, men and hobbits, have disappeared there.",
			"Please put an end to this threat!",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
		},
		QUEST_STATUS_UNTAKEN,
		30,

		&plots[PLOT_BREE],
		HOOK_TYPE_C,
		quest_wight_init_hook,
		{FALSE, 0},
	},

	/* Lorien plot */
	{
		FALSE,
		FALSE,
		"Spiders of Mirkwood",
		{
			"Powers lurk deep within Mirkwood. Spiders have blocked the",
			"path through the forest, and Thranduil's folk have been",
			"unable to hold them off. It is your task to drive them",
			"away. Be careful -- many traps have been laid by their",
			"webs, and their venom is dangerous indeed.",
			"",
			"",
			"",
			"",
			"",
		},
		QUEST_STATUS_UNTAKEN,
		25,

		&plots[PLOT_LORIEN],
		HOOK_TYPE_C,
		quest_spider_init_hook,
		{0, 0},
	},
	{
		FALSE,
		FALSE,
		"Poisoned water",
		{
			"A curse has beset Lothlorien. All trees along the shorelines of Nimrodel",
			"are withering away. We fear it would spread to the whole forest. The",
			"cause seems to be an unknown poison. You are to go to the West and travel",
			"along Celebrane and Nimrodel until you discover the source of poisoning.",
			"Then you must destroy it and drop these potions on the tainted water.",
			"",
			"",
			"",
			"",
			"",
		},
		QUEST_STATUS_UNTAKEN,
		30,

		&plots[PLOT_LORIEN],
		HOOK_TYPE_C,
		quest_poison_init_hook,
		{0, 0},
	},
	/* Other quests */
	{
		FALSE,
		FALSE,
		"The Broken Sword",
		{
			"You have found Narsil, a broken sword. It is said that the sword that",
			"was broken shall be reforged... Maybe it is this one.",
			"You should bring it to Aragorn at Minas Anor, he would sure know.",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
		},
		QUEST_STATUS_UNTAKEN,
		20,

		&plots[PLOT_OTHER],
		HOOK_TYPE_C,
		quest_narsil_init_hook,
		{0, 0},
	},
	/* Gondolin plot */
	{
		FALSE,
		FALSE,
		"Eol the Dark Elf",
		{
			"We have disturbing tidings. Eol the Dark Elf has come seeking his kin in",
			"Gondolin. We cannot let anyone pass the borders of the city without the",
			"King's leave. Go forth to the eastern mountains and apprehend him. If",
			"he resists, use whatever means possible to hinder him from reaching the",
			"city. Be wary -- the mountain caves may have many hidden traps.",
			"",
			"",
			"",
			"",
			"",
		},
		QUEST_STATUS_UNTAKEN,
		30,

		&plots[PLOT_GONDOLIN],
		HOOK_TYPE_C,
		quest_eol_init_hook,
		{0, 0},
	},
	{
		FALSE,
		FALSE,
		"Nirnaeth Arnoediad",
		{
			"The fortunes of war in the north turn against us.",
			"Morgoth's treachery has driven our armies back nigh",
			"to the city's walls. Go forth from the city gates",
			"and clear a path for them to retreat. You need not",
			"destroy the troll army, simply drive a path through.",
			"",
			"",
			"",
			"",
			"",
		},
		QUEST_STATUS_UNTAKEN,
		37,

		&plots[PLOT_GONDOLIN],
		HOOK_TYPE_C,
		quest_nirnaeth_init_hook,
		{0, 0},
	},
	{
		FALSE,
		FALSE,
		"Invasion of Gondolin",
		{
			"Morgoth is upon us! Dragons and Balrogs have poured over secret",
			"ways of the Echoriath, and are looking for our city. They are",
			"conducted by Maeglin! You must stop him or they will find us.",
			"Do not let maeglin get to the stairs or everything will be lost!",
			"Go now, be brave.",
			"",
			"",
			"",
			"",
			"",
		},
		QUEST_STATUS_UNTAKEN,
		80,

		&plots[PLOT_GONDOLIN],
		HOOK_TYPE_C,
		quest_invasion_init_hook,
		{0, 0},
	},
	{
		FALSE,
		FALSE,
		"The Last Alliance",
		{
			"The armies of Morgoth are closing in on the last remaining strongholds",
			"of resistance against him. We are too far apart to help each other.",
			"The arrival of our new Dragonrider allies has helped, but can only delay",
			"the inevitable. We must be able to stand together and reinforce each other,",
			"or both our kingdoms will fall separately. The Dragonriders have taught us",
			"how to use the Void Jumpgates: we need you to open a Void Jumpgate in our",
			"own city, and that of Gondolin.",
			"Simply travel to Gondolin, but beware of rebel dragonriders.",
			"",
			"",
		},
		QUEST_STATUS_UNTAKEN,
		60,

		&plots[PLOT_MINAS],
		HOOK_TYPE_C,
		quest_between_init_hook,
		{0, 0},
	},
	{
		FALSE,
		FALSE,
		"The One Ring",
		{
			"Find the One Ring, then bring it to Mount Doom, in Mordor to drop",
			"it in the Great Fire where it was once forged.",
			"But beware, *NEVER* use it, or you will be corrupted.",
			"Once destroyed you will be able to permanently defeat Sauron.",
			"The ring must be cast back into the fires of Mount Doom !",
			"",
			"",
			"",
			"",
			"",
		},
		QUEST_STATUS_UNTAKEN,
		99,

		&plots[PLOT_MAIN],
		HOOK_TYPE_C,
		quest_one_init_hook,
		{0, 0},
	},

	{
		FALSE,
		FALSE,
		"Mushroom supplies",
		{
			"Farmer Maggot asked you to bring him back his mushrooms.",
			"And do not harm the dogs.",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
		},
		QUEST_STATUS_UNTAKEN,
		3,

		&plots[PLOT_OTHER],
		HOOK_TYPE_C,
		quest_shroom_init_hook,
		{0, 0},
	},

	{
		FALSE,
		FALSE,
		"The prisoner of Dol Guldur",
		{
			"You keep hearing distress cries in the dark tower of",
			"Dol Guldur...",
			"Maybe there is someone being held prisoner and tortured!",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
		},
		QUEST_STATUS_UNTAKEN,
		60,

		&plots[PLOT_OTHER],
		HOOK_TYPE_C,
		quest_thrain_init_hook,
		{0, 0},
	},
};

/* List of powers for Symbiants/Powers */
monster_power monster_powers[96] =
{
	{ RF4_SHRIEK, "Aggravate Monster", 1, FALSE },
	{ RF4_MULTIPLY, "Multiply", 10, FALSE },
	{ RF4_S_ANIMAL, "Summon Animal", 30, FALSE },
	{ RF4_ROCKET, "Fire a Rocket", 40, TRUE },
	{ RF4_ARROW_1, "Light Arrow", 1, FALSE },
	{ RF4_ARROW_2, "Minor Arrow", 3, FALSE },
	{ RF4_ARROW_3, "Major Arrow", 7, TRUE },
	{ RF4_ARROW_4, "Great Arrow", 9, TRUE },
	{ RF4_BR_ACID, "Breathe Acid", 10, FALSE },
	{ RF4_BR_ELEC, "Breathe Lightning", 10, FALSE },
	{ RF4_BR_FIRE, "Breathe Fire", 10, FALSE },
	{ RF4_BR_COLD, "Breathe Cold", 10, FALSE },
	{ RF4_BR_POIS, "Breathe Poison", 15, TRUE },
	{ RF4_BR_NETH, "Breathe Nether", 30, TRUE },
	{ RF4_BR_LITE, "Breathe Light", 20, TRUE },
	{ RF4_BR_DARK, "Breathe Dark", 20, TRUE },
	{ RF4_BR_CONF, "Breathe Confusion", 15, TRUE },
	{ RF4_BR_SOUN, "Breathe Sound", 30, TRUE },
	{ RF4_BR_CHAO, "Breathe Chaos", 30, TRUE },
	{ RF4_BR_DISE, "Breathe Disenchantment", 30, TRUE },
	{ RF4_BR_NEXU, "Breathe Nexus", 30, TRUE },
	{ RF4_BR_TIME, "Breathe Time", 30, TRUE },
	{ RF4_BR_INER, "Breathe Inertia", 30, TRUE },
	{ RF4_BR_GRAV, "Breathe Gravity", 30, TRUE },
	{ RF4_BR_SHAR, "Breathe Shards", 30, TRUE },
	{ RF4_BR_PLAS, "Breathe Plasma", 30, TRUE },
	{ RF4_BR_WALL, "Breathe Force", 30, TRUE },
	{ RF4_BR_MANA, "Breathe Mana", 40, TRUE },
	{ RF4_BA_NUKE, "Nuke Ball", 30, TRUE },
	{ RF4_BR_NUKE, "Breathe Nuke", 40, TRUE },
	{ RF4_BA_CHAO, "Chaos Ball", 30, TRUE },
	{ RF4_BR_DISI, "Breathe Disintegration", 40, TRUE },

	{ RF5_BA_ACID, "Acid Ball", 8, FALSE },
	{ RF5_BA_ELEC, "Lightning Ball", 8, FALSE },
	{ RF5_BA_FIRE, "Fire Ball", 8, FALSE },
	{ RF5_BA_COLD, "Cold Ball", 8, FALSE },
	{ RF5_BA_POIS, "Poison Ball", 20, TRUE },
	{ RF5_BA_NETH, "Nether Ball", 20, TRUE },
	{ RF5_BA_WATE, "Water Ball", 20, TRUE },
	{ RF5_BA_MANA, "Mana Ball", 50, TRUE },
	{ RF5_BA_DARK, "Darkness Ball", 20, TRUE },
	{ 0, "(none)", 0, FALSE },
	{ 0, "(none)", 0, FALSE },
	{ 0, "(none)", 0, FALSE },
	{ RF5_CAUSE_1, "Cause Light Wounds", 20, FALSE },
	{ RF5_CAUSE_2, "Cause Medium Wounds", 30, FALSE },
	{ RF5_CAUSE_3, "Cause Critical Wounds", 35, TRUE },
	{ RF5_CAUSE_4, "Cause Mortal Wounds", 45, TRUE },
	{ RF5_BO_ACID, "Acid Bolt", 5, FALSE },
	{ RF5_BO_ELEC, "Lightning Bolt", 5, FALSE },
	{ RF5_BO_FIRE, "Fire Bolt", 5, FALSE },
	{ RF5_BO_COLD, "Cold Bolt", 5, FALSE },
	{ RF5_BO_POIS, "Poison Bolt", 10, TRUE },
	{ RF5_BO_NETH, "Nether Bolt", 15, TRUE },
	{ RF5_BO_WATE, "Water Bolt", 20, TRUE },
	{ RF5_BO_MANA, "Mana Bolt", 25, TRUE },
	{ RF5_BO_PLAS, "Plasma Bolt", 20, TRUE },
	{ RF5_BO_ICEE, "Ice Bolt", 20, TRUE },
	{ RF5_MISSILE, "Magic Missile", 1, FALSE },
	{ RF5_SCARE, "Scare", 4, FALSE },
	{ RF5_BLIND, "Blindness", 6, FALSE },
	{ RF5_CONF, "Confusion", 7, FALSE },
	{ RF5_SLOW, "Slowness", 10, FALSE },
	{ RF5_HOLD, "Paralyse", 10, FALSE },

	{ RF6_HASTE, "Haste Self", 50, FALSE },
	{ RF6_HAND_DOOM, "Hand of Doom", 30, TRUE },
	{ RF6_HEAL, "Healing", 60, FALSE },
	{ RF6_S_ANIMALS, "Summon Animals", 60, TRUE },
	{ RF6_BLINK, "Phase Door", 2, FALSE },
	{ RF6_TPORT, "Teleport", 10, FALSE },
	{ RF6_TELE_TO, "Teleport To", 20, TRUE },
	{ RF6_TELE_AWAY, "Teleport Away", 20, FALSE },
	{ RF6_TELE_LEVEL, "Teleport Level", 20, TRUE },
	{ RF6_DARKNESS, "Darkness", 3, FALSE },
	{ RF6_TRAPS, "Create Traps", 10, TRUE },
	{ 0, "(none)", 0, FALSE },
	{ RF6_RAISE_DEAD, "Raise the Dead", 400, TRUE },
	{ 0, "(none)", 0, FALSE },
	{ 0, "(none)", 0, FALSE },
	{ RF6_S_DRAGONRIDER, "Summon DragonRiders", 90, TRUE },
	{ RF6_S_KIN, "Summon Kin", 80, FALSE },
	{ RF6_S_HI_DEMON, "Summon Greater Demons", 90, TRUE },
	{ RF6_S_MONSTER, "Summon Monster", 50, FALSE },
	{ RF6_S_MONSTERS, "Summon Monsters", 60, TRUE },
	{ RF6_S_ANT, "Summon Ants", 30, FALSE },
	{ RF6_S_SPIDER, "Summon Spider", 30, FALSE },
	{ RF6_S_HOUND, "Summon Hound", 50, TRUE },
	{ RF6_S_HYDRA, "Summon Hydra", 40, TRUE },
	{ RF6_S_ANGEL, "Summon Angel", 60, TRUE },
	{ RF6_S_DEMON, "Summon Demon", 60, TRUE },
	{ RF6_S_UNDEAD, "Summon Undead", 70, TRUE },
	{ RF6_S_DRAGON, "Summon Dragon", 70, TRUE },
	{ RF6_S_HI_UNDEAD, "Summon High Undead", 90, TRUE },
	{ RF6_S_HI_DRAGON, "Summon High Dragon", 90, TRUE },
	{ RF6_S_WRAITH, "Summon Wraith", 90, TRUE },
	{ 0, "(none)", 0, FALSE },
};

/* Tval descriptions */
tval_desc tval_descs[] =
{
	{
		TV_BATERIE,
		"Essences contain the primitive magic forces which enable "
		"Alchemists using them to create powerful magic items from "
		"other magic items."
	},
	{
		TV_MSTAFF,
		"Mage Staves are the weapons of predilection of "
		"spellcasters, they all reduce spellcasting time to 80% of "
		"normal time and some will yield even greater powers."
	},
	{
		TV_FIRESTONE,
		"DragonRiders can feed their dragons with firestone to "
		"allow them to breath fire."
	},
	{
		TV_PARCHEMENT,
		"Parchments can contain useful information ... or useless "
		"junk."
	},
	{
		TV_EGG,
		"Eggs are laid by some monsters, if they hatch in your "
		"inventory the monster will be your friend."
	},
	{
		TV_TOOL,
		"Tools can be diggers, climbing equipment and such, they "
		"have their own slot in your inventory."
	},
	{
		TV_INSTRUMENT,
		"All instruments have a special magical song in them, "
		"activate them to play it. Harpers can use them to have "
		"two songs at a time."
	},
	{
		TV_BOOMERANG,
		"Boomerangs can be used instead of bows or slings, they "
		"are more like melee weapons than bows."
	},
	{
		TV_SHOT,
		"Shots are the standard ammunition for slings, you can "
		"wear them in your quiver if your have a sling equipped."
	},
	{
		TV_ARROW,
		"Arrows are the standard ammunition for bows, you can wear "
		"them in your quiver if your have a bow equipped."
	},
	{
		TV_BOLT,
		"Bolts are the standard ammunition for crossbows, you can "
		"wear them in your quiver if your have a crossbow equipped."
	},
	{
		TV_BOW,
		"Slings, bows and crossbows are used to attack monsters "
		"from a distance."
	},
	{
		TV_DIGGING,
		"Tools can be diggers, climbing equipment and such, they "
		"have their own slot in your inventory."
	},
	{
		TV_HAFTED,
		"Hafted weapons are melee weapons. Priests can use them "
		"without penalties."
	},
	{
		TV_SWORD,
		"Swords are melee weapons."
	},
	{
		TV_AXE,
		"Axes are melee weapons."
	},
	{
		TV_POLEARM,
		"Polearms are melee weapons."
	},
	{
		TV_DRAG_ARMOR,
		"Dragon armour is made from the scales of dead dragons, "
		"those mighty armour usually yields great power to their "
		"wearer."
	},
	{
		TV_LITE,
		"Lights are useful for they allow you to read things and "
		"see from afar. Some of them need to be refilled, some do "
		"not."
	},
	{
		TV_AMULET,
		"Amulets are fine pieces of jewelry, usually imbued with "
		"arcane magics."
	},
	{
		TV_RING,
		"Rings are fine pieces of jewelry, usually imbued with "
		"arcane magics."
	},
	{
		TV_TRAPKIT,
		"Trapping kits are used by rogues to set deadly monster "
		"traps."
	},
	{
		TV_STAFF,
		"Staves are objects imbued with mystical powers."
	},
	{
		TV_WAND,
		"Wands are like small staves and usually have a targeted "
		"effect."
	},
	{
		TV_ROD,
		"Rod tips are the physical bindings of some powerful "
		"spells, zap (attach) them to a rod to get a fully "
		"functional rod. Each spell takes some mana from the rod "
		"it is attached to to work."
	},
	{
		TV_ROD_MAIN,
		"Rods contain mana reserves used to cast spells in rod "
		"tips, zap(attach) a rod tip to them to get a fully "
		"functional rod. Each spell takes some mana from the rod "
		"it is attached to to work."
	},
	{
		TV_SCROLL,
		"Scrolls are magical parchments, imbued with magic spells, "
		"some are good some ... are not. When a scroll is read its "
		"magic is released and the scroll destroyed."
	},
	{
		TV_POTION,
		"Potions are magical liquids, some of them can be "
		"beneficial... some not."
	},
	{
		TV_POTION2,
		"Potions are magical liquids, some of them can be "
		"beneficial... some not."
	},
	{
		TV_FLASK,
		"Flasks of oil can be used to refill lanterns."
	},
	{
		TV_FOOD,
		"Everybody needs to eat, even you."
	},
	{
		TV_HYPNOS,
		"This monster seems to be hypnotized and friendly."
	},
	{
		TV_RANDART,
		"Those objects are only known by rumours, it is said that "
		"they can be activated for great or strange powers..."
	},
	{
		TV_RUNE1,
		"Runes are used by Runecrafters to create brand new spells."
	},
	{
		TV_RUNE2,
		"Runes are used by Runecrafters to create brand new spells."
	},
	{
		TV_JUNK,
		"Junk is usually worthless, though experienced archers can "
		"create ammo with them."
	},
	{
		TV_SKELETON,
		"It looks dead..."
	},
	{
		TV_BOTTLE,
		"An empty bottle. Maybe an alchemists could refill it."
	},
	{
		TV_SPIKE,
		"Spikes can be used to jam doors."
	},
	{
		TV_CORPSE,
		"It looks dead..."
	},
	{
		TV_BOOTS,
		"Boots can help your armour rating, some can be magical too."
	},
	{
		TV_GLOVES,
		"Handgear is used to protect hands, but non magical ones "
		"can sometimes hinder spellcasting. Also alchemists needs "
		"gloves in order to do alchemy."
	},
	{
		TV_HELM,
		"Headgear will protect your head."
	},
	{
		TV_CROWN,
		"Headgear will protect your head."
	},
	{
		TV_SHIELD,
		"Shields will help improve your defence rating, but you "
		"cannot use them with two handed weapons."
	},
	{
		TV_CLOAK,
		"Cloaks can shield you from damage, sometimes also "
		"providing magical powers."
	},
	{
		TV_SOFT_ARMOR,
		"Soft armour is light, not hindering your combat much."
	},
	{
		TV_HARD_ARMOR,
		"Hard armour provides much more protection than soft "
		"armour but also hinders combat much more."
	},
	{
		TV_SYMBIOTIC_BOOK,
		"This mystical book is used by symbiants to extend their "
		"symbiosis."
	},
	{
		TV_MUSIC_BOOK,
		"This song book is used by harpers to play songs."
	},
	{
		TV_DRUID_BOOK,
		"This mystical book is used by druids to call upon the "
		"powers of nature."
	},
	{
		TV_DAEMON_BOOK,
		"This magic book is used by daemonologists to control "
		"demons."
	},
	{0, ""},
};

/*
 * List of the between exits
 *       s16b corresp;           Corresponding between gate
 *       bool dungeon;           Do we exit in a dungeon or in the wild ?
 *
 *       s16b wild_x, wild_y;    Wilderness spot to land onto
 *       s16b px, py;            Location of the map
 *
 *       s16b d_idx;             Dungeon to land onto
 *       s16b level;
 */
between_exit between_exits[MAX_BETWEEN_EXITS] =
{
	{
		1,
		FALSE,
		49, 11,
		119, 25,
		0, 0
	},
	{
		0,
		FALSE,
		60, 56,
		10, 35,
		0, 0
	},
};

/*
 * Months
 */
int month_day[9] =
{
	0,      /* 1 day */

	1,      /* 54 days */
	55,     /* 72 days */
	127,    /* 54 days */

	181,    /* 3 days */

	184,    /* 54 days */
	238,    /* 72 days */
	310,    /* 54 days */

	364,    /* 1 day */
};
cptr month_name[9] =
{
	"Yestare",

	"Tuile",
	"Laire",
	"Yavie",

	"Enderi",

	"Quelle",
	"Hrive",
	"Coire",

	"Mettare",
};

/*
 * max body parts
 */
int max_body_part[BODY_MAX] =
{
	3,      /* Weapon */
	1,      /* Torso */
	3,      /* Arms */
	6,      /* Finger */
	2,      /* Head */
	2,      /* Legs */
};
