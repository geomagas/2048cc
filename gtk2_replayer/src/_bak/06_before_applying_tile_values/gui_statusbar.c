#define GUI_STATUSBAR_C

#include <gtk/gtk.h>
#include <stdio.h>    /* just for debugging messages in the console */
#include <stdlib.h>
#include <string.h>

#include "misc.h"
#include "text.h"
#include "gui_statusbar.h"
#include "gui.h"

/* a bit more convenient GUI abstraction of the status-bar */
struct _GuiStatusbar
{
	/* gui related */
	GtkWidget  *widget;
	guint      contextId;
	guint      currMessageId;
	char txtout[SZMAX_DBGMSG];       /* actual text to be outputted */
};


/* ---------------------------------------------------
 *
 * ---------------------------------------------------
 */
gboolean gui_statusbar_refresh_txtout( GuiStatusbar *gsb, const Gui *gui )
{
	const char *fname = NULL;
	int gamewon = 0;
	int dim     = 0;

	if ( NULL == gsb || NULL == gui ) {
		DBG_GUI_ERRMSG(
			NULL,
			"NULL pointer argument (gsb || gui)"
			);
		return FALSE;
	}

	memset( gsb->txtout, 0, SZMAX_DBGMSG );

	fname   = gui_get_gamedata_fname( gui );
	dim     = gui_get_gamedata_dim( gui );
	gamewon = gui_get_gamedata_gamewon( gui );

	char *fn = s_new_shortfname( fname );
	g_snprintf(
		gsb->txtout,
		SZMAX_DBGMSG,
		TXTF_STATUSBAR,
		( NULL == fname || '\0' == *fname )   /* loaded file */
			? "-"
			: fn,
		gui_get_gamedata_nmoves(gui),         /* total moves */
		dim, dim,                             /* board size  */
		gui_get_gamedata_sentinel(gui),       /* sentinel tile value */
		gui_get_gamedata_nrandom(gui),        /* spawned tiles */
		gui_get_gamedata_delay(gui),          /* animation delay */
		gamewon ? "Yes" : "No"                /* won game? */
		);
	gtk_statusbar_push(
		GTK_STATUSBAR( gsb->widget ),
		gsb->contextId,
		gsb->txtout
		);
	free( fn );

	return TRUE;
}

/* ---------------------------------------------------
 * Initialize the status-bar of our main window.
 *
 * Copies the GUI status-bar from the specified GTK+ builder into
 * my GUI status-bar abstraction, it connects callback functions to
 * it, and initializes its visual appearance.
 *
 * My status-bar abstraction consists of a GTK+ statusbar widget,
 * and a couple of supporting fields for the context-id and the
 * text-message. We get the former from the builder and we
 * initialize & apply the latter.
 *
 * NOTE:
 *	The GTK+ callback function: gtk_widget_destroyed()
 *	ensures that the widget pointer will be set to NULL
 *	after the widget gets destroyed.
 * ---------------------------------------------------
 */
gboolean gui_statusbar_init_from_builder(
	GuiStatusbar *gsb,
	GtkBuilder   *builder,
	const Gui    *gui
	)
{
	if ( NULL == gsb || NULL == builder ) {
		DBG_GUI_ERRMSG(
			NULL,
			"NULL pointer argument (gsb || builder)"
			);
		return FALSE;
	}

	gsb->widget = GTK_WIDGET(gtk_builder_get_object(builder, "statusbar"));

	gtk_widget_set_sensitive( gsb->widget, FALSE );

	g_signal_connect(
		G_OBJECT( gsb->widget ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		&gsb->widget
		);

	gsb->contextId =
		gtk_statusbar_get_context_id(
			GTK_STATUSBAR( gsb->widget ),
			"Info"
			);

	gsb->currMessageId = 0;

	gui_statusbar_refresh_txtout( gsb, gui );

	return TRUE;
}

/* ---------------------------------------------------
 *
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
 *
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

/* ---------------------------------------------------
 *
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
 *
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
