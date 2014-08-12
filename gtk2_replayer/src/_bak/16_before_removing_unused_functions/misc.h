#ifndef MISC_H

#include <gtk/gtk.h>
#include <stdio.h>

#define SZMAX_DBGMSG             (1023+1)

extern gboolean global_debugOn;

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

/**
 * Macro displaying an error-message with debugging info within a GUI alert-box
 * (this is a convenient wrapper of the custom function myGtk_alert_box()).
 *
 * @param appWindow The owner-window of the alert-box (may be NULL).
 * @param errmsg The message to be displayed.
 */
#define DBG_GUI_ERRMSG( appWindow, errmsg )                     \
do {                                                            \
	gchar dbgMsgOut[SZMAX_DBGMSG] = {'\0'};                \
	if ( !global_debugOn )                                  \
		break;                                          \
	g_snprintf(                                             \
		dbgMsgOut,                                      \
		SZMAX_DBGMSG,                                  \
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

extern void mygtk_alert_box( GtkWidget *appMainWindow, const gchar *msg );
extern void mygtk_widget_redraw( GtkWidget *widget );

extern int int_count_digits( int num );

extern char *s_fgets( char *s, int n, FILE *fp );
extern char *s_fnamepart( const char *s );
extern char *s_new_shortfname( const char *s );
extern char *s_fixeol( char *s  );
extern int  s_tokenize(
                  char *s,
                  char *tokens[],
                  int  ntoks,
                  const char *delims
                 );
extern char *s_dup( const char *src );

#endif

#endif
