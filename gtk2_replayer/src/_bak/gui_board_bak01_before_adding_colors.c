#define GUI_BOARD_C

#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>

#include "gui_board.h"
#include "gui.h"

#include "misc.h"

/* A tile is a label widget, contained in an event-box widget,
 * because label widgets do not have bg colors, and event-box
 * widgets do not have a fg color.
 */
typedef struct {
	GdkColor fg;        /* for the label widget */
	GdkColor bg;        /* for the even-box container */
	
} _TileColors;

typedef struct {
	GdkColor bgBoardContainer;

	_TileColors tile0;
	_TileColors tile2;
	_TileColors tile4;
	_TileColors tile8;
	_TileColors tile16;
	_TileColors tile32;
	_TileColors tile64;
	_TileColors tile128;
	_TileColors tile256;
	_TileColors tile512;
	_TileColors tile1024;
	_TileColors tile2048;
	_TileColors tilebig;

	_TileColors tilecolors[12];
} _Colors;

struct _GuiBoard {
	GtkWidget *ebContainer; /* event-box parent of table-widget */
	GtkWidget *table;       /* child of ebContainer */
//	int       dim;          /* single-dimension of the table */
	struct {
		GtkWidget *evbox;                 /* container widget */
		GtkWidget *label;                 /* label widget */
		char       ebname[SZMAX_DBGMSG];  /* name of evbox-widget */
		char       lblname[SZMAX_DBGMSG]; /* name of label-widget */
		GdkColor   bg;                    /* evbox's bg color */
		GdkColor   fg;                    /* label's fg color */
	} *tiles;

	_Colors *colors;
};

/* ---------------------------------------------------
 *
 * ---------------------------------------------------
 */
_Colors _new_colors( void )
{
	_Colors *colors = calloc(1, sizeof(colors) );
	if ( NULL == colors ) {
	}
}

/* ---------------------------------------------------
 *
 * ---------------------------------------------------
 */
static inline void _remove_tiles( GuiBoard *gb, int dim )
{
	int i,j, n;

	for (i=0; i < dim; i++) {
		for (j=0; j < dim; j++) {
			n = i * dim + j;

			gtk_widget_destroy( gb->tiles[n].evbox );
			memset( &gb->tiles[n].ebname,  0, SZMAX_DBGMSG );
			memset( &gb->tiles[n].lblname, 0, SZMAX_DBGMSG );
		}
	}
}

/* ---------------------------------------------------
 *
 * ---------------------------------------------------
 */
static inline gboolean _create_tiles(
	GuiBoard  *gb,
	int       dim,
	long int  imove,
	const Gui *gui
	)
{
	int i,j, n;
	char tilemarkup[SZMAX_DBGMSG] = {'\0'};
	GdkColor fgtile, bgtile;

	gdk_color_parse( "#776e65", &fgtile );
	gdk_color_parse( "#cdc0b4", &bgtile );

	free( gb->tiles );
	gb->tiles = calloc( dim * dim, sizeof( *(gb->tiles) ) );
	if ( NULL == gb->tiles ) {
		DBG_GUI_ERRMSG(
			gui_get_appWindow( gui ),
			"calloc(tiles) failed!"
			);
		return FALSE;
	}

//dbg_gamedata_print_tiles( gui_get_gamedata(gui) );

	for (i=0; i < dim; i++) {
		for (j=0; j < dim; j++) {
			n = i * dim + j;

int tileval = 0;
if ( -1 == imove
|| 0 == (tileval = gui_get_gamedata_tile_of_move(
			gui,
			n,
			imove
			)
	)
){
	g_snprintf(
		tilemarkup,
		SZMAX_DBGMSG,
		"<span font=\"18\" font_weight=\"bold\">%c</span>",
		' '
		);
}
else {
	g_snprintf(
		tilemarkup,
		SZMAX_DBGMSG,
		"<span font=\"18\" font_weight=\"bold\">%d</span>",
		tileval
		);
}
			gb->tiles[n].evbox = gtk_event_box_new();
			g_snprintf(
				gb->tiles[n].ebname,
				SZMAX_DBGMSG,
				"eb%d%d",
				i,j
				);
			gtk_widget_set_name(
				gb->tiles[n].evbox,
				gb->tiles[n].ebname
				);

			gb->tiles[n].label = gtk_label_new( "" );
			g_snprintf(
				gb->tiles[n].lblname,
				SZMAX_DBGMSG,
				"lb%d%d",
				i,j
				);
			gtk_widget_set_name(
				gb->tiles[n].label,
				gb->tiles[n].lblname
				);

			gtk_label_set_markup(
				GTK_LABEL( gb->tiles[n].label ),
//				"<span font=\"18\" font_weight=\"bold\">0</span>"
				tilemarkup
				);
			gtk_container_add(
				GTK_CONTAINER( gb->tiles[n].evbox ),
				gb->tiles[n].label
				);

			gtk_table_attach_defaults( 
				GTK_TABLE( gb->table ),
				gb->tiles[n].evbox,
				j,    /* left_attach column */
				j+1,  /* right_attach column */
				i,    /* top_attach row */
				i+1   /* bottom_attach row */
				);
			gtk_widget_modify_bg(
				GTK_WIDGET( gb->tiles[n].evbox ),
				GTK_STATE_NORMAL,
				&bgtile
				);
			gtk_widget_modify_fg(
				GTK_WIDGET( gb->tiles[n].label ),
				GTK_STATE_NORMAL,
				&fgtile
				);
			memcpy(
				&gb->tiles[n].bg,
				&bgtile,
				sizeof( GdkColor )
				);
			memcpy(
				&gb->tiles[n].fg,
				&fgtile,
				sizeof( GdkColor )
				);
		}
	}

	gtk_widget_show_all( gui_get_appWindow( gui ) );
	
	return TRUE;
}

/* ---------------------------------------------------
 *
 * ---------------------------------------------------
 */
GuiBoard *gui_board_free( GuiBoard *gb )
{
	if ( gb ) {
		free( gb->tiles );
		free( gb );
	}
	return NULL;
}

/* ---------------------------------------------------
 *
 * ---------------------------------------------------
 */
GuiBoard *make_gui_board( void )
{
	GuiBoard *gb = calloc( 1, sizeof(*gb) );
	if ( NULL == gb ) {
		DBG_GUI_ERRMSG( NULL, "calloc() failed!");
		return NULL;
	}
	return gb;
}

/* ---------------------------------------------------
 * Initialize the board table from builder.
 *
 * NOTE:
 *	The GTK+ callback function: gtk_widget_destroyed()
 *	ensures that the widget pointer will be set to NULL
 *	after the widget gets destroyed.
 * ---------------------------------------------------
 */
gboolean gui_board_init_from_builder(
	GuiBoard   *gb,
	GtkBuilder *builder,
	Gui        *gui
	)
{
	int      dim;
	GdkColor bgcontainer;

	if ( NULL == gb || NULL == builder || NULL == gui ) {
		DBG_GUI_ERRMSG(
			gui_get_appWindow( gui ),
			"NULL pointer argument (gsb || builder || gui)"
			);
		return FALSE;
	}

	gdk_color_parse( "#bbada0", &bgcontainer );

	/* board container (event-box widget) */
	gb->ebContainer = GTK_WIDGET(
				gtk_builder_get_object(
					builder,
					"ebBoardContainer"
					)
				);
	gtk_widget_modify_bg(
		GTK_WIDGET( gb->ebContainer ),
		GTK_STATE_NORMAL,
		&bgcontainer
		);
	g_signal_connect(
		G_OBJECT( gb->ebContainer ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		&gb->ebContainer
		);

	/* board table (table-widget) */
	gb->table = GTK_WIDGET( gtk_builder_get_object(builder, "tableBoard") );
	g_signal_connect(
		G_OBJECT( gb->table ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		&gb->table
		);

	/* board tiles */
	gtk_table_get_size(
		GTK_TABLE( gb->table ),
                (guint *) &dim,
                (guint *) &dim
		);
	if ( !_create_tiles(gb, dim, -1, gui) ) {
		DBG_GUI_ERRMSG(
			gui_get_appWindow( gui ),
			"_create_tiles() failed!"
			);
		return FALSE;
	}

//	gb->dim = dim;
	return TRUE;
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
gboolean gui_board_resize(
	GuiBoard *gb,
	long int imove,
	Gui      *gui
	)
{
	guint curdim = 0;
	int   dim = 0;

	/* sanity check */
	if ( NULL == gb || NULL == gui ) {
		DBG_GUI_ERRMSG(
			NULL,
			"NULL pointer argument (gui)"
			);
		return FALSE;
	}

	/* demand non-NULL gui->gamedata */
	if ( NULL == gui_get_gamedata(gui) ) {
		DBG_GUI_ERRMSG(
			gui_get_appWindow( gui ),
			"gui->gamedata is NULL!"
			);
		return FALSE;
	}

	/* remove tiles from board */
	gtk_table_get_size(
		GTK_TABLE( gb->table ),
		&curdim,
		&curdim
		);
	_remove_tiles( gb, (int)curdim );

	/* resize board according to gui->gamedata.dim & create tiles */
	dim = gui_get_gamedata_dim( gui );
	if ( dim < 4 ) {
		DBG_GUI_ERRMSG(
			gui_get_appWindow( gui ),
			"gui->gamedata.dim < 4!"
			);
		return FALSE;
	}
	gtk_table_resize( GTK_TABLE(gb->table), dim, dim );

	if ( !_create_tiles(gb, dim, imove, gui) ) {
		DBG_GUI_ERRMSG(
			gui_get_appWindow( gui ),
			"_create_tiles() failed!"
			);
		return FALSE;
	}

//	gb->dim = dim;

	return TRUE;
}
