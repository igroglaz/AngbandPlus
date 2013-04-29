/* File: z-util.c */

/*
 * Copyright (c) 1997 Ben Harrison
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.
 */

/* Purpose: Low level utilities -BEN- */

#include "z-util.h"



/*
 * Global variables for temporary use
 */
char char_tmp = 0;
byte byte_tmp = 0;
sint sint_tmp = 0;
uint uint_tmp = 0;
long long_tmp = 0;
huge huge_tmp = 0;
errr errr_tmp = 0;


/*
 * Global pointers for temporary use
 */
cptr cptr_tmp = NULL;
vptr vptr_tmp = NULL;




/*
 * Constant bool meaning true
 */
bool bool_true = 1;

/*
 * Constant bool meaning false
 */
bool bool_false = 0;


/*
 * Global NULL cptr
 */
cptr cptr_null = NULL;


/*
 * Global NULL vptr
 */
vptr vptr_null = NULL;



/*
 * Global SELF vptr
 */
vptr vptr_self = (vptr)(&vptr_self);



/*
 * Convenient storage of the program name
 */
cptr argv0 = NULL;



/*
 * A routine that does nothing
 */
void func_nothing(void)
{
  /* Do nothing */
}


/*
 * A routine that always returns "success"
 */
errr func_success(void)
{
  return (0);
}


/*
 * A routine that always returns a simple "problem code"
 */
errr func_problem(void)
{
  return (1);
}


/*
 * A routine that always returns a simple "failure code"
 */
errr func_failure(void)
{
  return (-1);
}



/*
 * A routine that always returns "true"
 */
bool func_true(void)
{
  return (1);
}


/*
 * A routine that always returns "false"
 */
bool func_false(void)
{
  return (0);
}




/*
 * Determine if string "t" is equal to string "t"
 */
bool streq(cptr a, cptr b)
{
  return (!strcmp(a, b));
}


/*
 * Determine if string "t" is a suffix of string "s"
 */
bool suffix(cptr s, cptr t)
{
  int tlen = strlen(t);
  int slen = strlen(s);
  
  /* Check for incompatible lengths */
  if (tlen > slen) return (FALSE);
  
  /* Compare "t" to the end of "s" */
  return (!strcmp(s + slen - tlen, t));
}


/*
 * Determine if string "t" is a prefix of string "s"
 */
bool prefix(cptr s, cptr t)
{
  /* Scan "t" */
  while (*t)
    {
      /* Compare content and length */
      if (*t++ != *s++) return (FALSE);
    }
  
  /* Matched, we have a prefix */
  return (TRUE);
}



/*
 * Redefinable "plog" action
 */
void (*plog_aux)(cptr) = NULL;

/*
 * Print (or log) a "warning" message (ala "perror()")
 * Note the use of the (optional) "plog_aux" hook.
 */
void plog(cptr str)
{
  /* Use the "alternative" function if possible */
  if (plog_aux) (*plog_aux)(str);
  
  /* Just do a labeled fprintf to stderr */
  else (void)(fprintf(stderr, "%s: %s\n", argv0 ? argv0 : "???", str));
}



/*
 * Redefinable "quit" action
 */
void (*quit_aux)(cptr) = NULL;

/*
 * Exit (ala "exit()").  If 'str' is NULL, do "exit(0)".
 * If 'str' begins with "+" or "-", do "exit(atoi(str))".
 * Otherwise, plog() 'str' and exit with an error code of -1.
 * But always use 'quit_aux', if set, before anything else.
 */
void quit(cptr str)
{
  /* Attempt to use the aux function */
  if (quit_aux) (*quit_aux)(str);
  
  /* Success */
  if (!str) (void)(exit(0));
  
  /* Extract a "special error code" */
  if ((str[0] == '-') || (str[0] == '+')) (void)(exit(atoi(str)));
  
  /* Send the string to plog() */
  plog(str);
  
  /* Failure */
  (void)(exit(EXIT_FAILURE));
}



/*
 * Redefinable "core" action
 */
void (*core_aux)(cptr) = NULL;

/*
 * Dump a core file, after printing a warning message
 * As with "quit()", try to use the "core_aux()" hook first.
 */
void core(cptr str)
{
  char *crash = NULL;
  
  /* Use the aux function */
  if (core_aux) (*core_aux)(str);
  
  /* Dump the warning string */
  if (str) plog(str);
  
  /* Attempt to Crash */
  (*crash) = (*crash);
  
  /* Be sure we exited */
  quit("core() failed");
}


/*
 * Case insensitive comparison between two strings
 */
int my_stricmp(const char *s1, const char *s2)
{
	char ch1 = 0;
	char ch2 = 0;

	/* Just loop */
	while (TRUE)
	{
		/* We've reached the end of both strings simultaneously */
		if ((*s1 == 0) && (*s2 == 0))
		{
			/* We're still here, so s1 and s2 are equal */
			return (0);
		}

		ch1 = toupper(*s1);
		ch2 = toupper(*s2);

		/* If the characters don't match */
		if (ch1 != ch2)
		{
			/* return the difference between them */
			return ((int)(ch1 - ch2));
		}

		/* Step on through both strings */
		s1++;
		s2++;
	}

	return (0);
}

/*
 * Case insensitive comparison between the first n characters of two strings
 */
int my_strnicmp(cptr a, cptr b, int n)
{
	cptr s1, s2;
	char z1, z2;

	/* Scan the strings */
	for (s1 = a, s2 = b; n > 0; s1++, s2++, n--)
	{
		z1 = toupper((unsigned char)*s1);
		z2 = toupper((unsigned char)*s2);
		if (z1 < z2) return (-1);
		if (z1 > z2) return (1);
		if (!z1) return (0);
	}

	return 0;
}

/*
 * The my_strcpy() function copies up to 'bufsize'-1 characters from 'src'
 * to 'buf' and NUL-terminates the result.  The 'buf' and 'src' strings may
 * not overlap.
 *
 * my_strcpy() returns strlen(src).  This makes checking for truncation
 * easy.  Example: if (my_strcpy(buf, src, sizeof(buf)) >= sizeof(buf)) ...;
 *
 * This function should be equivalent to the strlcpy() function in BSD.
 */
size_t my_strcpy(char *buf, const char *src, size_t bufsize)
{
  size_t len = strlen(src);
  size_t ret = len;
  
  /* Paranoia */
  if (bufsize == 0) return ret;
  
  /* Truncate */
  if (len >= bufsize) len = bufsize - 1;
  
  /* Copy the string and terminate it */
  (void)memcpy(buf, src, len);
  buf[len] = '\0';
  
  /* Return strlen(src) */
  return ret;
}



