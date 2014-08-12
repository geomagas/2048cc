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
	_TileColors tile4096;
	_TileColors tile8192;
	_TileColors tile16384;
	_TileColors tile32768;
	_TileColors tile65536;
	_TileColors tilebig;
} _BoardSkin;

struct _GuiBoard {
	GtkWidget *ebContainer; /* event-box parent of table-widget */
	GtkWidget *table;       /* child of ebContainer */
	struct {
		GtkWidget *evbox;                 /* container widget */
		GtkWidget *label;                 /* label widget */
		char       ebname[SZMAX_DBGMSG];  /* name of evbox-widget */
		char       lblname[SZMAX_DBGMSG]; /* name of label-widget */
	} *tiles;

	_BoardSkin skin;
};

gboolean gui_board_refresh( GuiBoard *gb, Gui *gui );

/* ---------------------------------------------------
 *
 * ---------------------------------------------------
 */
static inline guint _dim_to_border_width( guint dim )
{
	switch( dim ) {
		case 4:
			return 14;
		case 5:
			return 12;
		case 6:
			return 10;
		case 8:
			return 4;
		default:
			break;
	}
	return 14;
}

/* ---------------------------------------------------
 *
 * ---------------------------------------------------
 */
static inline int  _dim_and_tileval_to_font_markupsize(
	guint    dim,
	long int tileval
	)
{
	int ret   = 24;         /* default */
	int ndigs = int_count_digits( tileval );

	switch( dim ) {
		case 4:
			switch( ndigs ) {
			case 1:
				return 30;
			case 2:
				return 28;
			case 3:
				return 26;
			case 4:
				return 24;
			case 5:
				return 18;
			default:
				return 14;
			}
			break;

		case 5:
			switch( ndigs ) {
			case 1:
				return 28;
			case 2:
				return 26;
			case 3:
				return 21;
			case 4:
				return 19;
			case 5:
				return 14;
			default:
				return 12;
			}
			break;

		case 6:
			switch( ndigs ) {
			case 1:
				return 24;
			case 2:
				return 22;
			case 3:
				return 18;
			case 4:
				return 15;
			case 5:
				return 12;
			default:
				return 9;
			}
			break;
		case 8:
			switch( ndigs ) {
			case 1:
				return 19;
			case 2:
				return 17;
			case 3:
				return 14;
			case 4:
				return 12;
			case 5:
				return 10;
			default:
				return 8;
			}
			break;
		default:
			break;
	}
	return ret;
}

/* ---------------------------------------------------
 *
 * ---------------------------------------------------
 */
static inline const _TileColors *_tileval_to_tilecolors(
	long int          tileval,
	const _BoardSkin  *skin
	)
{
	switch ( tileval )
	{
		case 0:
			return &skin->tile0;
		case 2:
			return &skin->tile2;
		case 4:
			return &skin->tile4;
		case 8:
			return &skin->tile8;
		case 16:
			return &skin->tile16;
		case 32:
			return &skin->tile32;
		case 64:
			return &skin->tile64;
		case 128:
			return &skin->tile128;
		case 256:
			return &skin->tile256;
		case 512:
			return &skin->tile512;
		case 1024:
			return &skin->tile1024;
		case 2048:
			return &skin->tile2048;
		case 4096:
			return &skin->tile4096;
		case 8192:
			return &skin->tile8192;
		case 16384:
			return &skin->tile16384;
		case 32768:
			return &skin->tile32768;
		case 65536:
			return &skin->tile65536;
		default:
			return &skin->tilebig;
	}

	return NULL;
}

/* ---------------------------------------------------
 *
 * ---------------------------------------------------
 */
static void _init_skin( GuiBoard *gb  )
{
	_BoardSkin *p = &gb->skin;

	/* board container */
	gdk_color_parse( "#bbada0", &p->bgBoardContainer );

	/* tile0 */
	gdk_color_parse( "#cdc0b4", &p->tile0.fg );
	gdk_color_parse( "#cdc0b4", &p->tile0.bg );

	/* tile2 */
	gdk_color_parse( "#776e65", &p->tile2.fg );
	gdk_color_parse( "#eee4da", &p->tile2.bg );

	/* tile4 */
	gdk_color_parse( "#776e65", &p->tile4.fg );
	gdk_color_parse( "#ede0c8", &p->tile4.bg );

	/* tile8 */
	gdk_color_parse( "#f9f6f2", &p->tile8.fg );
	gdk_color_parse( "#f2b179", &p->tile8.bg );

	/* tile16 */
	gdk_color_parse( "#f9f6f2", &p->tile16.fg );
	gdk_color_parse( "#f59563", &p->tile16.bg );

	/* tile32 */
	gdk_color_parse( "#f9f6f2", &p->tile32.fg );
	gdk_color_parse( "#f67c5f", &p->tile32.bg );

	/* tile64 */
	gdk_color_parse( "#f9f6f2", &p->tile64.fg );
	gdk_color_parse( "#f65e3b", &p->tile64.bg );

	/* tile128 */
	gdk_color_parse( "#f9f6f2", &p->tile128.fg );
	gdk_color_parse( "#edcf72", &p->tile128.bg );

	/* tile256 */
	gdk_color_parse( "#f9f6f2", &p->tile256.fg );
	gdk_color_parse( "#edcc61", &p->tile256.bg );

	/* tile512 */
	gdk_color_parse( "#f9f6f2", &p->tile512.fg );
	gdk_color_parse( "#edc952", &p->tile512.bg );

	/* tile1024 */
	gdk_color_parse( "#f9f6f2", &p->tile1024.fg );
	gdk_color_parse( "#edc53f", &p->tile1024.bg );

	/* tile2048 */
	gdk_color_parse( "#f9f6f2", &p->tile2048.fg );
	gdk_color_parse( "#edc22e", &p->tile2048.bg );

	/* tile4096 */
	gdk_color_parse( "#f9f6f2", &p->tile4096.fg );
	gdk_color_parse( "#77a136", &p->tile4096.bg );

	/* tile8192 */
	gdk_color_parse( "#f9f6f2", &p->tile8192.fg );
	gdk_color_parse( "#2db388", &p->tile8192.bg );

	/* tile16384 */
	gdk_color_parse( "#f9f6f2", &p->tile16384.fg );
	gdk_color_parse( "#2693FF", &p->tile16384.bg );

	/* tile32768 */
	gdk_color_parse( "#f9f6f2", &p->tile32768.fg );
	gdk_color_parse( "#007FFF", &p->tile32768.bg );

	/* tile65536 */
	gdk_color_parse( "#f9f6f2", &p->tile65536.fg );
	gdk_color_parse( "#8C008C", &p->tile65536.bg );

	/* tilebig */
	gdk_color_parse( "#f9f6f2", &p->tilebig.fg );
	gdk_color_parse( "#000000", &p->tilebig.bg );
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
	const Gui *gui
	)
{
	int i,j, n;

	free( gb->tiles );

	gb->tiles = calloc( dim * dim, sizeof( *(gb->tiles) ) );
	if ( NULL == gb->tiles ) {
		DBG_GUI_ERRMSG(
			gui_get_appWindow( gui ),
			"calloc(tiles) failed!"
			);
		return FALSE;
	}

//	dbg_gamedata_print_tiles( gui_get_gamedata(gui) );

	for (i=0; i < dim; i++) {
		for (j=0; j < dim; j++) {
			n = i * dim + j;

			/* create tile's container */
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

			/* create tile's label & add it to its container */
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
			gtk_container_add(
				GTK_CONTAINER( gb->tiles[n].evbox ),
				gb->tiles[n].label
				);

			/* add tile into the table */
			gtk_table_attach_defaults( 
				GTK_TABLE( gb->table ),
				gb->tiles[n].evbox,
				j,    /* left_attach column */
				j+1,  /* right_attach column */
				i,    /* top_attach row */
				i+1   /* bottom_attach row */
				);
		}
	}

	return TRUE;
}

/* ---------------------------------------------------
 *
 * ---------------------------------------------------
 */
static inline void _draw_tile(
	GuiBoard         *gb,
	long int          imove,
	int               dim,
	int               itile,
	const Gui         *gui
	)
{
	long int tileval = 0;
	char     tilemarkup[SZMAX_DBGMSG];
	const    _TileColors *tilecolors;

	if ( !gui_is_imove_valid(gui) ) {
		DBG_GUI_ERRMSG(
			gui_get_appWindow( gui ),
			"gui contains and invalid imove!"
			);
		return;
	}
	if ( imove > -1 ) {
		tileval = gui_get_gamedata_tile_of_move(
				gui,
				itile,
				imove
				);
	}
	g_snprintf(
		tilemarkup,
		SZMAX_DBGMSG,
		"<span font=\"%d\" font_weight=\"bold\">%ld</span>",
		_dim_and_tileval_to_font_markupsize( dim, tileval ),
		tileval
		);

	tilecolors = _tileval_to_tilecolors( tileval, &gb->skin );

	gtk_label_set_markup(
		GTK_LABEL( gb->tiles[itile].label ),
		tilemarkup
		);
	gtk_widget_modify_bg(
		GTK_WIDGET( gb->tiles[itile].evbox ),
		GTK_STATE_NORMAL,
		&tilecolors->bg
		);
	gtk_widget_modify_fg(
		GTK_WIDGET( gb->tiles[itile].label ),
		GTK_STATE_NORMAL,
		&tilecolors->fg
		);
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

	_init_skin( gb );

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
	int dim;

	if ( NULL == gb || NULL == builder || NULL == gui ) {
		DBG_GUI_ERRMSG(
			gui_get_appWindow( gui ),
			"NULL pointer argument (gsb || builder || gui)"
			);
		return FALSE;
	}

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
		&gb->skin.bgBoardContainer
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
	if ( !_create_tiles(gb, dim, gui) ) {
		DBG_GUI_ERRMSG(
			gui_get_appWindow( gui ),
			"_create_tiles() failed!"
			);
		return FALSE;
	}

	gui_board_refresh( gb, gui );

	return TRUE;
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
gboolean gui_board_resize( GuiBoard *gb, Gui *gui )
{
	guint curdim = 0;
	int   dim = 0;

	/* sanity checks */
	if ( NULL == gui ) {
		DBG_STDERR_MSG( "NULL pointer argument (gui)" );
		return FALSE;
	}
	if ( NULL == gb ) {
		DBG_GUI_ERRMSG(
			gui_get_appWindow( gui ),
			"NULL pointer argument (gb)"
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

	/* resize board according to gui->gamedata.dim */
	dim = gui_get_gamedata_dim( gui );
	if ( dim < 4 ) {
		DBG_GUI_ERRMSG(
			gui_get_appWindow( gui ),
			"gui->gamedata.dim < 4!"
			);
		return FALSE;
	}
	gtk_table_resize( GTK_TABLE(gb->table), dim, dim );

	/* re-style resized board */
	gtk_container_set_border_width(
		GTK_CONTAINER( gb->table ),
		_dim_to_border_width( dim )
		);
	gtk_table_set_row_spacings(
		GTK_TABLE( gb->table ),
		_dim_to_border_width( dim )
		);
	gtk_table_set_col_spacings(
		GTK_TABLE( gb->table ),
		_dim_to_border_width( dim )
		);

	/* create new tiles in the resized board */
	if ( !_create_tiles(gb, dim, gui) ) {
		DBG_GUI_ERRMSG(
			gui_get_appWindow( gui ),
			"_create_tiles() failed!"
			);
		return FALSE;
	}

	return TRUE;
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
gboolean gui_board_refresh( GuiBoard *gb, Gui *gui )
{
	int dim, i,j, n;

	/* sanity checks */
	if ( NULL == gui ) {
		DBG_STDERR_MSG( "NULL pointer argument (gui)" );
		return FALSE;
	}
	if ( NULL == gb ) {
		DBG_GUI_ERRMSG(
			gui_get_appWindow( gui ),
			"NULL pointer argument (gb)"
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

	gtk_table_get_size(
		GTK_TABLE( gb->table ),
                (guint *) &dim,
                (guint *) &dim
		);

	for (i=0; i < dim; i++) {
		for (j=0; j < dim; j++) {
			n = i * dim + j;
			if ( gui_is_imove_valid(gui) ) {
				_draw_tile(gb, gui_get_imove(gui), dim, n, gui);
			}
		}
	}

	gtk_widget_show_all( gb->ebContainer /* gui_get_appWindow( gui ) */ );

	return TRUE;
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
GtkWidget *gui_board_get_container( GuiBoard *gb, const Gui *gui )
{
	if ( NULL == gui ) {
		DBG_STDERR_MSG( "NULL pointer argument (gb)" );
		return NULL;
	}
	if ( NULL == gb ) {
		DBG_GUI_ERRMSG(
			gui_get_appWindow( gui ),
			"NULL pointer argument (gb)"
			);
		return NULL;
	}

	return gb->ebContainer;
}
