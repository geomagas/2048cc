#define MISC_C

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
	GtkWidget  *appMainWindow,
	gchar      *message
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

