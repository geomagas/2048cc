/****************************************************************
 * This file is part of the "2048cc GTK+2 Replayer".
 *
 * Copyright:    2014 (c) migf1 <mig_f1@hotmail.com>
 * License:      Free Software (see comments in main.c for limitations)
 * Dependencies: misc.h, text.h gui.h gui_statusbar.h
 * --------------------------------------------------------------
 *
 * Private implementation of the GuiStatusbar "class". The accompanying
 * header file "gui_statusbar.h" exposes publicly the "class" as an opaque
 * data-type.
 *
 * The status-bar displays the following pieces of information, related
 * to the currently loaded replay-file:
 *
 *   - the file-name (shortened to its 15 last characters)
 *   - the total count of recorded moves
 *   - the dimensions of the square board
 *   - the value of the winning tile (sentinel tile value)
 *   - the count of randomly spawned tiles after every successful move
 *   - the delay between moves during playback (in milliseconds)
 *   - whether the games has been won or not
 *
 * A GuiStatusbar object does not have direct access to those pieces of
 * information. It gets them (via getters) from the Gui object that is
 * initially passed as an argument to the initializer of the GuiStatusbar
 * object (function: gui_statusbar_init_from_builder() ). The initializer
 * calls, in turn, the function: gui_statusbar_refresh(), passing to it
 * the Gui object.
 *
 * Thus, the update of the required info is actually done in the function
 * gui_statusbar_refresh().
 *
 * As a side note, the Gui object does not have direct access to the
 * requested pieces of info, either. It gets them from a Gamedata object
 * associated with it (see files: "gamedata.c", "gamedata.h", "gui.c"
 * and "gui.h").
 *
 * Finally, the formatted-output-template for the text to be displayed
 * in the status-bar, is the TXTF_STATUSBAR string literal, defined in
 * the file: "text.h"
 ****************************************************************
 */

#define GUI_STATUSBAR_C

#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

#include "misc.h"
#include "text.h"
#include "gui_statusbar.h"
#include "gui.h"

/* The GuiStatusbar "class" consists of a GTK+2 widget, a
 * context and a message id (required for GTK+2 status-bars)
 * and a c-string used for outputting the desired text in
 * the status-bar.
 *
 * The class is exposed publicly as an opaque data-type,
 * via the header file: "gui_statusbar.h").
 */
struct _GuiStatusbar
{
	GtkWidget  *widget;
	guint      contextId;
	guint      currMessageId;     /* will init this, but won't use it */
	char txtout[SZMAX_DBGMSG];    /* actual text to be outputted */
};

/* ---------------------------------------------------
 * gboolean gui_statusbar_refresh():
 *
 * Update the text of the specified Guistatusbar object (gsb)
 * and display the updated widget of the object.
 *
 * NOTES:
 *
 *   As explained in the opening comments at the top of this file,
 *   this function uses the specified Gui object argument (gui)
 *   for getting the desired Gamedata information to be displayed.
 *
 *   The filename of the currently loaded replay-file, is not used
 *   directly. A local copy of it, truncated down to the last 15
 *   characters, is used instead (via the function: s_new_shortfname(),
 *   defined in the file: "misc.c"). Otherwise the remaining info
 *   would NOT fit into the status-bar.
 * ---------------------------------------------------
 */
gboolean gui_statusbar_refresh( GuiStatusbar *gsb, const Gui *gui )
{
	const char *fname = NULL;
	int gamewon = 0;
	int dim     = 0;

	/* sanity checks */
	if ( NULL == gui ) {
		DBG_STDERR_MSG( "NULL pointer argument (gui)" );
		return FALSE;
	}
	if ( NULL == gsb ) {
		DBG_GUI_ERRMSG(
			gui_get_appWindow( gui ),
			"NULL pointer argument (gsb)"
			);
		return FALSE;
	}

	/* get need info from the Gui object */
	fname   = gui_get_gamedata_fname( gui );
	dim     = gui_get_gamedata_dim( gui );
	gamewon = gui_get_gamedata_gamewon( gui );

	/* update text to be outputted */
	memset( gsb->txtout, 0, SZMAX_DBGMSG );
	char *fn = s_new_shortfname( fname );
	g_snprintf(
		gsb->txtout,
		SZMAX_DBGMSG,
		_( TXTF_STATUSBAR ),                /* see file: "text.h" */
		( NULL == fname || '\0' == *fname ) /* name of loaded file */
			? "-"
			: fn,
		gui_get_gamedata_nmoves( gui ),     /* total moves */
		dim, dim,                           /* board size  */
		gui_get_gamedata_sentinel( gui ),   /* sentinel tile value */
		gui_get_gamedata_nrandom( gui ),    /* spawned tiles */
		gui_get_delay( gui ),               /* playback delay */
		gamewon ? _(TXT_YES) : _(TXT_NO)    /* won game? */
		);
	free( fn );

	/* udpate status-bar widget and display it */
	gtk_statusbar_push(
		GTK_STATUSBAR( gsb->widget ),
		gsb->contextId,
		gsb->txtout
		);
	gtk_widget_set_sensitive(
		gsb->widget,
		gui_get_hasloadedfile( gui )
			? TRUE
			: FALSE
		);

	return TRUE;
}

/* ---------------------------------------------------
 * (Initializer) gboolean gui_statusbar_init_from_builder():
 *
 * Initialize the specified GuiStatusbar object (gsb), according
 * the specified GtkBuilder object (builder) and the specified
 * Gui object (gui). Return FALSE on error, TRUE otherwise.
 *
 * NOTES:
 *
 *   The function copies the status-bar widget from the GTK+2
 *   builder into the GuiStatusbar object, connects callback
 *   functions to the widget, and initializes the remaining
 *   fields of the object (see struct _GuiStatusbar at the
 *   top of this file).
 *
 *   The initialization (and displaying) of the text to be
 *   outputted in the status-bar is actuyally handled in the
 *   function: gui_statusbar_refresh()).
 *
 *   The GTK+2 callback function: gtk_widget_destroyed()
 *   ensures that the widget pointer will be set to NULL
 *   after the widget gets destroyed.
 * ---------------------------------------------------
 */
gboolean gui_statusbar_init_from_builder(
	GuiStatusbar *gsb,
	GtkBuilder   *builder,
	const Gui    *gui
	)
{
	/* sanity checks */
	if ( NULL == gsb || NULL == builder || NULL == gui ) {
		DBG_GUI_ERRMSG(
			NULL,
			"NULL pointer argument (gsb || builder || gui)"
			);
		return FALSE;
	}

	/* init the status-bar widget */
	gsb->widget = GTK_WIDGET(gtk_builder_get_object(builder, "statusbar"));
//	gtk_widget_set_sensitive( gsb->widget, FALSE );
	g_signal_connect(
		G_OBJECT( gsb->widget ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		&gsb->widget
		);

	/* init the context & message ids */
	gsb->contextId =
		gtk_statusbar_get_context_id(
			GTK_STATUSBAR( gsb->widget ),
			"Info"
			);
	gsb->currMessageId = 0;     /* we are not gonna use this */

	/* init & display the text of the status-bar */
	gui_statusbar_refresh( gsb, gui );

	return TRUE;
}

/* ---------------------------------------------------
 * (Destructor) GuiStatusbar *gui_statusbar_free():
 *
 * Release the memory reserved for the specified GuiStatusbar
 * object (gsb) and return NULL (so the caller nay assign it
 * back to the object pointer).
 * ---------------------------------------------------
 */
GuiStatusbar *gui_statusbar_free( GuiStatusbar *gsb )
{
	if ( gsb ) {
		free( gsb );
	}
	return NULL;
}

/* ---------------------------------------------------
 * (Constructor) GuiStatusbar *make_gui_statusbar():
 *
 * Reserve zero'ed memory for a GuiStatusbar object
 * and return a pointer to it, or NULL on error.
 *
 * NOTE: On success, the constructed object is zero'ed,
 *       so it's up to the caller to initialize it with
 *       proper values. Currently, this can be done with
 *       the function: gui_statusbar_init_from_builder().
 * ---------------------------------------------------
 */
GuiStatusbar *make_gui_statusbar( void )
{
	GuiStatusbar *gsb = calloc( 1, sizeof(*gsb) );
	if ( NULL == gsb ) {
		DBG_GUI_ERRMSG( NULL, "calloc() failed!");
		return NULL;
	}
	return gsb;
}

#if 0
/* ---------------------------------------------------
 * Currently Unused
 * ---------------------------------------------------
 */
GtkWidget *gui_statusbar_get_widget( GuiStatusbar *gsb )
{
	if ( NULL == gsb ) {
		DBG_GUI_ERRMSG( NULL, "NULL pointer argument (gsb)!");
		return NULL;
	}
	return gsb->widget;
}

/* ---------------------------------------------------
 * Currently Unused
 * ---------------------------------------------------
 */
guint gui_statusbar_get_contextId( GuiStatusbar *gsb )
{
	if ( NULL == gsb ) {
		DBG_GUI_ERRMSG( NULL, "NULL pointer argument (gsb)!");
		return G_MININT;
	}
	return gsb->contextId;
}
#endif /* #if 0 */