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
#define DESIRED_LEN 20
char *s_new_shortfname( const char *s )
{
	const char   *ellipsis = "...";
	const char   *cp = NULL;
	size_t       slen = strlen( s );
	size_t       retsz = DESIRED_LEN + strlen(ellipsis) + 1;
	char         *ret = calloc( retsz, sizeof(char) );

	if ( NULL == ret ) {
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
