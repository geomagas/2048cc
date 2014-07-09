/****************************************************************
 * This file is part of the "2048 Console Clone" game.
 *
 * Author:       migf1 <mig_f1@hotmail.com>
 * Version:      0.3a2
 * Date:         July 9, 2014
 * License:      Free Software (see comments in main.c for limitations)
 * Dependencies: common.h
 * --------------------------------------------------------------
 *
 ****************************************************************
 */

#define COMMON_C

#include <stdio.h>        /* vsnprintf() */
#include <stdlib.h>       /* calloc(), realloc(), free() */
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#include "common.h"

/* --------------------------------------------------------------
 * char *vprintf_to_text():
 *
 * This function is a wrapper to ISO C99's vsnprintf(). It creates
 * dynamically the buffer needed to hold the printed output, and if
 * it succeeds it returns a pointer to it. On error, it returns NULL.
 *
 * NOTES:
 *   1. vargs MUST have been already initialized in the caller
 *      (by the va_start macro, and possibly subsequent va_arg
 *      calls).
 *   2. The returned buffer, if non-NULL, MUST be freed in the
 *      calling environment.
 *   3. The function is currently used only in: _printfxy()
 * --------------------------------------------------------------
 */
char *vprintf_to_text( const char *fmt, va_list vargs )
{
	int   nchars = 0;
	char  *buf = NULL;
	size_t bufsz = BUFSIZ;

	/* make buf to hold the text and pass it to vsnprintf */
	if ( NULL == (buf = calloc(1, bufsz)) ) {
		DBGF( "%s", "calloc failed!" );
		goto ret_failure;
	}
	nchars = vsnprintf( buf, bufsz, fmt, vargs );
	if ( nchars < 0 ) {
		DBGF( "%s", "vsnprintf failed!" );
		goto ret_failure;
	}

	/* Was buf too small to hold the text?
	 * Reallocate 1+nchars bytes and re-apply.
	 */
	if ( nchars >= (int)bufsz ) {
		char *try = NULL;
		bufsz = 1 + nchars;   /* for the NUL byte */
		try = realloc( buf, bufsz );
		if ( NULL == try ) {
			DBGF( "%s", "realloc failed!" );
			goto ret_failure;
		}
		buf = try;

		nchars = vsnprintf( buf, bufsz, fmt, vargs );
		if ( nchars < 0 ) {
			DBGF( "%s", "vsnprintf failed!" );
			goto ret_failure;
		}
	}

	return buf;

ret_failure:
	if ( buf ) {
		free( buf );
	}
	return NULL;
}

/* --------------------------------------------------------------
 * char *printf_to_text():
 *
 * This function is similar to ISO C99's snprintf() but it creates
 * dynamically the string needed to hold the print-out. It returns
 * the string, or NULL on error.
 * --------------------------------------------------------------
 */
char *printf_to_text( const char *fmt, ... )
{
	char  *txtout = NULL;
	va_list vargs;

	va_start( vargs, fmt );
	txtout = vprintf_to_text( fmt, vargs );
	va_end( vargs );

	return txtout;
}

/* --------------------------------------------------------------
 * Return 1 (true) if the specified file-name (full path) is
 * available, return 0 (false) otherwise.
 * --------------------------------------------------------------
 */
int f_exists( const char *fname )
{
	FILE *fp = fopen(fname, "rb");
	if ( NULL == fp ) {
		return 0;  /* false */
	}

	fclose( fp );
	return 1;  /* true */
}

/* --------------------------------------------------------------
 * Read a c-string from stdin, flushing any extra characters.
 *
 * Differences over gets():
 *	- performs sanity checks
 * 	- limits the number of input chars
 *	- flushes any extra chars from stdin
 * --------------------------------------------------------------
 */
char *s_getflushed( char *s, size_t ssize )
{
	int c;
	size_t i;

	/* sanity checks */
	if ( !s ) {
		DBGF( "%s", "NULL pointer argument!" );
		return NULL;
	}
	if ( ssize < 1 ) {
		DBGF( "Invalid size (%zd)!", ssize );
		return NULL;
	}

	/* read chars from stdin */
	for (i=0; i < ssize-1 && '\n' != (c=getchar()) && EOF != c; i++) {
		s[i] = c;
	}

	if ( s[i] && s[i] != '\n' ) {         /* ssize reached without '\n' ?*/
		while ( getchar() != '\n' ) { /* flush any remaining chars */
			/* void */ ;
		}
	}
	s[i] = '\0';                          /* NUL-terminate s */

	return s;
}

/* --------------------------------------------------------------
 * int s_tokenize():
 *
 * Tokenize a c-string, up to ntoks tokens.
 * Return the number of tokens which s was broken to, or 0 on error.
 * --------------------------------------------------------------
 */
int s_tokenize( char *s, char *tokens[], int ntoks, const char *delims )
{
	int i=0;

	/* sanity checks */
	if ( NULL == s || NULL == tokens || NULL == delims ) {
		DBGF( "%s", "NULL pointer argument!" );
		return 0;
	}
	if ( '\0' == *s || '\0' == *delims || ntoks < 1 ) {
		DBGF( "Empty s, or empty delims or invalid ntoks (%d)", ntoks );
		return 0;
	}

	tokens[ 0 ] = strtok(s, delims);
	if ( tokens[0] == NULL ) {
		return 0;
	}
	for (i=1; i < ntoks && (tokens[i]=strtok(NULL, delims)) != NULL; i++) {
		/* void */ ;
	}

	return i;
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
		DBGF( "%s", "NULL pointer argument!" );
		return s;
	}
	if ( '\0' == cin ) {
		DBGF( "%s", "NUL byte is not allowed to get modified!" );
		return s;
	}
	if ( '\0' == cout ) {
		DBGF( "%s", "NUL byte is not allowed as a replacement!" );
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
 * char *s_trim():
 *
 * Trim leading & trailing blanks from a c-string. Return a pointer
 * to the modified c-string s (in case of error, it will be unchanged).
 *
 * NOTES: isspace() is used for determining blank characters.
 * --------------------------------------------------------------
 */
char *s_trim( char *s )
{
	char *cp1 = NULL;      /* for parsing the whole s */
	char *cp2 = NULL;      /* for shifting & padding  */

	/* sanity checks */
	if ( NULL == s )  {
		DBGF( "%s", "NULL pointer argument!" );
		return s;
	}

	/* skip leading blanks, via cp1 */
	for (cp1=s; isspace( (int)(*cp1)); cp1++) {
		; /* void */
	}

	/* shift to the left remaining chars, via cp2  */
	for (cp2=s; *cp1; cp1++, cp2++) {
		*cp2 = *cp1;
	}
	*cp2-- = '\0';  /* mark end of left trimmed s */

	/* skip trailing blanks via cp2 */
	while ( cp2 > s && isspace((int)(*cp2)) ) {
		cp2--;
	}
	*++cp2 = '\0';  /* mark end of trimmed s */

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
		DBGF( "%s", "NULL pointer argument!" );
		return s;
	}
	if ( '\0' == *s || '\0' == *del ) {
		DBGF( "%s", "s or del is empty!" );
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

/* --------------------------------------------------------------
 * char *s_fixeol():
 *
 * Depending on the compilation platform, fix all eol characters
 * in the specified c-string (s). Return a pointer to the modified
 * c-string s (in case of error, it will be unchanged).
 *
 * NOTES (IMPORTANT!):
 *        On Windows eol is "\r\n", on Unix & Linux it is "\n",
 *        and on MacOSX it is "\r". The c-string (s) passed in
 *        the function, is expected to follow the Windows format,
 *        that is using "\r\n" for eol.
 * --------------------------------------------------------------
 */
char *s_fixeol( char *s  )
{
#if defined( CC2048_OS_WINDOWS )
	if ( NULL == s ) {
		DBGF( "%s", "NULL pointer argument!" );
		return s;
	}

	if ( NULL == strstr(s, "\r\n") ) {
		return s_char_replace( s, '\r', '\n' );
	}

	return s;

#elif defined( CC2048_OS_UNIX ) || defined( CC2048_OS_LINUX )
	return s_strip( s, "\r" );

#elif defined( CC2048_OS_OSX )
	return s_strip( s, "\n" );

#else
	return s;
#endif
}
