#define MISC_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>
#include "misc.h"

/* ---------------------------------------------------
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

/* ---------------------------------------------------
 * Display the specified message inside a simple modal dialog.
 *
 * The owner of the dialog is the window specified in
 * the first argument, which usually is the main window
 * of the application (if passed as NULL, the alert-box
 * will not have an owner, which is fine but a bit odd).
 * ---------------------------------------------------
 */
void mygtk_alert_box(
	GtkWidget   *appMainWindow,
	const gchar *message
	)
{
	GtkWidget *alertBox = NULL;

	if ( appMainWindow ) {
		gtk_widget_set_sensitive( appMainWindow, FALSE );
	}

	alertBox = gtk_message_dialog_new(
			GTK_WINDOW(appMainWindow),
			GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_INFO,
			GTK_BUTTONS_OK,
			"%s", message
		);

	gtk_window_set_title( GTK_WINDOW(alertBox), "Alert Box" );
	gtk_dialog_run( GTK_DIALOG(alertBox) );
	gtk_widget_destroy( alertBox );

	if ( appMainWindow ) {
		gtk_widget_set_sensitive( appMainWindow, TRUE );
	}
}

/* ---------------------------------------------------
 * Force a redraw of the specified widget.
 *
 * First the widget is flagged for display, and then
 * GTK+ is forced to iterate through its main-loop
 * until no more events are left for processing.
 * ---------------------------------------------------
 */
void mygtk_widget_refresh( GtkWidget *widget )
{
	/* sanity check */
	if ( !widget ) {
		return;
	}

	gtk_widget_show( widget );
	while( gtk_events_pending() ) {
		gtk_main_iteration();
	}

	return;
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

/* ---------------------------------------------------
 *
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
		DBG_STDERR_MSG( "calloc(falied)!" );
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
