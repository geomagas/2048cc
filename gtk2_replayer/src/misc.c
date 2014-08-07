/****************************************************************
 * This file is part of the "2048cc GTK+2 Replayer".
 *
 * Copyright:    2014 (c) migf1 <mig_f1@hotmail.com>
 * License:      Free Software (see comments in main.c for limitations)
 * Dependencies: misc.h
 * --------------------------------------------------------------
 *
 * A small collection of misc functions to be used across the project.
 ****************************************************************
 */
#define MISC_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "misc.h"

/* ---------------------------------------------------
 * void dbg_print_info():
 *
 * Print the specified arguments in the stdout stream,
 * only if 'global_debugOn' is true.
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
 * char *s_fgets():
 *
 * Copy into the specified c-string (s) up to (n-1) leading bytes of
 * the next line available in the specified file-stream (fp).
 * Drop EOL, NUL terminate (s) and return it (or NULL on error).
 *
 * NOTES: This function is similar to the standard ISO C function
 *        fgets(), but it does NOT copy the EOL sequence into (s).
 *
 *        Moreover, this function is meant to be used in a file-stream
 *        opened in BINARY_MODE, because it attempts to handle manually
 *        three (3) different kinds of EOL sequences, namely:
 *        "\r\n", '\n' and '\r'.
 *
 *        In any of the above cases, the EOL sequence is NOT copied
 *        into (s). A NUL byte is copied, instead.
 *
 *        The function is used for reading replay-files.
 *        See functions: gamedata_set_from_fname() and _fp_skip_list(),
 *        in the source-module: gamedata.c
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
	s[i] = '\0';  /* NUL-terminate s */

	return s;
}
