#define GUI_DIALOGS_C

#include <gtk/gtk.h>
//#include <stdio.h>    /* just for debugging messages in the console */
#include <stdlib.h>
#include <errno.h>

#ifdef G_OS_WIN32
#include <windows.h>
#endif

#include "misc.h"
#include "text.h"
#include "gui_dialogs.h"
#include "gui.h"

struct _GuiDialogs
{
	struct {
		GtkWidget *root;
	} about;

	struct {
		GtkWidget *root;
		GtkWidget *lblRange;
		GtkWidget *lblCurrent;
		GtkWidget *te;
	} jumpto;
};

#ifdef G_OS_WIN32
/* ---------------------------------------------------
 * Callback function connected to the GTK+ "activate-link"
 * signal, for the dialog About.
 *
 * When run on Win32, GTK+ (Gio) cannot open links,
 * thus we overwrite the default behaviour by using
 * our own Win32 specific callback function.
 * ---------------------------------------------------
 */
static gboolean _on_activate_link_dlgAbout(
	GtkWidget  *link,
	gchar      *uri,
	gpointer   *unused
	)
{
	/* avoid compiler warnings for unused parameters */
	(void)unused;
	if ( NULL == link || NULL == uri ) {
		return FALSE;
	}

	/* Win32 API call */
	ShellExecute( NULL, "open", uri, NULL, NULL, SW_SHOWNORMAL );

	dbg_print_info( "Signal \"activate-link\" sent for uri: %s\n", uri );


	/* Returning TRUE means we handled the event, so the signal
	 * emission should be stopped (don't call any further callbacks
	 * that may be connected). Return FALSE to continue invoking
	 * callbacks.
	 */
	return TRUE;
}
#endif	/* #ifdef G_OS_WIN32 */

/* ---------------------------------------------------
 * Initialize the "About" dialog.
 *
 * Copies the GUI dialog "About" from the specified GTK+
 * builder into my GUI abstraction: gui->dialogs->about.root,
 * it initializes the dialog's contents and it connects
 * callback functions.
 *
 * The "About" dialog is defined of type GtkAboutDialog
 * in the Glade file, which provides a standardized way
 * of presenting information. However, I haven't found
 * an easy way to access the members of its action-area,
 * namely the buttons: "Credits", "License" and "Close".
 *
 * Consequently, I cannot mark the labels of those buttons
 * as translatable, for the GNU-gettext library (I could
 * implement it as a normal GtkDialog, but I prefer to
 * learn the GtkAboutDialog internals and use them in
 * a future version).
 *
 * NOTE:
 *	The callback function: on_activate_link_dlgAbout()
 *	is connected to the "activate-link" signal ONLY
 *	when the program is compiled under Win32. GTK+'s
 *	file-engine (Gio) has trouble registering the
 *	"http://" and "mailto:" protocols under Win32,
 *	so I conditionally use the Win32 API if needed.
 *
 *	The GTK+ callback function: gtk_widget_destroyed()
 *	ensures that the dialog's pointer will be set to
 *	NULL after the widget gets destroyed.
 * ---------------------------------------------------
 */
static inline gboolean _init_about_dialog(
	GuiDialogs  *dialogs,
	GtkBuilder  *builder
	)
{
	GtkWidget   **root     = &dialogs->about.root;
	GdkPixbuf   *logo      = NULL;
	GError      *error     = NULL;
	const gchar *authors[] = {TXT_APP_AUTHOR, NULL};
	const gchar *artists[] = {TXT_APP_ARTIST, NULL};

	/* load the about-dialog into gui, from the GTK+2 builder */
	*root = GTK_WIDGET( gtk_builder_get_object(builder, "dlgAbout") );

	/* set dialog's icon & opacity */
	gtk_window_set_icon_from_file( GTK_WINDOW(*root), FNAME_APPICON, NULL);
	gtk_window_set_opacity( GTK_WINDOW(*root), 0.90 );

	/* set dialog's logo */
	logo = gdk_pixbuf_new_from_file( FNAME_APPLOGO, &error );
	gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(*root), logo );

	/* set the remaining properties */
	gtk_about_dialog_set_program_name(
		GTK_ABOUT_DIALOG( *root ),
		gettext( TXT_APP_NAME )
		);

	gtk_about_dialog_set_version(
		GTK_ABOUT_DIALOG( *root ),
		gettext( TXT_APP_VERSION )
		);

	gtk_about_dialog_set_comments(
		GTK_ABOUT_DIALOG( *root ),
		TXT_APP_DESCRIPTION
		);

	gtk_about_dialog_set_copyright(
		GTK_ABOUT_DIALOG( *root ),
		gettext( TXT_APP_COPYRIGHT )
		);

	gtk_about_dialog_set_wrap_license(
		GTK_ABOUT_DIALOG( *root ),
		TRUE
		);
	gtk_about_dialog_set_license(
		GTK_ABOUT_DIALOG( *root ),
		_(TXT_APP_LICENSE)
		);

	gtk_about_dialog_set_authors(
		GTK_ABOUT_DIALOG( *root ),
		authors
		);

	gtk_about_dialog_set_documenters(
		GTK_ABOUT_DIALOG( *root ),
		authors
		);

	gtk_about_dialog_set_artists(
		GTK_ABOUT_DIALOG( *root ),
		artists
		);

	gtk_about_dialog_set_website_label(
		GTK_ABOUT_DIALOG( *root ),
		gettext( TXT_APP_WEBSITE_LABEL )
		);
	gtk_about_dialog_set_website(
		GTK_ABOUT_DIALOG( *root ),
		TXT_APP_WEBSITE
		);

	/*
	 * connect callback functions to signals emitted by the dialog
	 */

#ifdef G_OS_WIN32
	/* when run on Win32, GTK+ (Gio) cannot open links,
	 * thus we overwrite the default behaviour by using
	 * our own Win32 specific callback function
	 */
	g_signal_connect(
		G_OBJECT( *root ),
		"activate-link",
		G_CALLBACK( _on_activate_link_dlgAbout ),
		NULL
		);
#endif

	/* ensure that *root will be set to NULL after it is destroyed */
	g_signal_connect(
		G_OBJECT( *root ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),   /* provided by GTK+ */
		root
		);

	return TRUE;
}

/* ---------------------------------------------------
 * Initialize the "Jumpto" dialog.
 *
 * Copies the GUI dialog "About" from the specified GTK+
 * builder into my GUI abstraction: gui->dlgJumpto, it
 * initializes the dialog's contents and it connects
 * callback functions.
 *
 * The GTK+ callback function: gtk_widget_destroyed()
 * ensures that the pointer of the connected widget
 * will be set to NULL after the widget gets destroyed.
 * ---------------------------------------------------
 */
static inline gboolean _init_jumpto_dialog(
	GuiDialogs  *dialogs,
	GtkBuilder  *builder
	)
{
	/* just for brevity later on */
	GtkWidget **pw = NULL;        /* pointer to a widget pointer */

	/* load the root window of the dialog */
	pw  = &dialogs->jumpto.root;
	*pw = GTK_WIDGET( gtk_builder_get_object(builder, "dlgJumpto") );

	/* set dialog's initial position, icon and opacity */
	gtk_window_set_position( GTK_WINDOW(*pw), GTK_WIN_POS_CENTER );
	gtk_window_set_icon_from_file( GTK_WINDOW(*pw), FNAME_APPICON, NULL );
	gtk_window_set_opacity( GTK_WINDOW(*pw), 0.90 );

	/* */
	g_signal_connect(
		G_OBJECT( *pw ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		pw
		);

	/* load dialog's inner widgets */

	dialogs->jumpto.lblRange
	= GTK_WIDGET( gtk_builder_get_object(builder, "lblJumptoRange") );

	dialogs->jumpto.lblCurrent
	= GTK_WIDGET( gtk_builder_get_object(builder, "lblJumptoCurrent") );

	dialogs->jumpto.te
	= GTK_WIDGET( gtk_builder_get_object(builder, "teJumpto") );

	return TRUE;
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
GuiDialogs *gui_dialogs_free( GuiDialogs *dialogs )
{
	if ( dialogs ) {
		free( dialogs );
	}
	return NULL;
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
GuiDialogs *make_gui_dialogs( void )
{
	GuiDialogs *dialogs = calloc( 1, sizeof(*dialogs) );
	if ( NULL == dialogs ) {
		DBG_GUI_ERRMSG( NULL, "calloc() failed!");
		return NULL;
	}

	return dialogs;
}

/* ---------------------------------------------------
 * Initialize the dialogs of the program.
 *
 * Copies the dialog widgets from the specified GTK+ builder
 * into my GUI abstraction: gui->dialogs (GuiDialogs), it connects
 * callback functions to them and it initializes their visual
 * appearance.
 *
 * NOTE:
 *	The GTK+ callback function: gtk_widget_destroyed()
 *	ensures that the widget pointer will be set to NULL
 *	after the widget gets destroyed.
 * ---------------------------------------------------
 */
gboolean gui_dialogs_init_from_builder(
	GuiDialogs  *dialogs,
	GtkBuilder  *builder,
	Gui         *gui
	)
{
	if ( NULL == gui ) {
		DBG_STDERR_MSG( "NULL pointer argument (gui)" );
		return FALSE;
	}
	if ( NULL == dialogs ) {
		DBG_GUI_ERRMSG(
			gui_get_appWindow(gui),
			"NULL pointer argument (dialogs)"
			);
		return FALSE;
	}
	if ( NULL == builder ) {
		DBG_GUI_ERRMSG(
			gui_get_appWindow(gui),
			"NULL pointer argument (builder)"
			);
		return FALSE;
	}

	_init_about_dialog( dialogs, builder );
	_init_jumpto_dialog( dialogs, builder );

	return TRUE;
}

/* ---------------------------------------------------
 *
 * ---------------------------------------------------
 */
GtkWidget *gui_dialogs_get_about_root( GuiDialogs *dialogs )
{
	if ( NULL == dialogs ) {
		DBG_GUI_ERRMSG(
			NULL,
			"NULL pointer argument (dialogs)"
			);
		return NULL;
	}

	return dialogs->about.root;
}

/* ---------------------------------------------------
 *
 * ---------------------------------------------------
 */
GtkWidget *gui_dialogs_get_jumpto_root( GuiDialogs *dialogs )
{
	if ( NULL == dialogs ) {
		DBG_GUI_ERRMSG(
			NULL,
			"NULL pointer argument (dialogs)"
			);
		return NULL;
	}

	return dialogs->jumpto.root;
}

/* ---------------------------------------------------
 *
 * ---------------------------------------------------
 */
GtkWidget *gui_dialogs_get_jumpto_lblRange( GuiDialogs *dialogs )
{
	if ( NULL == dialogs ) {
		DBG_GUI_ERRMSG(
			NULL,
			"NULL pointer argument (dialogs)"
			);
		return NULL;
	}

	return dialogs->jumpto.lblRange;
}

/* ---------------------------------------------------
 *
 * ---------------------------------------------------
 */
GtkWidget *gui_dialogs_get_jumpto_lblCurrent( GuiDialogs *dialogs )
{
	if ( NULL == dialogs ) {
		DBG_GUI_ERRMSG(
			NULL,
			"NULL pointer argument (dialogs)"
			);
		return NULL;
	}

	return dialogs->jumpto.lblCurrent;
}

/* ---------------------------------------------------
 *
 * ---------------------------------------------------
 */
GtkWidget *gui_dialogs_get_jumpto_te( GuiDialogs *dialogs )
{
	if ( NULL == dialogs ) {
		DBG_GUI_ERRMSG(
			NULL,
			"NULL pointer argument (dialogs)"
			);
		return NULL;
	}

	return dialogs->jumpto.te;
}

/* ---------------------------------------------------
 *
 * ---------------------------------------------------
 */
gboolean gui_dialogs_do_jumpto( GuiDialogs *dialogs, Gui *gui )
{
	extern int errno;

	gboolean ret    = TRUE;          /* function's return value */
	GtkWidget *appWindow = NULL;     /* application's main window */
	GtkWidget *root = NULL;          /* dialog's root widget */
	GtkEntry  *te   = NULL;          /* dialog's inner text-entry widget */
	gint response   = GUI_RESPONSE_OK;
//	char *txtOut    = NULL;          /* duplicate of te's text */
	long int jumpto = 0;             /* te's text converted to long int */
	char *tailptr   = NULL;          /* for strtol() */
	gchar txt[SZMAX_DBGMSG] = {'\0'};/* set/get text of inner widgets */
	long int rangeFirst = -1;
	long int rangeLast  = -1;

	/* sanity checks */
	if ( NULL == gui ) {
		DBG_STDERR_MSG( "NULL pointer argument (gui)" );
		return FALSE;
	}
	appWindow = gui_get_appWindow( gui );
	if ( NULL == dialogs ) {
		DBG_GUI_ERRMSG( appWindow, "NULL pointer argument (dialogs)" );
		return FALSE;
	}

	/* disable application's main window */
	gtk_widget_set_sensitive( appWindow, FALSE );

	/* set text of label-widget: range */
	rangeFirst = 1L;
	rangeLast  = gui_get_gamedata_nmoves( gui );
	g_snprintf(
		txt,
		SZMAX_DBGMSG,
		TXTF_DLG_JUMPTO_RANGE,
		rangeFirst,
		rangeLast
		);
	gtk_label_set_text(
		GTK_LABEL( gui_get_dialog_jumpto_lblRange(gui) ),
		txt
		);

	/* set text of label-widget: current */
	g_snprintf(
		txt,
		SZMAX_DBGMSG,
		TXTF_DLG_JUMPTO_CURRENT,
		1 + gui_get_imove( gui )
		);
	gtk_label_set_text(
		GTK_LABEL( gui_get_dialog_jumpto_lblCurrent(gui) ),
		txt
		);

	/* set text of text-entry-widget: te (suggest current imove) */
	te = GTK_ENTRY( dialogs->jumpto.te );
	gtk_entry_set_text( te, txt );
	gtk_editable_select_region( GTK_EDITABLE(te), 0, -1 );

	/* run the JumpTo dialog */
	root = gui_get_dialog_jumpto_root( gui );
	response = gtk_dialog_run( GTK_DIALOG(root) );

	if ( GUI_RESPONSE_OK == response ) {

		/* get inputted text of te into txt */
		memset( txt, '\0', SZMAX_DBGMSG );
		strncpy( txt, gtk_entry_get_text(te), SZMAX_DBGMSG-1 );

		/* validate inputted text of te */
		errno  = 0;
		jumpto = strtol( txt, &tailptr, 10 );
		if ( ERANGE != errno
		&& '\0' == *tailptr
		&& jumpto >= rangeFirst
		&& jumpto <= rangeLast
		){
			gui_set_imove( gui, jumpto - 1 );
			gui_refresh( gui );
		}
		else {
			mygtk_alert_box(appWindow, _(TXT_ERR_INVALID_JUMPTO));
			ret = FALSE;
		}
	}

	/* hide the dialog window */
	gtk_widget_hide( GTK_WIDGET(root) );

	gtk_widget_set_sensitive( appWindow, TRUE );

	return ret;
}
