#ifndef MISC_H

#include <gtk/gtk.h>
#include <stdio.h>

#define MAXSIZ_DBGMSG             (1023+1)

extern gboolean global_debugOn;

/**
 * Macro displaying an error-message with debugging info within a GUI alert-box
 * (this is a convenient wrapper of the custom function myGtk_alert_box()).
 *
 * @param appWindow The owner-window of the alert-box (may be NULL).
 * @param errmsg The message to be displayed.
 */
#define DBG_GUI_ERRMSG( appWindow, errmsg )                     \
do {                                                            \
	gchar dbgMsgOut[MAXSIZ_DBGMSG] = {'\0'};                \
	if ( !global_debugOn )                                  \
		break;                                          \
	g_snprintf(                                             \
		dbgMsgOut,                                      \
		MAXSIZ_DBGMSG,                                  \
		"File\t: %s\nFunc\t: %s\nLine\t: %d\n\n%s",     \
		__FILE__, __func__,  __LINE__,                  \
		(const gchar *)(errmsg) ? errmsg : "\0"         \
		);                                              \
	mygtk_alert_box( GTK_WIDGET(appWindow), dbgMsgOut );    \
} while(0)

/**
 * Macro displaying an error-message with debugging info in the stderr stream.
 */
#define DBG_STDERR_MSG( errmsg )                                \
do {                                                            \
	if ( !global_debugOn )                                  \
		break;                                          \
	fputs( "*** ERROR:\n", stderr );                        \
	fprintf(stderr,                                         \
		"*** File: %s | Func: %s | Line: %d\n*** %s\n", \
		__FILE__, __func__,  __LINE__,                  \
		(const char *)(errmsg) ? errmsg : "\0"          \
		);                                              \
} while(0)


#ifndef MISC_C
extern void dbg_print_info( char *fmtxt, ...);

extern void mygtk_alert_box( GtkWidget *appMainWindow, gchar *msg);
extern void mygtk_widget_refresh( GtkWidget *widget );
#endif

#endif
