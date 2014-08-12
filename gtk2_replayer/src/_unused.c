/*
 * misc.h
 */

/* Determine the compilation OS & define accordingly some
 * more constants related to replay files.
 */
#if defined(_WIN32) || defined(_WIN64) || defined(__WINDOWS__) \
|| defined(__TOS_WIN__)
	#define MISC_OS_WINDOWS

#elif ( defined(__APPLE__) && defined(__MACH__) )              \
|| ( defined(__APPLE__) && defined(__MACH) )
	#define MISC_OS_OSX

#elif defined(__linux__) || defined(__linux) || defined(linux) \
|| defined(__gnu_linux__)
	#define MISC_OS_LINUX

#elif defined(__unix__) || defined(__unix) || defined(unix)    \
|| defined(__CYGWIN__)
	#define MISC_OS_UNIX

#else
	#define MISC_OS_UNKNOWN

#endif

extern int  s_tokenize(
                  char *s,
                  char *tokens[],
                  int  ntoks,
                  const char *delims
                 );
extern char *s_dup( const char *src );
extern char *s_fnamepart( const char *s );
extern char *s_char_replace( char *s, int cin, int cout );
extern char *s_strip( char *s, const char *del );
extern char *s_fixeol( char *s  );

/*
 * misc.c
 */

/* --------------------------------------------------------------
 * int s_tokenize():
 *
 * Tokenize the c-string (s) to up to (ntoks) tokens, using any char
 * contained in the c-string (delims) as delimiters. Store the tokens
 * in the c-stings array (tokens) and return the number of tokens (s)
 * was broken to, or 0 on error.
 * --------------------------------------------------------------
 */
int s_tokenize( char *s, char *tokens[], int ntoks, const char *delims )
{
	int i=0;

	/* sanity checks */
	if ( NULL == s || NULL == tokens || NULL == delims ) {
		DBG_STDERR_MSG( "NULL pointer argument!" );
		return 0;
	}
	if ( '\0' == *s || '\0' == *delims || ntoks < 1 ) {
		DBG_STDERR_MSG( "Empty s, or empty delims or invalid ntoks" );
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
 * int s_dup():
 *
 * --------------------------------------------------------------
 */
char *s_dup( const char *src )
{
	char *s = NULL;
	size_t sz = 0;

	/* sanity check */
	if ( !src ) {
		DBG_STDERR_MSG( "NULL pointer argument (s)" );
		return NULL;
	}

	sz = strlen( src ) + 1;
	if ( !(s = malloc(sz)) ) {
		DBG_STDERR_MSG( "malloc(sz) failed!" );
		return NULL;
	}

	return memcpy( s, src, sz );
}

/* ---------------------------------------------------
 *
 * ---------------------------------------------------
 */
char *s_fnamepart( const char *s )
{
	char *cp = NULL;

	if ( !s || !*s )
		return (char *)s;

	cp = (char *) &s[ strlen(s)-1 ];
	while ( cp != s && *cp != G_DIR_SEPARATOR && *cp != ':' )
		cp--;

	return (*cp == G_DIR_SEPARATOR || *cp == ':') ? ++cp : cp;
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

/* --------------------------------------------------------------
 * char *s_fixeol():
 *
 * Depending on the compilation platform, fix all eol characters
 * in the specified c-string (s). Return a pointer to the modified
 * c-string s (in case of error, it will be unchanged).
 *
 * NOTES (IMPORTANT!):
 *        On Windows, the eol convention is "\r\n". On Unix & Linux
 *        it is '\n', and on MacOSX it is '\r'.
 *
 *        The c-string (s) passed in the function, is expected to
 *        follow the Windows convention, but on Windows platforms
 *        the function will work even if (s) is using any of the
 *        other two conventions.
 * --------------------------------------------------------------
 */
char *s_fixeol( char *s  )
{
	if ( NULL == s ) {
		DBG_STDERR_MSG( "NULL pointer argument!" );
		return s;
	}

#if defined( MISC_OS_WINDOWS )
	if ( NULL == strstr(s, "\r\n") ) {
		return s_char_replace( s, '\r', '\n' );
	}

	return s;

#elif defined( MISC_OS_UNIX ) || defined( CC2048_OS_LINUX )
	return s_strip( s, "\r" );

#elif defined( MISC_OS_OSX )
	return s_strip( s, "\n" );

#else
	return s;
#endif
}

