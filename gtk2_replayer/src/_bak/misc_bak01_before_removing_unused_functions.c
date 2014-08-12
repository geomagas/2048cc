/****************************************************************
 * This file is part of the "2048cc GTK+2 Replayer".
 *
 * COpyright:    2014 (c) migf1 <mig_f1@hotmail.com>
 * License:      Free Software (see comments in main.c for limitations)
 * Dependencies: gtk.h, misc.h
 * --------------------------------------------------------------
 *
 * A small collection of misc functions to be used across the project.
 ****************************************************************
 */
#define MISC_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>
#include "misc.h"

/* ---------------------------------------------------
 * void dbg_print_info():
 *
 * Print the specified arguments in the stdout stream,
 * only if 'global_debugOn' is TRUE.
 * ---------------------------------------------------
 */
void dbg_print_info(
	char *fmtxt,
	...
	)
{
	va_list args;

	if ( !global_debugOn || !fmtxt ) {
		return;
	}

	va_start(args, fmtxt);
	vprintf( fmtxt, args );
	va_end( args );
}

/* --------------------------------------------------------------
 * int _int_count_digits():
 *
 * Return the digits count of the given integer (0 counts as 1 digit,
 * for negative numbers the minus sign in not counted).
 * --------------------------------------------------------------
 */
int int_count_digits( int num )
{
	int i = 0;

	if ( 0 == num ) {
		return 1;
	}
	while ( num ) {
		num /= 10;
		i++;
	}
	return i;
}

/* ---------------------------------------------------
 * char *s_new_shortfname():
 *
 * Return a newly allocated copy of the specified c-string,
 * consisting of the last DESIRED_LEN+1 bytes of the original
 * c-string, PLUS a prepended ellipsis. Return NULL on error.
 * ---------------------------------------------------
 */
#define DESIRED_LEN 15
char *s_new_shortfname( const char *s )
{
	const char   *ellipsis = "...";
	const char   *cp = NULL;
	size_t       slen = strlen( s );
	size_t       retsz = DESIRED_LEN + strlen(ellipsis) + 1;
	char         *ret = NULL;

	if ( NULL == s ) {
		DBG_STDERR_MSG( "NULL pointer argument!" );
		return NULL;
	}

	ret = calloc( retsz, sizeof(char) );
	if ( NULL == ret ) {
		DBG_STDERR_MSG( "calloc(failed)!" );
		return NULL;
	}

	if ( slen > DESIRED_LEN ) {
		cp = (char *) &s[ slen-DESIRED_LEN ];
	}
	else {
		cp = s;
	}

	snprintf(
		ret,
		retsz,
		"%s%s",
		cp != s ? ellipsis : "\0",
		cp
		);

	return ret;
}

/* ----------------------------------------------------
 * Get a line from a stream.
 *
 * Differences over fgets():
 *	- sanity checks, sets errno
 *	- rejects final '\n'
 *	- does not directly set file descriptor flags (only via fgetc())
 * --------------------------------------------------------------
 */
char *s_fgets( char *s, int n, FILE *fp )
{
	register int i = 0, c = 0;

	/* sanity checks */
	if ( NULL == s || NULL == fp ) {
		DBG_STDERR_MSG( "NULL pointer argument (s OR fp)!" );
		return NULL;
	}
	if ( n < 1 ) {
		DBG_STDERR_MSG( "Invalid argument (n)!" );
		return NULL;
	}

#if 1
	for (i=0; i < n-1 && EOF != (c = getc(fp)); i++ ) {
		if ( '\n' == c ) {
			break;
		}
		if ( '\r' == c ) {
			int ch = getc(fp);
			if ( '\n' != ch ) {
				ungetc( ch, fp );
			}
			break;
		}
		s[i] = c;
	}
	s[i] = '\0';				/* nul-terminate s */
#else
	for (i=0; i < n-1 && EOF != (c = getc(fp)) && '\n' != c; i++) {
		s[i] = c;
	}
	s[i] = '\0';				/* nul-terminate s */
#endif

	return s;
}

/* --------------------------------------------------------------
 * char *s_char_replace():
 *
 * Given a c-string s, replace all occurrences of character the
 * cin with the character cout. Return a pointer to the modified
 * c-string s (in case of error, it will be unchanged).
 *
 * NOTES: NUL bytes ('\0') are not allowed as a replacement,
 *        and also they are not allowed to get modified.
 * --------------------------------------------------------------
 */
char *s_char_replace( char *s, int cin, int cout )
{
	char *cp = NULL;

	/* sanity checks */
	if ( NULL == s ) {
		DBG_STDERR_MSG( "NULL pointer argument!" );
		return s;
	}
	if ( '\0' == cin ) {
		DBG_STDERR_MSG( "NUL byte is not allowed to get modified!" );
		return s;
	}
	if ( '\0' == cout ) {
		DBG_STDERR_MSG( "NUL byte is not allowed as a replacement!" );
		return s;
	}

	for ( cp=s; '\0' != *cp; cp++ ) {
		if ( *cp == cin ) {
			*cp = cout;
		}
	}

	return s;
}

/* --------------------------------------------------------------
 * char *s_strip():
 *
 * Remove any of the characters contained in the c-string del, from
 * the c-string s. Return a pointer to the modified c-string s (in
 * case of error, it will be unchanged).
 * --------------------------------------------------------------
 */
char *s_strip( char *s, const char *del )
{
	char *cp1 = NULL;            /* for parsing the whole s    */
	char *cp2 = NULL;            /* for keeping desired *cp1's */

	/* sanity checks */
	if ( NULL == s || NULL == del ) {
		DBG_STDERR_MSG( "NULL pointer argument!" );
		return s;
	}
	if ( '\0' == *s || '\0' == *del ) {
		DBG_STDERR_MSG( "s or del is empty!" );
		return s;
	}

	for (cp1=cp2=s; *cp1; cp1++ ) {
		if ( !strchr(del, *cp1) ) {/* *cp1 is NOT contained in del */
			*cp2++ = *cp1;     /* copy it to start of s, via cp2*/
		}
	}
	*cp2 = 0;                          /* NUL terminate the stripped s */

	return s;
}
