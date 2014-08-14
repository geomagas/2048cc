/****************************************************************
 * This file is part of the "2048cc GTK+2 Replayer".
 *
 * Copyright:    2014 (c) migf1 <mig_f1@hotmail.com>
 * License:      Free Software (see comments in main.c for limitations)
 * Dependencies: misc.h, text.h gui.h gui_dialogs.h
 * --------------------------------------------------------------
 * 
 * For generic guidelines regarding the sources of this project
 * (including the coding-style) please visit the following link:
 * https://github.com/geomagas/2048cc/blob/dev_gtk2_replayer/gtk2_replayer/src/BROWSING.md
 *
 * --------------------------------------------------------------
 * Private implementation of the GuiDialogs "class". The accompanying
 * header file "gui_dialogs.h" exposes publicly the "class" as an opaque
 * data-type.
 *
 * The GuiDialogs "class" does NOT encapsulate a generic, re-usable
 * abstraction over GTK+2 dialogs. Its encapsulated abstraction is
 * very specific to this particular project.
 *
 * The constructor, the initializer and the destructor of the "class"
 * act collectively on all the dialogs. However, the "class" also
 * provides separate, specialized functions for each dialog (usually
 * getters, but not only).
 *
 * A GuiDialogs object does NOT have direct access to data loaded
 * from replay-files. It asks for them from an associated Gui object
 * (which in turn, gets the requested data from a Gamedata object
 * associated with it).
 *
 * Actually, the association between a GuiDialogs and a Gui object
 * is NOT built into the definition of the GuiDialogs "class". Put
 * otherwise, the struct _GuiDialogs does NOT contain a reference
 * to a Gui object.
 *
 * Rather, when a public function of the GuiDialogs "class" needs
 * to communicate with a Gui object, the latter is passed as an
 * argument to that public function. For an example, have a look
 * at the function: gui_dialogs_do_jumpto() which expects the Gui
 * object as an argument.
 *
 * Construction & initialization of a GuiDialogs object is done by
 * the caller in two separate steps. The initializer expects a valid
 * GtkBuilder as an argument, from which it initializes the internal
 * GTK+2 widgets of all the dialogs. Keep in mind that each dialog
 * has its own representation inside the "class" (in other words,
 * dialogs are NOT abstracted uniformly). 
 *
 ****************************************************************
 */

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

/* The GuiDialogs "class" consists of specialized abstractions
 * for every dialog used in the program. Each dialog abstraction
 * consists of a collection of GTK+2 widgets, needed for handling
 * ONLY that particular dialog. The first widget of all dialog
 * abstractions is called "root" and reflects the root window of
 * that particular dialog.
 *
 * The class is exposed publicly as an opaque data-type,
 * via the header file: "gui_dialogs.h").
 */
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
 * gboolean _on_activate_link_in_about_dialog:
 *
 * Callback function connected to the GTK+ "activate-link"
 * signal, which is emitted when a link button is clicked
 * inside the about-dialog. Return FALSE on error, TRUE
 * otherwise.
 *
 * NOTE: When run on Win32, GTK+2's file-engine (Gio) cannot
 *       open links, thus we overwrite the default behaviour
 *       by using this custom Win32 specific callback function.
 * ---------------------------------------------------
 */
static gboolean _on_activate_link_in_about_dialog(
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
 * (Private Initializer) gboolean _init_about_dialog():
 *
 * Initialize the about-dialog of the specified GuiDialogs
 * object (dialogs), according to the specified GtkBuilder
 * object (builder). Return FALSE on error, TRUE otherwise.
 *
 * NOTES:
 *
 *   The function first copies the about-dialog root widget
 *   from the builder into the dialogs->about abstraction.
 *   The widget in the builder must be of type GtkDialogAbout.
 *
 *   It then sets up the GtkDialogAbout properties of the
 *   widget (such as icon, logo, app name, app authors,
 *   etc) and connects callback functions to it.
 *
 *   The GTK+2 callback function: gtk_widget_destroyed()
 *   ensures that the widget pointer will be set to NULL
 *   after the widget gets destroyed.
 *
 *   The callback function: _on_activate_link_in_about_dialog()
 *   is conditionally connected to the "activate-signal"
 *   of the about-dialog, only when Windows is the compilation
 *   platform. That's because GTK+2's file-engine (Gio) seems
 *   to have trouble correctly registering the "http://" and
 *   the "mailto:" protocols under Win32.
 *
 *   On other platforms, gtk_show_uri() is set automatically
 *   by GTK+2 as the default callback function for the
 *   "activate-link" signal.
 *
 * TODO:
 *
 *   The GtkAboutDialog widget provides a standardized 
 *   way of presenting about-dialogs. However, Glade
 *   does not seem to allow direct access to the strings
 *   in its action-area, namely the buttons: "Credits",
 *   "License" and "Close".
 *
 *   Consequently, I cannot mark the labels of those
 *   buttons as translatable for the GNU-gettext
 *   library, from within Glade.
 *
 *   I must dig a bit deeper into the GtkAboutDialog
 *   internals. If that doesn't help, then most probably
 *   I will re-implement the about-dialog as a normal
 *   GtkDialog widget.
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
		G_CALLBACK( _on_activate_link_in_about_dialog ),
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
 * (Private Initializer) gboolean _init_jumpto_dialog():
 *
 * Initialize the jumpto-dialog of the specified GuiDialogs
 * object (dialogs), according to the specified GtkBuilder
 * object (builder). Return FALSE on error, TRUE otherwise.
 *
 * NOTES:
 *
 *   The function copies the jumpto-dialog widgets from
 *   the builder into the dialogs->jumpto abstraction,
 *   sets up an icon and the position of the dialog,
 *   and connects callback functions to it.
 *
 *   The GTK+ callback function: gtk_widget_destroyed()
 *   ensures that the pointer of the connected widget
 *   will be set to NULL after the widget gets destroyed.
 * ---------------------------------------------------
 */
static inline gboolean _init_jumpto_dialog(
	GuiDialogs  *dialogs,
	GtkBuilder  *builder
	)
{
	/* just for brevity later on */
	GtkWidget **pw = NULL;        /* pointer to a widget pointer */

	/* load the root widget of the dialog */
	pw  = &dialogs->jumpto.root;
	*pw = GTK_WIDGET( gtk_builder_get_object(builder, "dlgJumpto") );

	/* set dialog's initial position, icon and opacity */
	gtk_window_set_position( GTK_WINDOW(*pw), GTK_WIN_POS_CENTER );
	gtk_window_set_icon_from_file( GTK_WINDOW(*pw), FNAME_APPICON, NULL );
	gtk_window_set_opacity( GTK_WINDOW(*pw), 0.90 );

	/* connect callback functions to the root widget */
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
 * (Destructor) GuiDialogs *gui_dialogs_free():
 *
 * Release the memory reserved for the specified GuiDialogs
 * object (dialogs) and return NULL (so the caller nay assign
 * it back to the object pointer).
 *
 * NOTE: A GuiDialogs object abstracts ALL the dialogs used
 *       by the program.
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
 * (Constructor) GuiDialogs *make_gui_dialogs():
 *
 * Reserve zero'ed memory for a GuiDialogs object
 * and return a pointer to it, or NULL on error.
 *
 * NOTES:
 *
 *   On success, the constructed object is zero'ed,
 *   so it's up to the caller to initialize it with
 *   proper values. Currently, this can be done with
 *   the function: gui_dialogs_init_from_builder().
 *
 *   On another note, just remember that a GuiDialogs
 *   object abstracts ALL the dialogs used by the program.
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
 * (Initializer) gboolean gui_dialogs_init_from_builder():
 *
 * Initialize the specified GuiDialogs object (dialogs),
 * according to the specified GtkBuilder object (builder).
 * Return FALSE on error, TRUE otherwise.
 *
 * NOTES:
 *
 *   A GuiDialogs object abstracts ALL the dialogs used by
 *   the program. The individual initializations are done
 *   via calls to corresponding private functions.
 *
 *   The specified Gui object argument, does NOT take part
 *   in the initialization process of the dialogs. It is
 *   used only for displaying error-messages in alert-boxes,
 *   if needed.
 * ---------------------------------------------------
 */
gboolean gui_dialogs_init_from_builder(
	GuiDialogs  *dialogs,
	GtkBuilder  *builder,
	Gui         *gui
	)
{
	/* sanity checks */
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
 * (Getter) GtkWidget *gui_dialogs_get_about_root():
 *
 * Return a pointer to the root widget of the about-dialog,
 * stored in the specified GuiDialogs object, or NULL on
 * error.
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
 * (Getter) GtkWidget *gui_dialogs_get_jumpto_root():
 *
 * Return a pointer to the root widget of the jumpto-dialog,
 * stored in the specified GuiDialogs object, or NULL on
 * error.
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
 * (Getter) GtkWidget *gui_dialogs_get_jumpto_lblRange():
 *
 * Return a pointer to the range label-widget of the
 * jumpto-dialog, stored in the specified GuiDialogs
 * object, or NULL on error.
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
 * (Getter) GtkWidget *gui_dialogs_get_jumpto_lblCurrent():
 *
 * Return a pointer to the current-move label-widget of
 * the jumpto-dialog, stored in the specified GuiDialogs
 * object, or NULL on error.
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
 * (Getter) GtkWidget *gui_dialogs_get_jumpto_te():
 *
 * Return a pointer to the  text-entry widget of the
 * jumpto-dialog, stored in the specified GuiDialogs
 * object, or NULL on error.
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
 * gboolean gui_dialogs_do_jumpto():
 *
 * Open the jumpto-dialog of the specified GuiDialogs object
 * (dialogs), using the specified Gui object (gui) for setting
 * the contents to be displayed inside the dialog, and for
 * getting back the user input.
 *
 * NOTES:
 *
 *   The GuiDialogs object is not allowed to access directly the
 *   data it needs to display inside the dialog. Likewise, it is
 *   not allowed to apply the data read from the user.
 *
 *   Both the above tasks, are done via getters & setters of the
 *   Gui object.
 * ---------------------------------------------------
 */
gboolean gui_dialogs_do_jumpto( GuiDialogs *dialogs, Gui *gui )
{
//	extern int errno;

	gboolean ret    = TRUE;          /* function's return value */
	GtkWidget *appWindow = NULL;     /* application's main window */
	GtkWidget *root = NULL;          /* dialog's root widget */
	GtkEntry  *te   = NULL;          /* dialog's inner text-entry widget */
	gint response   = GUI_RESPONSE_OK;
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
			gui_alert_box(appWindow, _(TXT_ERR_INVALID_JUMPTO));
			ret = FALSE;
		}
	}

	/* hide the dialog window */
	gtk_widget_hide( GTK_WIDGET(root) );

	gtk_widget_set_sensitive( appWindow, TRUE );

	return ret;
}
